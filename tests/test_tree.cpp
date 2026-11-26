#include <gtest/gtest.h>

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include "tree.hpp"
#include "point.hpp"
#include "selection.hpp"
#include "criterion.hpp"
#include "first_quadrant_criterion.hpp"

namespace {

using Coordinates = std::pair<double, double>;

class TestAllCriterion : public Criterion {
public:
    bool matches(const Point&) const override {
        return true;
    }
};

void insertPoints(Tree& tree, std::initializer_list<Coordinates> points) {
    for (const auto& [x, y] : points) {
        ASSERT_TRUE(tree.insert(Point(x, y)));
    }
}

void expectSelection(const Selection& selection,
                     std::initializer_list<Coordinates> expected) {
    ASSERT_EQ(selection.size(), expected.size());
    std::size_t i = 0;
    for (const auto& [x, y] : expected) {
        auto point = selection[i++].lock();
        ASSERT_TRUE(point);
        EXPECT_DOUBLE_EQ(point->x(), x);
        EXPECT_DOUBLE_EQ(point->y(), y);
    }
}

bool isEmptyWeak(const std::weak_ptr<Point>& point) {
    std::weak_ptr<Point> empty;
    return !point.owner_before(empty) && !empty.owner_before(point);
}

class TreeTest : public testing::Test {
protected:
    void TearDown() override {
        EXPECT_EQ(Point::aliveCount(), 0);
    }
};

TEST_F(TreeTest, InsertIncreasesSize) {
    Tree tree;
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_TRUE(tree.insert(Point(2, 3)));
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_TRUE(tree.insert(Point(2, 4)));
    EXPECT_EQ(tree.size(), 2u);
}

TEST_F(TreeTest, DuplicateKeyIsRejected) {
    Tree tree;
    ASSERT_TRUE(tree.insert(Point(2, 3)));
    auto original = tree.find(Point(2, 3));
    ASSERT_FALSE(original.expired());
    EXPECT_FALSE(tree.insert(Point(2, 3)));
    EXPECT_EQ(tree.size(), 1u);
    EXPECT_EQ(original.lock(), tree.find(Point(2, 3)).lock());
}

TEST_F(TreeTest, FindReturnsActualStoredPoint) {
    Tree tree;
    Point exemplar(3, 4);
    ASSERT_TRUE(tree.insert(exemplar));
    auto found = tree.find(exemplar).lock();
    ASSERT_TRUE(found);
    EXPECT_NE(found.get(), &exemplar);
    EXPECT_DOUBLE_EQ(found->x(), 3);
    EXPECT_DOUBLE_EQ(found->y(), 4);
    EXPECT_EQ(found, tree.find(Point(3, 4)).lock());
    auto selection = tree.inorder();
    ASSERT_EQ(selection.size(), 1u);
    EXPECT_EQ(found, selection[0].lock());
}

TEST_F(TreeTest, MissingKeyReturnsEmptyWeakPointer) {
    Tree tree;
    EXPECT_TRUE(isEmptyWeak(tree.find(Point(1, 1))));
    ASSERT_TRUE(tree.insert(Point(2, 2)));
    EXPECT_TRUE(isEmptyWeak(tree.find(Point(2, 3))));
    EXPECT_TRUE(tree.find(Point(2, 3)).expired());
}

TEST_F(TreeTest, FindAfterRemovalReturnsEmptyWeakPointer) {
    Tree tree;
    ASSERT_TRUE(tree.insert(Point(1, 2)));
    auto old = tree.find(Point(1, 2));
    ASSERT_FALSE(old.expired());
    ASSERT_TRUE(tree.remove(Point(1, 2)));
    EXPECT_TRUE(old.expired());
    EXPECT_TRUE(isEmptyWeak(tree.find(Point(1, 2))));
}

TEST_F(TreeTest, HeightOnEmptyLeafAndMultilevelTree) {
    Tree tree;
    EXPECT_EQ(tree.height(), 0u);
    ASSERT_TRUE(tree.insert(Point(4, 0)));
    EXPECT_EQ(tree.height(), 1u);
    ASSERT_TRUE(tree.insert(Point(2, 0)));
    EXPECT_EQ(tree.height(), 2u);
    insertPoints(tree, {{6, 0}, {1, 0}, {0, 0}});
    EXPECT_EQ(tree.height(), 4u);
}

TEST_F(TreeTest, InorderIsLexicographicallyAscending) {
    Tree tree;
    insertPoints(tree, {{2, 3}, {1, 9}, {2, -1}, {3, 0}, {1, 2}});
    expectSelection(tree.inorder(), {{1, 2}, {1, 9}, {2, -1}, {2, 3}, {3, 0}});
}

TEST_F(TreeTest, ReverseInorderIsLexicographicallyDescending) {
    Tree tree;
    insertPoints(tree, {{2, 3}, {1, 9}, {2, -1}, {3, 0}, {1, 2}});
    expectSelection(tree.reverseInorder(), {{3, 0}, {2, 3}, {2, -1}, {1, 9}, {1, 2}});
}

TEST_F(TreeTest, RemoveLeafPreservesBST) {
    Tree tree;
    insertPoints(tree, {{4, 0}, {2, 0}, {6, 0}});
    ASSERT_TRUE(tree.remove(Point(2, 0)));
    EXPECT_EQ(tree.size(), 2u);
    EXPECT_TRUE(tree.find(Point(2, 0)).expired());
    expectSelection(tree.inorder(), {{4, 0}, {6, 0}});
}

TEST_F(TreeTest, RemoveNodeWithOnlyLeftChildPreservesSubtree) {
    Tree tree;
    insertPoints(tree, {{5, 0}, {3, 0}, {2, 0}, {1, 0}, {7, 0}});
    ASSERT_TRUE(tree.remove(Point(3, 0)));
    EXPECT_EQ(tree.size(), 4u);
    EXPECT_TRUE(tree.find(Point(3, 0)).expired());
    expectSelection(tree.inorder(), {{1, 0}, {2, 0}, {5, 0}, {7, 0}});
}

TEST_F(TreeTest, RemoveNodeWithOnlyRightChildPreservesSubtree) {
    Tree tree;
    insertPoints(tree, {{5, 0}, {3, 0}, {4, 0}, {4, 1}, {7, 0}});
    ASSERT_TRUE(tree.remove(Point(3, 0)));
    EXPECT_EQ(tree.size(), 4u);
    EXPECT_TRUE(tree.find(Point(3, 0)).expired());
    expectSelection(tree.inorder(), {{4, 0}, {4, 1}, {5, 0}, {7, 0}});
}

TEST_F(TreeTest, RemoveTwoChildRootPreservesBSTAndRecordIdentity) {
    Tree tree;
    insertPoints(tree, {{5, 0}, {3, 0}, {9, 0}, {6, 0}, {7, 0}, {4, 0}, {10, 0}});
    auto removed = tree.find(Point(5, 0));
    auto successor = tree.find(Point(6, 0));
    auto predecessor = tree.find(Point(4, 0));
    ASSERT_FALSE(removed.expired());
    ASSERT_FALSE(successor.expired());
    ASSERT_FALSE(predecessor.expired());
    ASSERT_TRUE(tree.remove(Point(5, 0)));
    EXPECT_EQ(tree.size(), 6u);
    EXPECT_TRUE(removed.expired());
    EXPECT_FALSE(successor.expired());
    EXPECT_FALSE(predecessor.expired());
    EXPECT_EQ(successor.lock(), tree.find(Point(6, 0)).lock());
    EXPECT_EQ(predecessor.lock(), tree.find(Point(4, 0)).lock());
    expectSelection(tree.inorder(), {{3, 0}, {4, 0}, {6, 0}, {7, 0}, {9, 0}, {10, 0}});
}

TEST_F(TreeTest, RemoveMissingKeyDoesNotChangeTree) {
    Tree tree;
    EXPECT_FALSE(tree.remove(Point(1, 0)));
    insertPoints(tree, {{2, 0}, {1, 0}, {3, 0}});
    EXPECT_FALSE(tree.remove(Point(2, 1)));
    EXPECT_EQ(tree.size(), 3u);
    expectSelection(tree.inorder(), {{1, 0}, {2, 0}, {3, 0}});
}

TEST_F(TreeTest, FirstQuadrantSelectionHasCorrectOrderAndBoundaries) {
    Tree tree;
    FirstQuadrantCriterion criterion;
    insertPoints(tree, {{2, 3}, {-1, 4}, {0, 1}, {1, 0}, {1, 2}, {2, -3}, {2, 1}, {-1, -1}});
    expectSelection(tree.select(criterion), {{1, 2}, {2, 1}, {2, 3}});
}

TEST_F(TreeTest, CriterionWithoutMatchesReturnsEmptySelection) {
    Tree tree;
    FirstQuadrantCriterion criterion;
    EXPECT_TRUE(tree.select(criterion).empty());
    insertPoints(tree, {{0, 1}, {1, 0}, {-1, 2}, {2, -1}});
    auto selection = tree.select(criterion);
    EXPECT_TRUE(selection.empty());
    EXPECT_EQ(selection.size(), 0u);
}

TEST_F(TreeTest, DifferentCriteriaGiveDifferentSelections) {
    Tree tree;
    FirstQuadrantCriterion first;
    TestAllCriterion all;
    insertPoints(tree, {{2, 2}, {-1, 1}, {0, 0}, {1, 1}});
    expectSelection(tree.select(first), {{1, 1}, {2, 2}});
    expectSelection(tree.select(all), {{-1, 1}, {0, 0}, {1, 1}, {2, 2}});
}

TEST_F(TreeTest, SelectDoesNotModifyTree) {
    Tree tree;
    FirstQuadrantCriterion criterion;
    insertPoints(tree, {{2, 2}, {-1, 1}, {3, 1}, {0, 0}});
    const Tree& constTree = tree;
    const auto size = constTree.size();
    const auto height = constTree.height();
    auto before = constTree.inorder();
    expectSelection(constTree.select(criterion), {{2, 2}, {3, 1}});
    auto after = constTree.inorder();
    EXPECT_EQ(constTree.size(), size);
    EXPECT_EQ(constTree.height(), height);
    ASSERT_EQ(before.size(), after.size());
    for (std::size_t i = 0; i < before.size(); ++i) {
        ASSERT_FALSE(before[i].expired());
        EXPECT_EQ(before[i].lock(), after[i].lock());
    }
}

TEST_F(TreeTest, SelectionCopiesAreIndependent) {
    Tree tree;
    insertPoints(tree, {{1, 1}, {2, 2}, {3, 3}});
    auto original = tree.inorder();
    Selection copy(original);
    Selection assigned;
    assigned = original;
    expectSelection(copy, {{1, 1}, {2, 2}, {3, 3}});
    expectSelection(assigned, {{1, 1}, {2, 2}, {3, 3}});
    EXPECT_EQ(copy[0].lock(), original[0].lock());
    EXPECT_EQ(assigned[0].lock(), original[0].lock());
    copy.add(tree.find(Point(3, 3)));
    ASSERT_EQ(copy.size(), 4u);
    ASSERT_EQ(original.size(), 3u);
    ASSERT_EQ(assigned.size(), 3u);
    ASSERT_TRUE(tree.remove(Point(2, 2)));
    EXPECT_TRUE(original[1].expired());
    EXPECT_TRUE(copy[1].expired());
    EXPECT_TRUE(assigned[1].expired());
    EXPECT_EQ(original.size(), 3u);
    EXPECT_EQ(copy.size(), 4u);
    EXPECT_EQ(assigned.size(), 3u);
    EXPECT_FALSE(copy[3].expired());
}

TEST_F(TreeTest, SelectionMoveConstructorEmptiesSource) {
    Tree tree;
    insertPoints(tree, {{1, 1}, {2, 2}});
    auto source = tree.inorder();
    Selection moved(std::move(source));
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.capacity(), 0u);
    expectSelection(moved, {{1, 1}, {2, 2}});
    source.add(tree.find(Point(1, 1)));
    expectSelection(source, {{1, 1}});
}

