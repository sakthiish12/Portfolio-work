// /******************************************************************************/
// /*!
// \file         Matrix4.tpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        282 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "Matrix4.h"
//#include "Assert.h"

namespace Mathf
{
    template <typename T>
    typename Matrix<4, 4, T>::col_type& Matrix<4, 4, T>::operator[](length_type i)
    {
        //Assert(i < this->m_length, "accessing index for Matrix4 has to be less than 4");
        return this->m_value[i];
    }

    template <typename T>
    typename Matrix<4, 4, T>::col_type const& Matrix<4, 4, T>::operator[](length_type i) const
    {
        //Assert(i < this->m_length, "accessing index for Matrix4 has to be less than 4");
        return this->m_value[i];
    }

    template <typename T>
    constexpr Matrix<4, 4, T>::Matrix(Matrix const& _m) :
        m_value{col_type{_m[0]},col_type{_m[1]},col_type{_m[2]},col_type{_m[3]} }
    {
    }

    template <typename T>
    constexpr Matrix<4, 4, T>::Matrix(T const& _scalar) :
        m_value{ col_type{_scalar,0,0,0},col_type{0,_scalar,0,0},col_type{0,0,_scalar,0},col_type{0,0,0,_scalar} }
    {
    }

    template <typename T>
    constexpr Matrix<4, 4, T>::Matrix(
        T const& _x0, T const& _y0, T const& _z0, T const& _w0,
        T const& _x1, T const& _y1, T const& _z1, T const& _w1,
        T const& _x2, T const& _y2, T const& _z2, T const& _w2,
        T const& _x3, T const& _y3, T const& _z3, T const& _w3) :
        m_value{
            col_type {_x0,_y0,_z0, _w0},
            col_type {_x1,_y1,_z1, _w1},
            col_type {_x2,_y2,_z2, _w2},
            col_type {_x3,_y3,_z3, _w3}}
    {
    }

    template <typename T>
    constexpr Matrix<4, 4, T>::Matrix(col_type const& _v0, col_type const& _v1, col_type const& _v2, col_type const& _v3) :
        m_value{ _v0 ,_v1 ,_v2 ,_v3 }
    {
    }

    template <typename T>
    template <
        typename X1, typename Y1, typename Z1, typename W1,
        typename X2, typename Y2, typename Z2, typename W2,
        typename X3, typename Y3, typename Z3, typename W3,
        typename X4, typename Y4, typename Z4, typename W4>
    constexpr Matrix<4, 4, T>::Matrix(
        X1 const& _x1, Y1 const& _y1, Z1 const& _z1, W1 const& _w1,
        X2 const& _x2, Y2 const& _y2, Z2 const& _z2, W2 const& _w2,
        X3 const& _x3, Y3 const& _y3, Z3 const& _z3, W3 const& _w3,
        X4 const& _x4, Y4 const& _y4, Z4 const& _z4, W4 const& _w4) :
        m_value{
            col_type {_x1,_y1,_z1, _w1},
            col_type {_x2,_y2,_z2, _w2},
            col_type {_x3,_y3,_z3, _w3},
            col_type {_x4,_y4,_z4, _w4} }
    {
    }

    template <typename T>
    template <typename V1, typename V2, typename V3, typename V4>
    constexpr Matrix<4, 4, T>::Matrix(Vector<4, V1> const& _v1, Vector<4, V2> const& _v2, Vector<4, V3> const& _v3, Vector<4, V4> const& _v4) :
        m_value{col_type{_v1}, col_type{_v2}, col_type{_v3}, col_type{_v4}}
    {
    }

