// /******************************************************************************/
// /*!
// \file         Vector2.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        77 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include <limits>
#include "Vector.h"

namespace Mathf
{
    // typedef
    typedef Vector<2, float> Vector2;
    typedef Vector<2, int> iVector2;

    template<typename T>
    struct Vector<2, T>
    {
        T x{ 0 }, y{ 0 };

        // constructors
        constexpr Vector() = default;

        constexpr Vector(const Vector& _v) = default;

        constexpr Vector(T _x, T _y);

        constexpr explicit Vector(T _scalar);

        template<typename X, typename Y>
        constexpr Vector(X _x, Y _y);

        template<typename U>
        constexpr Vector(const Vector<3, U>& _v);

        template<typename U>
        constexpr Vector(const Vector<2, U>& _v);

        // unary arithmetic operators
        constexpr Vector& operator=(const Vector& _v) = default;

        template<typename U>
        constexpr Vector& operator=(const Vector<2, U>& _v);

        template<typename U>
        constexpr Vector& operator+=(const Vector<2, U>& _v);

        template<typename U>
        constexpr Vector& operator+=(U _scalar);

        template<typename U>
        constexpr Vector& operator-=(const Vector<2, U>& _v);

        template<typename U>
        constexpr Vector& operator-=(U _scalar);

        template<typename U>
        constexpr Vector& operator*=(const Vector<2, U>& _v);

        template<typename U>
        constexpr Vector& operator*=(U _scalar);

        template<typename U>
        constexpr Vector& operator/=(const Vector<2, U>& _v);

        template<typename U>
        constexpr Vector& operator/=(U _scalar);

    };

    // unary operator
    template<typename T>
    constexpr Vector<2, T> operator+ (const Vector<2, T>& _v);

    template<typename T>
    constexpr Vector<2, T> operator- (const Vector<2, T>& _v);
        
    // binary operators
    template<typename T>
    constexpr Vector<2, T> operator+(const Vector<2, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<2, T> operator+(T _scalar, const Vector<2, T>& _v);

    template<typename T>
    constexpr Vector<2, T> operator+(const  Vector<2, T>& _v1, const Vector<2, T>& _v2);

    template<typename T>
    constexpr Vector<2, T> operator-(const  Vector<2, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<2, T> operator-(T _scalar, const Vector<2, T>& _v);

    template<typename T>
    constexpr Vector<2, T> operator-(const Vector<2, T>& _v1, const Vector<2, T>& _v2);

    template<typename T>
    constexpr Vector<2, T> operator*(const Vector<2, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<2, T> operator*(T _scalar, const Vector<2, T>& _v);

    template<typename T>
    constexpr Vector<2, T> operator*(const Vector<2, T>& _v1, const Vector<2, T>& _v2);

    template<typename T>
    constexpr Vector<2, T> operator/(const Vector<2, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<2, T> operator/(T _scalar, const Vector<2, T>& _v);

    template<typename T>
    constexpr  Vector<2, T> operator/(const Vector<2, T>& _v1, const Vector<2, T>& _v2);

    // boolean operators
    template<typename T>
    constexpr bool operator== (const Vector<2, T>& _v1, const Vector<2, T>& _v2);

    template<typename T>
    constexpr bool operator!= (const Vector<2, T>& _v1, const Vector<2, T>& _v2);

	using vec2 = Vector2;
	using Vec2 = Vector2;
};

#include "Vector2.tpp"