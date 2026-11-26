#include <iostream>
#include <limits>
#include <cmath>

#include "tree.hpp"
#include "point.hpp"
#include "selection.hpp"
#include "first_quadrant_criterion.hpp"

void printMenu() {
    std::cout << "\n========== BST MENU ==========\n"
              << "1. Create new tree\n"
              << "2. Insert point\n"
              << "3. Find point\n"
              << "4. Remove point\n"
              << "5. Inorder traversal\n"
              << "6. Reverse inorder traversal\n"
              << "7. Select points (first quadrant)\n"
              << "8. Tree information\n"
              << "0. Exit\n"
              << "==============================\n";
}

void printSelection(const Selection& selection) {
    if (selection.empty()) {
        std::cout << "Selection is empty\n";
        return;
    }

    for (std::size_t i = 0; i < selection.size(); i++) {
        if (auto point = selection[i].lock()) {
            std::cout << "(" << point->x() << ", " << point->y() << ") ";
        } else {
            std::cout << "[Expired] ";
        }
    }
    std::cout << '\n';
}

int main() {
    Tree tree;
    FirstQuadrantCriterion criterion;
    int command;

    printMenu();

    while (true) {
        std::cout << "\nEnter command: ";
        if (!(std::cin >> command)) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            std::cout << "Invalid input\n";
            continue;
        }
        if (command == 0) {
            break;
        }
        if (command >= 2 && command <= 4) {
            double x, y;

            std::cout << "Enter x and y: ";
            if (!(std::cin >> x >> y)) {
                if (std::cin.eof()) {
                    break;
                }
                std::cin.clear();
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
                );
                std::cout << "Invalid coordinates\n";
                continue;
            }
            if (!std::isfinite(x) || !std::isfinite(y)) {
                std::cout << "Invalid coordinates\n";
                continue;
            }
            Point point(x, y);

            switch (command) {
                case 2:
                    if (tree.insert(point)) {
                        std::cout << "Point inserted successfully\n";
                    } else {
                        std::cout << "Point already exists\n";
                    }
                    break;

                case 3:
                    if (auto found = tree.find(point).lock()) {
                        std::cout << "Point found: (" << found->x() << ", " << found->y() << ")\n";
                    } else {
                        std::cout << "Point not found\n";
                    }
                    break;

                case 4:
                    if (tree.remove(point)) {
                        std::cout << "Point removed successfully\n";
                    } else {
                        std::cout << "Point not found\n";
                    }
                    break;
            }

            continue;
        }
        switch (command) {
            case 1:
                tree = Tree{};
                std::cout << "New tree created\n";
                break;

            case 5:
                printSelection(tree.inorder());
                break;

            case 6:
                printSelection(tree.reverseInorder());
                break;

            case 7:
                printSelection(tree.select(criterion));
                break;

            case 8:
                std::cout << "Size: " << tree.size() << '\n'
                          << "Height: " << tree.height() << '\n'
                          << "Alive points: " << Point::aliveCount() << '\n';
                break;

            default:
                std::cout << "Unknown command\n";
                break;
        }
    }

    return 0;
}