TEST_F(TreeTest, SelectionMoveAssignmentEmptiesSource) {
    Tree tree;
    insertPoints(tree, {{1, 1}, {2, 2}});
    auto source = tree.inorder();
    Selection target;
    target.add(tree.find(Point(2, 2)));
    target = std::move(source);
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.capacity(), 0u);
    expectSelection(target, {{1, 1}, {2, 2}});
}

TEST_F(TreeTest, SelectionKeepsSizeWhenRecordExpires) {
    Tree tree;
    FirstQuadrantCriterion criterion;
    insertPoints(tree, {{1, 1}, {2, 2}});
    auto selection = tree.select(criterion);
    ASSERT_EQ(selection.size(), 2u);
    ASSERT_FALSE(selection[0].expired());
    ASSERT_TRUE(tree.remove(Point(1, 1)));
    EXPECT_EQ(selection.size(), 2u);
    EXPECT_TRUE(selection[0].expired());
    EXPECT_FALSE(selection[0].lock());
    EXPECT_FALSE(selection[1].expired());
}

TEST_F(TreeTest, AliveCounterTracksDeletionAndDestruction) {
    Selection selection;
    {
        Tree tree;
        insertPoints(tree, {{2, 2}, {1, 1}, {3, 3}});
        EXPECT_EQ(Point::aliveCount(), 3);
        selection = tree.inorder();
        ASSERT_TRUE(tree.remove(Point(2, 2)));
        EXPECT_EQ(Point::aliveCount(), 2);
    }
    EXPECT_EQ(Point::aliveCount(), 0);
    ASSERT_EQ(selection.size(), 3u);
    for (std::size_t i = 0; i < selection.size(); ++i) {
        EXPECT_TRUE(selection[i].expired());
    }
}

