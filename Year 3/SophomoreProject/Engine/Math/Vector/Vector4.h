// /******************************************************************************/
// /*!
// \file         Vector4.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        83 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include "Vector.h"

namespace Mathf
{
    typedef Vector<4, float> Vector4;

    template<typename T>
    struct Vector<4, T>
    {
        T x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 };

        // constructors
        constexpr Vector() = default;

        constexpr Vector(const Vector & _v) = default;

        constexpr Vector(T _x, T _y, T _z, T _w);

        constexpr explicit Vector(T _scalar);

        template<typename X, typename Y, typename Z, typename W>
        constexpr Vector(X _x, Y _y, Z _z, W _w);

        template<typename U, typename Z, typename W>
        constexpr Vector(const Vector<2, U>& _v, Z _z, W _w);

        template<typename X, typename U, typename W>
        constexpr Vector(X _x, const Vector<2, U>& _v, W _w);

        template<typename X, typename Y, typename U>
        constexpr Vector(X _x, Y _y, const Vector<2, U>& _v);

        template<typename U, typename W>
        constexpr Vector(const Vector<3, U>& _v, W _w);

        template<typename U, typename X>
        constexpr Vector(X _x, const Vector<3, U>& _v);

        template<typename U, typename V>
        constexpr Vector(const Vector<2, U>& _v1, const Vector<2, V>& _v2);

        template<typename U>
        constexpr explicit Vector(const Vector<4, U>& _v);

        // unary arithmetic operators
        constexpr Vector& operator=(const Vector& _v) = default;

        template<typename U>
        constexpr Vector& operator=(const Vector<4, U>& _v);

        template<typename U>
        constexpr Vector& operator+=(const Vector<4, U>& _v);

        template<typename U>
        constexpr Vector& operator+=(U _scalar);

        template<typename U>
        constexpr Vector& operator-=(const Vector<4, U>& _v);

        template<typename U>
        constexpr Vector& operator-=(U _scalar);

        template<typename U>
        constexpr Vector& operator*=(const Vector<4, U>& _v);

        template<typename U>
        constexpr Vector& operator*=(U _scalar);

        template<typename U>
        constexpr Vector& operator/=(const Vector<4, U>& _v);

        template<typename U>
        constexpr Vector& operator/=(U _scalar);
    };

    // unary operator
    template<typename T>
    constexpr Vector<4, T> operator+(const Vector<4, T>& _v);

    template<typename T>
    constexpr Vector<4, T> operator-(const Vector<4, T>& _v);

    // binary operators
    template<typename T>
    constexpr Vector<4, T> operator+(const Vector<4, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<4, T> operator+(T _scalar, const Vector<4, T>& _v);

    template<typename T>
    constexpr Vector<4, T> operator+(const  Vector<4, T>& _v1, const Vector<4, T>& _v2);

    template<typename T>
    constexpr Vector<4, T> operator-(const  Vector<4, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<4, T> operator-(T _scalar, const Vector<4, T>& _v);

    template<typename T>
    constexpr Vector<4, T> operator-(const Vector<4, T>& _v1, const Vector<4, T>& _v2);

    template<typename T>
    constexpr Vector<4, T> operator*(const Vector<4, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<4, T> operator*(T _scalar, const Vector<4, T>& _v);

    template<typename T>
    constexpr Vector<4, T> operator*(const Vector<4, T>& _v1, const Vector<4, T>& _v2);

    template<typename T>
    constexpr Vector<4, T> operator/(const Vector<4, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<4, T> operator/(T _scalar, const Vector<4, T>& _v);

    template<typename T>
    constexpr  Vector<4, T> operator/(const Vector<4, T>& _v1, const Vector<4, T>& _v2);

    // boolean operators
    template<typename T>
    constexpr bool operator== (const Vector<4, T>& _v1, const Vector<4, T>& _v2);

    template<typename T>
    constexpr bool operator!= (const Vector<4, T>& _v1, const Vector<4, T>& _v2);
}

#include "Vector4.tpp"