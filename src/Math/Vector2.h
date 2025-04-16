#pragma once

#ifndef PIXELPULSE_VECTOR2_H
#define PIXELPULSE_VECTOR2_H

#include "../Platform/Std.h"

namespace PixelPulse::Math
{
    template <typename T = float>
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

        Vector2<T> operator*(const Vector2<T> &other) const
        {
            return Vector2<T>(x * other.x, y * other.y);
        }

        Vector2<T> operator/(T scalar) const
        {
            return Vector2<T>(x / scalar, y / scalar);
        }

        Vector2<T> &operator+=(const Vector2<T> &other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        // For convenience, add some common type definitions
        using Float = Vector2<float>;
        using Double = Vector2<double>;
        using Int16 = Vector2<std::int16_t>;
        using Int32 = Vector2<std::int32_t>;
        using Int64 = Vector2<std::int64_t>;
        using UInt16 = Vector2<std::uint16_t>;
        using UInt32 = Vector2<std::uint32_t>;
        using UInt64 = Vector2<std::uint64_t>;
    };
}

#endif