TEST_F(TreeTest, TreeMoveConstructorEmptiesSource) {
    static_assert(!std::is_copy_constructible_v<Tree>);
    static_assert(!std::is_copy_assignable_v<Tree>);
    Tree source;
    insertPoints(source, {{2, 2}, {1, 1}});
    auto weak = source.find(Point(1, 1));
    Tree target(std::move(source));
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.height(), 0u);
    EXPECT_TRUE(source.inorder().empty());
    EXPECT_EQ(target.size(), 2u);
    EXPECT_FALSE(weak.expired());
    EXPECT_EQ(weak.lock(), target.find(Point(1, 1)).lock());
    expectSelection(target.inorder(), {{1, 1}, {2, 2}});
}

TEST_F(TreeTest, TreeMoveAssignmentReleasesOldRecords) {
    Tree source;
    Tree target;
    insertPoints(source, {{2, 2}, {3, 3}});
    insertPoints(target, {{9, 9}});
    auto old = target.find(Point(9, 9));
    auto moved = source.find(Point(2, 2));
    target = std::move(source);
    EXPECT_TRUE(old.expired());
    EXPECT_FALSE(moved.expired());
    EXPECT_EQ(source.size(), 0u);
    EXPECT_EQ(source.height(), 0u);
    EXPECT_EQ(target.size(), 2u);
    EXPECT_EQ(Point::aliveCount(), 2);
    expectSelection(target.inorder(), {{2, 2}, {3, 3}});
}

TEST_F(TreeTest, SelectionGrowsWithoutLosingElements) {
    Tree tree;
    Selection selection;
    for (int i = 0; i < 40; ++i) {
        ASSERT_TRUE(tree.insert(Point(i, i + 1)));
        selection.add(tree.find(Point(i, i + 1)));
        EXPECT_EQ(selection.size(), static_cast<std::size_t>(i + 1));
        EXPECT_GE(selection.capacity(), selection.size());
    }
    for (int i = 0; i < 40; ++i) {
        auto point = selection[static_cast<std::size_t>(i)].lock();
        ASSERT_TRUE(point);
        EXPECT_DOUBLE_EQ(point->x(), i);
        EXPECT_DOUBLE_EQ(point->y(), i + 1);
    }
}

}
