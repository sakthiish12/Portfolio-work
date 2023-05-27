// /******************************************************************************/
// /*!
// \file         Matrix4.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        101 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

#include "Matrix.h"
#include "../Vector/Vector4.h"

namespace Mathf
{
    typedef Matrix<4, 4, float> Matrix4;

    template<typename T>
    struct Matrix<4,4,T>
    {
        typedef Vector<4, T> col_type;
        typedef int length_type;
    private:
        col_type m_value[4];
        const int m_length{4};
    public:
        // accessors
        col_type& operator[](length_type i);

        col_type const& operator[](length_type i) const;

        // constructors
        constexpr Matrix() = default;

        constexpr Matrix(Matrix const& _m);

        explicit constexpr Matrix(T const& _scalar);

        constexpr Matrix(
            T const& _x0, T const& _y0, T const& _z0, T const& _w0,
            T const& _x1, T const& _y1, T const& _z1, T const& _w1,
            T const& _x2, T const& _y2, T const& _z2, T const& _w2,
            T const& _x3, T const& _y3, T const& _z3, T const& _w3);

        constexpr Matrix(
            col_type const& _v0,
            col_type const& _v1,
            col_type const& _v2,
            col_type const& _v3);

        template<
            typename X1, typename Y1, typename Z1, typename W1,
            typename X2, typename Y2, typename Z2, typename W2,
            typename X3, typename Y3, typename Z3, typename W3,
            typename X4, typename Y4, typename Z4, typename W4>
        constexpr Matrix(
                X1 const& _x1, Y1 const& _y1, Z1 const& _z1, W1 const& _w1,
                X2 const& _x2, Y2 const& _y2, Z2 const& _z2, W2 const& _w2,
                X3 const& _x3, Y3 const& _y3, Z3 const& _z3, W3 const& _w3,
                X4 const& _x4, Y4 const& _y4, Z4 const& _z4, W4 const& _w4);

        template<typename V1, typename V2, typename V3, typename V4>
        constexpr Matrix(
            Vector<4, V1> const& _v1,
            Vector<4, V2> const& _v2,
            Vector<4, V3> const& _v3,
            Vector<4, V4> const& _v4);

        template<typename U>
        constexpr Matrix(Matrix<4, 4, U> const& _m);
        // 3x3??

        // unary arithmetic operators
        template<typename U>
        Matrix& operator=(Matrix<4, 4, U> const& _m);

        template<typename U>
        Matrix& operator+=(U _scalar);

        template<typename U>
        Matrix& operator+=(Matrix<4, 4, U> const& _m);

        template<typename U>
        Matrix& operator-=(U _scalar);

        template<typename U>
        Matrix& operator-=(Matrix<4, 4, U> const& _m);

        template<typename U>
        Matrix& operator*=(U _scalar);

        template<typename U>
        Matrix& operator*=(Matrix<4, 4, U> const& _m);

        template<typename U>
        Matrix& operator/=(U _scalar);

        template<typename U>
        Matrix& operator/=(Matrix<4, 4, U> const& _m);

    };

    // unary operators
    template<typename T>
    Matrix<4, 4, T> operator+(Matrix<4, 4, T> const& _m);

    template<typename T>
    Matrix<4, 4, T> operator-(Matrix<4, 4, T> const& _m);

    // binary operators
    template<typename T>
    Matrix<4, 4, T>operator+ (const Matrix<4, 4, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<4, 4, T>operator+ (const T& _scalar, const Matrix<4, 4, T>& _m);

    template<typename T>
    Matrix<4, 4, T>operator+ (const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2);

    template<typename T>
    Matrix<4, 4, T>operator- (const Matrix<4, 4, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<4, 4, T>operator- (const T& _scalar, const Matrix<4, 4, T>& _m);

    template<typename T>
    Matrix<4, 4, T>operator- (const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2);

    template<typename T>
    Matrix<4, 4, T>operator* (const Matrix<4, 4, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<4, 4, T>operator* (const T& _scalar, const Matrix<4, 4, T>& _m);

    template<typename T>
    Matrix<4, 4, T>operator* (const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2);

    template<typename T>
    Matrix<4, 4, T>operator/ (const Matrix<4, 4, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<4, 4, T>operator/ (const T& _scalar, const Matrix<4, 4, T>& _m);

    template<typename T>
    Matrix<4, 4, T>operator/ (const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2);

    // boolean operators
    template<typename T>
    constexpr bool operator== (const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2);

    template<typename T>
    constexpr bool operator!= (const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2);
}

#include "Matrix4.tpp"