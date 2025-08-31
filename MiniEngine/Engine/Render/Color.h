#pragma once

#include "Engine/math/math.h"

struct Color {
public:
    Vector3 value;
    Color():value(Vector3(0, 0, 0)){}

    Color(const Vector3 val, const float alpha = 1.0f) : value(val) {}
    Color(const float _r, const float _g, const float _b, const float alpha = 1.0f)
        : value(Vector3(_r, _g, _b)) {}
    ~Color(){}

    static const Color BLACK;
    static const Color WHITE;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
    static const Color CYAN;
    static const Color GRAY;
    static const Color PURPLE;
    static const Color ORANGE;
};