    template <typename T>
    template <typename U>
    constexpr Matrix<4, 4, T>::Matrix(Matrix<4, 4, U> const& _m) :
     m_value{col_type{_m[0]}, col_type{_m[1]}, col_type{_m[2]}, col_type{_m[3]}}
    {
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator=(Matrix<4, 4, U> const& _m)
    {
        this->m_value[0] = _m[0];
        this->m_value[1] = _m[1];
        this->m_value[2] = _m[2];
        this->m_value[3] = _m[3];
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator+=(U _scalar)
    {
        this->m_value[0] += _scalar;
        this->m_value[1] += _scalar;
        this->m_value[2] += _scalar;
        this->m_value[3] += _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator+=(Matrix<4, 4, U> const& _m)
    {
        this->m_value[0] += _m[0];
        this->m_value[1] += _m[1];
        this->m_value[2] += _m[2];
        this->m_value[3] += _m[3];
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator-=(U _scalar)
    {
        this->m_value[0] -= _scalar;
        this->m_value[1] -= _scalar;
        this->m_value[2] -= _scalar;
        this->m_value[3] -= _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator-=(Matrix<4, 4, U> const& _m)
    {
        this->m_value[0] -= _m[0];
        this->m_value[1] -= _m[1];
        this->m_value[2] -= _m[2];
        this->m_value[3] -= _m[3];
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator*=(U _scalar)
    {
        this->m_value[0] *= _scalar;
        this->m_value[1] *= _scalar;
        this->m_value[2] *= _scalar;
        this->m_value[3] *= _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator*=(Matrix<4, 4, U> const& _m)
    {
        return (*this = *this * _m);
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator/=(U _scalar)
    {
        this->m_value[0] /= _scalar;
        this->m_value[1] /= _scalar;
        this->m_value[2] /= _scalar;
        this->m_value[3] /= _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<4, 4, T>& Matrix<4, 4, T>::operator/=(Matrix<4, 4, U> const& _m)
    {
        return *this *= inverse(_m);
    }

    template <typename T>
    Matrix<4, 4, T> operator+(Matrix<4, 4, T> const& _m)
    {
        return _m;
    }

    template <typename T>
    Matrix<4, 4, T> operator-(Matrix<4, 4, T> const& _m)
    {
        return Matrix<4, 4, T>{-_m[0], -_m[1], -_m[2], -_m[3]};
    }

    template <typename T>
    Matrix<4, 4, T> operator+(const Matrix<4, 4, T>& _m, const T& _scalar)
    {
        return Matrix<4, 4, T>{
            _m[0] + _scalar,
            _m[1] + _scalar,
            _m[2] + _scalar,
            _m[3] + _scalar};
    }

    template <typename T>
    Matrix<4, 4, T> operator+(const T& _scalar, const Matrix<4, 4, T>& _m)
    {
        return Matrix<4, 4, T>{
            _m[0] + _scalar,
            _m[1] + _scalar,
            _m[2] + _scalar,
            _m[3] + _scalar};
    }

    template <typename T>
    Matrix<4, 4, T> operator+(const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2)
    {
        return Matrix<4, 4, T>{
            _m1[0] + _m2[0],
            _m1[1] + _m2[1],
            _m1[2] + _m2[2],
            _m1[3] + _m2[3]};
    }

    template <typename T>
    Matrix<4, 4, T> operator-(const Matrix<4, 4, T>& _m, const T& _scalar)
    {
        return Matrix<4, 4, T>{
            _m[0] - _scalar,
            _m[1] - _scalar,
            _m[2] - _scalar,
            _m[3] - _scalar};
    }

    template <typename T>
    Matrix<4, 4, T> operator-(const T& _scalar, const Matrix<4, 4, T>& _m)
    {
        return Matrix<4, 4, T>{
            _scalar - _m[0],
            _scalar - _m[1],
            _scalar - _m[2],
            _scalar - _m[3]};
    }

    template <typename T>
    Matrix<4, 4, T> operator-(const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2)
    {
        return Matrix<4, 4, T>{
            _m1[0] - _m2[0],
            _m1[1] - _m2[1],
            _m1[2] - _m2[2],
            _m1[3] - _m2[3]};
    }

    template <typename T>
    Matrix<4, 4, T> operator*(const Matrix<4, 4, T>& _m, const T& _scalar)
    {
        return Matrix<4, 4, T>{
            _m[0] * _scalar,
            _m[1] * _scalar,
            _m[2] * _scalar,
            _m[3] * _scalar};
    }

    template <typename T>
    Matrix<4, 4, T> operator*(const T& _scalar, const Matrix<4, 4, T>& _m)
    {
        return Matrix<4, 4, T>{
            _m[0] * _scalar,
            _m[1] * _scalar,
            _m[2] * _scalar,
            _m[3] * _scalar};
    }

    template <typename T>
    Matrix<4, 4, T> operator*(const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2)
    {
        return Matrix<4, 4, T>{
            _m1[0] * _m2[0],
            _m1[1] * _m2[1],
            _m1[2] * _m2[2],
            _m1[3] * _m2[3]};
    }

    template <typename T>
    Matrix<4, 4, T> operator/(const Matrix<4, 4, T>& _m, const T& _scalar)
    {
        return Matrix<4, 4, T>{
            _m[0] / _scalar,
            _m[1] / _scalar,
            _m[2] / _scalar,
            _m[3] / _scalar};
    }

    template <typename T>
    Matrix<4, 4, T> operator/(const T& _scalar, const Matrix<4, 4, T>& _m)
    {
        return Matrix<4, 4, T>{
            _scalar / _m[0],
            _scalar / _m[1],
            _scalar / _m[2],
            _scalar / _m[3]};
    }

    template <typename T>
    Matrix<4, 4, T> operator/(const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2)
    {
        return Matrix<4, 4, T>{
            _m1[0] / _m2[0],
            _m1[1] / _m2[1],
            _m1[2] / _m2[2],
            _m1[3] / _m2[3]};
    }

    template <typename T>
    constexpr bool operator==(const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2)
    {
        return (_m1[0] == _m2[0]) && (_m1[1] == _m2[1]) && (_m1[2] == _m2[2]) && (_m1[3] == _m2[3]);
    }

    template <typename T>
    constexpr bool operator!=(const Matrix<4, 4, T>& _m1, const Matrix<4, 4, T>& _m2)
    {
        return (_m1[0] != _m2[0]) || (_m1[1] != _m2[1]) || (_m1[2] != _m2[2]) || (_m1[3] != _m2[3]);
    }
}
