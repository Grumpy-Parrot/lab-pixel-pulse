#pragma once

#ifndef GP_VECTOR2_H
#define GP_VECTOR2_H

template<typename T = float>
struct Vector2
{
    T x;
    T y;

    Vector2(T x = T(0), T y = T(0)) : x(x), y(y) {}

    Vector2<T> operator+(const Vector2<T> &other) const
    {
        return Vector2<T>(x + other.x, y + other.y);
    }

    Vector2<T> operator-(const Vector2<T> &other) const
    {
        return Vector2<T>(x - other.x, y - other.y);
    }

    Vector2<T> operator*(T scalar) const
    {
        return Vector2<T>(x * scalar, y * scalar);
    }

    Vector2<T> operator/(T scalar) const
    {
        return Vector2<T>(x / scalar, y / scalar);
    }

    // For convenience, add some common type definitions
    using Float = Vector2<float>;
    using Int = Vector2<int>;
    using Double = Vector2<double>;
};

#endif
