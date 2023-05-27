// /******************************************************************************/
// /*!
// \file         Matrix3.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        96 Lines
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
#include "../Vector/Vector3.h"

namespace Mathf
{
    typedef Matrix<3, 3, float> Matrix3;

    template<typename T>
    struct Matrix<3, 3, T>
    {
        typedef Vector<3, T> col_type;
        typedef int length_type;
    private:
        col_type m_value[3];
        const int m_length{ 3 };

    public:
        // accessors
        col_type& operator[](length_type i);

        col_type const& operator[](length_type i) const;

        // constructors
        constexpr Matrix() = default;

        constexpr Matrix(Matrix const& _m);

        explicit constexpr Matrix(T const& _scalar);

        //_RowByRow true to be like glm order of parameter
        constexpr Matrix(
            T const& _11, T const& _12, T const& _13,
            T const& _21, T const& _22, T const& _23,
            T const& _31, T const& _32, T const& _33,
            bool _RowByRow = true
        );

        //each vector is a colum vector
        constexpr Matrix(
            col_type const& _v0,
            col_type const& _v1,
            col_type const& _v2);

        //Transpose and update its own data
        void TransposeSelf();

        //Get the transpose matrix without modifying data
        Matrix Transpose() const;

       /* template<
            typename X1, typename Y1, typename Z1,
            typename X2, typename Y2, typename Z2,
            typename X3, typename Y3, typename Z3>
            constexpr Matrix(
                X1 const& _x1, Y1 const& _y1, Z1 const& _z1,
                X2 const& _x2, Y2 const& _y2, Z2 const& _z2,
                X3 const& _x3, Y3 const& _y3, Z3 const& _z3);*/

        template<typename V1, typename V2, typename V3>
        constexpr Matrix(
            Vector<3, V1> const& _v1,
            Vector<3, V2> const& _v2,
            Vector<3, V3> const& _v3);

        template<typename U>
        constexpr Matrix(Matrix<3, 3, U> const& _m);

        // unary arithmetic operators
        template<typename U>
        Matrix& operator=(Matrix<3, 3, U> const& _m);

        template<typename U>
        Matrix& operator+=(U _scalar);

        template<typename U>
        Matrix& operator+=(Matrix<3, 3, U> const& _m);

        template<typename U>
        Matrix& operator-=(U _scalar);

        template<typename U>
        Matrix& operator-=(Matrix<3, 3, U> const& _m);

        template<typename U>
        Matrix& operator*=(U _scalar);

        template<typename U>
        Matrix& operator*=(Matrix<3, 3, U> const& _m);

        template<typename U>
        Matrix& operator/=(U _scalar);

        template<typename U>
        Matrix& operator/=(Matrix<3, 3, U> const& _m);
    };

    // unary operators
    template<typename T>
    Matrix<3, 3, T> operator+(Matrix<3, 3, T> const& _m);

    template<typename T>
    Matrix<3, 3, T> operator-(Matrix<3, 3, T> const& _m);

    // binary operators
    template<typename T>
    Matrix<3, 3, T>operator+ (const Matrix<3, 3, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<3, 3, T>operator+ (const T& _scalar, const Matrix<3, 3, T>& _m);

    template<typename T>
    Matrix<3, 3, T>operator+ (const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2);

    template<typename T>
    Matrix<3, 3, T>operator- (const Matrix<3, 3, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<3, 3, T>operator- (const T& _scalar, const Matrix<3, 3, T>& _m);

    template<typename T>
    Matrix<3, 3, T>operator- (const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2);

    template<typename T>
    Matrix<3, 3, T>operator* (const Matrix<3, 3, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<3, 3, T>operator* (const T& _scalar, const Matrix<3, 3, T>& _m);

    template<typename T>
    Matrix<3, 3, T>operator* (const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2);

    template<typename T>
    Vector<3, T> operator* (const Matrix<3, 3, T>& _m, const Vector<3, T>& _v);

    template<typename T>
    Matrix<3, 3, T>operator/ (const Matrix<3, 3, T>& _m, const T& _scalar);

    template<typename T>
    Matrix<3, 3, T>operator/ (const T& _scalar, const Matrix<3, 3, T>& _m);

    template<typename T>
    Matrix<3, 3, T>operator/ (const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2);

    // boolean operators
    template<typename T>
    constexpr bool operator== (const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2);

    template<typename T>
    constexpr bool operator!= (const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2);
}

#include "Matrix3.tpp"