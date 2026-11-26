#pragma once

class Point {
    double x_;
    double y_;

    static int alive_count_;

public:
    Point(double x, double y);
    Point(const Point& other);

    ~Point();

    double x() const;
    double y() const;

    bool operator<(const Point& other) const;
    bool operator==(const Point& other) const;
    static int aliveCount() noexcept;
};