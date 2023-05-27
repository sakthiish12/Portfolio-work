// /******************************************************************************/
// /*!
// \file         Vector3.h
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

#include "Vector.h"

namespace Mathf
{
    // typedef
    typedef Vector<3, float> Vector3;

    template<typename T>
    struct Vector<3, T>
    {
        T x{ 0 }, y{ 0 }, z{ 0 };

        // constructors
        constexpr Vector() = default;

        constexpr Vector(const Vector& _v) = default;

        constexpr Vector(T _x, T _y, T _z);

        constexpr explicit Vector(T _scalar);

        template<typename X, typename Y, typename Z>
        constexpr Vector(X _x, Y _y, Z _z);

        template<typename U, typename V>
        constexpr Vector(const Vector<2, U>& _v, V _scalar);

        template<typename U, typename V>
        constexpr Vector(U _scalar, const Vector<2, V>& _v);

        template<typename U>
        constexpr Vector(const Vector<3, U>& _v);

        // unary arithmetic operators
        constexpr Vector& operator=(const Vector& v) = default;

        template<typename U>
        constexpr Vector& operator=(const Vector<3, U>& _v);

        template<typename U>
        constexpr Vector& operator+=(const Vector<3, U>& _v);

        template<typename U>
        constexpr Vector& operator+=(U _scalar);

        template<typename U>
        constexpr Vector& operator-=(const Vector<3, U>& _v);

        template<typename U>
        constexpr Vector& operator-=(U _scalar);

        template<typename U>
        constexpr Vector& operator*=(const Vector<3, U>& _v);

        template<typename U>
        constexpr Vector& operator*=(U _scalar);

        template<typename U>
        constexpr Vector& operator/=(const Vector<3, U>& _v);

        template<typename U>
        constexpr Vector& operator/=(U _scalar);

        Vector<2, T> getVec2();

    };

    // unary operators
    template<typename T>
    constexpr Vector<3, T> operator+(const Vector<3, T>& _v);

    template<typename T>
    constexpr Vector<3, T> operator-(const Vector<3, T>& _v);

    // binary operators
    template<typename T>
    constexpr Vector<3, T> operator+(const Vector<3, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<3, T> operator+(T _scalar, const Vector<3, T>& _v);

    template<typename T>
    constexpr Vector<3, T> operator+(const  Vector<3, T>& _v1, const Vector<3, T>& _v2);

    template<typename T>
    constexpr Vector<3, T> operator-(const  Vector<3, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<3, T> operator-(T _scalar, const Vector<3, T>& _v);

    template<typename T>
    constexpr Vector<3, T> operator-(const Vector<3, T>& _v1, const Vector<3, T>& _v2);

    template<typename T>
    constexpr Vector<3, T> operator*(const Vector<3, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<3, T> operator*(T _scalar, const Vector<3, T>& v);

    template<typename T>
    constexpr Vector<3, T> operator*(const Vector<3, T>& _v1, const Vector<3, T>& _v2);

    template<typename T>
    constexpr Vector<3, T> operator/(const Vector<3, T>& _v, T _scalar);

    template<typename T>
    constexpr Vector<3, T> operator/(T _scalar, const Vector<3, T>& _v);

    template<typename T>
    constexpr  Vector<3, T> operator/(const Vector<3, T>& _v1, const Vector<3, T>& _v2);

    // boolean operators
    template<typename T>
    constexpr bool operator== (const Vector<3, T>& _v1, const Vector<3, T>& _v2);

    template<typename T>
    constexpr bool operator!= (const Vector<3, T>& _v1, const Vector<3, T>& _v2);

	using vec3 = Vector<3, float>;
	using Vec3 = Vector<3, float>;
};

#include "Vector3.tpp"