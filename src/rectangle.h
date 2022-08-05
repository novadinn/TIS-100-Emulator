#pragma once

#include <algorithm>

struct Rectangle {
    Rectangle(float x, float y, float width, float height) :
        x_(x), y_(y), width_(width), height_(height) {}

    float center_x() const { return x_ + width_ / 2; }
    float center_y() const { return y_ + height_ / 2; }

    float left() const { return x_; }
    float right() const { return x_ + width_; }
    float top() const { return y_; }
    float bottom() const { return y_ + height_; }

    float width() const { return width_; }
    float height() const { return height_; }

    bool pointIntersection(int x, int y) const {
	return (left() <= x && right() >= x) && (top() <= y && bottom() >= y);
    };

    Rectangle& operator=(Rectangle other) {
        std::swap(x_, other.x_);
        std::swap(y_, other.y_);
        std::swap(width_, other.width_);
        std::swap(height_, other.height_);
        return *this;
    }

private:
    float x_, y_, width_, height_;
};
