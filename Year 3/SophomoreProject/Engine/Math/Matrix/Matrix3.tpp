// /******************************************************************************/
// /*!
// \file         Matrix3.tpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        258 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/

#include "Matrix3.h"
//#include "Assert.h"

namespace Mathf
{
    template <typename T>
    typename Matrix<3, 3, T>::col_type& Matrix<3, 3, T>::operator[](length_type i)
    {
        //Assert(i < this->m_length, "accessing index for Matrix3 has to be less than 3");
        return this->m_value[i];
    }

    template <typename T>
    typename Matrix<3, 3, T>::col_type const& Matrix<3, 3, T>::operator[](length_type i) const
    {
        //Assert(i < this->m_length, "accessing index for Matrix3 has to be less than 3");
        return this->m_value[i];
    }

    template <typename T>
    constexpr Matrix<3, 3, T>::Matrix(Matrix const& _m) :
        m_value{ col_type{_m[0]},col_type{_m[1]},col_type{_m[2]}}
    {
    }

    template <typename T>
    constexpr Matrix<3, 3, T>::Matrix(T const& _scalar) :
        m_value{ col_type{_scalar,0,0},col_type{0,_scalar,0},col_type{0,0,_scalar}}
    {
    }

    template <typename T>
    constexpr Matrix<3, 3, T>::Matrix(
        T const& _11, T const& _12, T const& _13,
        T const& _21, T const& _22, T const& _23,
        T const& _31, T const& _32, T const& _33,
        bool _RowByRow) :
        m_value{
            _RowByRow?col_type {_11,_21,_31}:col_type {_11,_12,_13},
            _RowByRow?col_type {_12,_22,_32}:col_type {_21,_22,_23},
            _RowByRow?col_type {_13,_23,_33}:col_type {_31,_32,_33} 
            }
    {
    }

    template <typename T>
    constexpr Matrix<3, 3, T>::Matrix(col_type const& _v0, col_type const& _v1, col_type const& _v2) :
        m_value{ _v0 ,_v1 ,_v2}
    {
    }

    //Transpose and update its own data
    template <typename T>
    void Matrix<3, 3, T>::TransposeSelf()
    {
        T temp; 

        temp = m_value[1].x;
        m_value[1].x = m_value[0].y;
        m_value[0].y = temp;

        temp = m_value[2].x;
        m_value[2].x = m_value[0].z;
        m_value[0].z = temp;

        temp = m_value[1].z;
        m_value[1].z = m_value[2].y;
        m_value[2].y = temp;
    }

    //Get the transpose matrix without modifying data
    template <typename T>
    Matrix<3, 3, T> Matrix<3, 3, T>::Transpose() const
    {
        Matrix<3, 3, T> newMat = *this;
        newMat.TransposeSelf();
        return newMat;
    }

    /*template <typename T>
    template <
        typename X1, typename Y1, typename Z1,
        typename X2, typename Y2, typename Z2,
        typename X3, typename Y3, typename Z3>
    constexpr Matrix<3, 3, T>::Matrix(
        X1 const& _x1, Y1 const& _y1, Z1 const& _z1,
        X2 const& _x2, Y2 const& _y2, Z2 const& _z2,
        X3 const& _x3, Y3 const& _y3, Z3 const& _z3) :
        m_value{
            col_type {_x1,_y1,_z1},
            col_type {_x2,_y2,_z2},
            col_type {_x3,_y3,_z3}}
    {
    }*/

    template <typename T>
    template <typename V1, typename V2, typename V3>
    constexpr Matrix<3, 3, T>::Matrix(Vector<3, V1> const& _v1, Vector<3, V2> const& _v2, Vector<3, V3> const& _v3) :
        m_value{ col_type{_v1}, col_type{_v2}, col_type{_v3}}
    {
    }

    template <typename T>
    template <typename U>
    constexpr Matrix<3, 3, T>::Matrix(Matrix<3, 3, U> const& _m) :
        m_value{ col_type{_m[0]}, col_type{_m[1]}, col_type{_m[2]}}
    {
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator=(Matrix<3, 3, U> const& _m)
    {
        this->m_value[0] = _m[0];
        this->m_value[1] = _m[1];
        this->m_value[2] = _m[2];
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator+=(U _scalar)
    {
        this->m_value[0] += _scalar;
        this->m_value[1] += _scalar;
        this->m_value[2] += _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator+=(Matrix<3, 3, U> const& _m)
    {
        this->m_value[0] += _m[0];
        this->m_value[1] += _m[1];
        this->m_value[2] += _m[2];
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator-=(U _scalar)
    {
        this->m_value[0] -= _scalar;
        this->m_value[1] -= _scalar;
        this->m_value[2] -= _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator-=(Matrix<3, 3, U> const& _m)
    {
        this->m_value[0] -= _m[0];
        this->m_value[1] -= _m[1];
        this->m_value[2] -= _m[2];
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator*=(U _scalar)
    {
        this->m_value[0] *= _scalar;
        this->m_value[1] *= _scalar;
        this->m_value[2] *= _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator*=(Matrix<3, 3, U> const& _m)
    {
        return (*this = *this * _m);
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator/=(U _scalar)
    {
        this->m_value[0] /= _scalar;
        this->m_value[1] /= _scalar;
        this->m_value[2] /= _scalar;
        return *this;
    }

    template <typename T>
    template <typename U>
    Matrix<3, 3, T>& Matrix<3, 3, T>::operator/=(Matrix<3, 3, U> const& _m)
    {
        return *this *= inverse(_m);
    }

    template <typename T>
    Matrix<3, 3, T> operator+(Matrix<3, 3, T> const& _m)
    {
        return _m;
    }

    template <typename T>
    Matrix<3, 3, T> operator-(Matrix<3, 3, T> const& _m)
    {
        return Matrix<3, 3, T>{-_m[0], -_m[1], -_m[2]};
    }

    template <typename T>
    Matrix<3, 3, T> operator+(const Matrix<3, 3, T>& _m, const T& _scalar)
    {
        return Matrix<3, 3, T>{
            _m[0] + _scalar,
            _m[1] + _scalar,
            _m[2] + _scalar};
    }

    template <typename T>
    Matrix<3, 3, T> operator+(const T& _scalar, const Matrix<3, 3, T>& _m)
    {
        return Matrix<3, 3, T>{
            _m[0] + _scalar,
            _m[1] + _scalar,
            _m[2] + _scalar};
    }

    template <typename T>
    Matrix<3, 3, T> operator+(const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2)
    {
        return Matrix<3, 3, T>{
            _m1[0] + _m2[0],
            _m1[1] + _m2[1],
            _m1[2] + _m2[2]};
    }

    template <typename T>
    Matrix<3, 3, T> operator-(const Matrix<3, 3, T>& _m, const T& _scalar)
    {
        return Matrix<3, 3, T>{
            _m[0] - _scalar,
            _m[1] - _scalar,
            _m[2] - _scalar};
    }

    template <typename T>
    Matrix<3, 3, T> operator-(const T& _scalar, const Matrix<3, 3, T>& _m)
    {
        return Matrix<3, 3, T>{
            _scalar - _m[0],
            _scalar - _m[1],
            _scalar - _m[2]};
    }

    template <typename T>
    Matrix<3, 3, T> operator-(const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2)
    {
        return Matrix<3, 3, T>{
            _m1[0] - _m2[0],
            _m1[1] - _m2[1],
            _m1[2] - _m2[2]};
    }

    template <typename T>
    Matrix<3, 3, T> operator*(const Matrix<3, 3, T>& _m, const T& _scalar)
    {
        return Matrix<3, 3, T>{
            _m[0] * _scalar,
            _m[1] * _scalar,
            _m[2] * _scalar};
    }

    template <typename T>
    Matrix<3, 3, T> operator*(const T& _scalar, const Matrix<3, 3, T>& _m)
    {
        return Matrix<3, 3, T>{
            _m[0] * _scalar,
            _m[1] * _scalar,
            _m[2] * _scalar};
    }

    template <typename T>
    Matrix<3, 3, T> operator*(const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2)
    {
        return Matrix<3, 3, T>{
            _m1[0].x * _m2[0].x + _m1[1].x * _m2[0].y + _m1[2].x * _m2[0].z,
            _m1[0].x * _m2[1].x + _m1[1].x * _m2[1].y + _m1[2].x * _m2[1].z,
            _m1[0].x * _m2[2].x + _m1[1].x * _m2[2].y + _m1[2].x * _m2[2].z,
            
            _m1[0].y * _m2[0].x + _m1[1].y * _m2[0].y + _m1[2].y * _m2[0].z,
            _m1[0].y * _m2[1].x + _m1[1].y * _m2[1].y + _m1[2].y * _m2[1].z,
            _m1[0].y * _m2[2].x + _m1[1].y * _m2[2].y + _m1[2].y * _m2[2].z,
            
            _m1[0].z * _m2[0].x + _m1[1].z * _m2[0].y + _m1[2].z * _m2[0].z,
            _m1[0].z * _m2[1].x + _m1[1].z * _m2[1].y + _m1[2].z * _m2[1].z,
            _m1[0].z * _m2[2].x + _m1[1].z * _m2[2].y + _m1[2].z * _m2[2].z

            //Param with column first
            //_m1[0].x * _m2[0].x + _m1[1].x * _m2[0].y + _m1[2].x * _m2[0].z,
            //_m1[0].y * _m2[0].x + _m1[1].y * _m2[0].y + _m1[2].y * _m2[0].z,
            //_m1[0].z * _m2[0].x + _m1[1].z * _m2[0].y + _m1[2].z * _m2[0].z,
            //
            //_m1[0].x * _m2[1].x + _m1[1].x * _m2[1].y + _m1[2].x * _m2[1].z,
            //_m1[0].y * _m2[1].x + _m1[1].y * _m2[1].y + _m1[2].y * _m2[1].z,
            //_m1[0].z * _m2[1].x + _m1[1].z * _m2[1].y + _m1[2].z * _m2[1].z,
            //
            //_m1[0].x * _m2[2].x + _m1[1].x * _m2[2].y + _m1[2].x * _m2[2].z,
            //_m1[0].y * _m2[2].x + _m1[1].y * _m2[2].y + _m1[2].y * _m2[2].z,
            //_m1[0].z * _m2[2].x + _m1[1].z * _m2[2].y + _m1[2].z * _m2[2].z,
            //false
        };
    }

    template<typename T>
    Vector<3, T> operator* (const Matrix<3, 3, T>& _m, const Vector<3, T>& _v)
    {
        return Vector<3, T>{
            _m[0].x * _v.x + _m[1].x * _v.y + _m[2].x * _v.z,
            _m[0].y * _v.x + _m[1].y * _v.y + _m[2].y * _v.z,
            _m[0].z * _v.x + _m[1].z * _v.y + _m[2].z * _v.z
        };
    }

    template <typename T>
    Matrix<3, 3, T> operator/(const Matrix<3, 3, T>& _m, const T& _scalar)
    {
        return Matrix<3, 3, T>{
            _m[0] / _scalar,
            _m[1] / _scalar,
            _m[2] / _scalar};
    }

    template <typename T>
    Matrix<3, 3, T> operator/(const T& _scalar, const Matrix<3, 3, T>& _m)
    {
        return Matrix<3, 3, T>{
            _scalar / _m[0],
            _scalar / _m[1],
            _scalar / _m[2]};
    }

    template <typename T>
    Matrix<3, 3, T> operator/(const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2)
    {
        return Matrix<3, 3, T>{
            _m1[0] / _m2[0],
            _m1[1] / _m2[1],
            _m1[2] / _m2[2]};
    }

    template <typename T>
    constexpr bool operator==(const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2)
    {
        return (_m1[0] == _m2[0]) && (_m1[1] == _m2[1]) && (_m1[2] == _m2[2]);
    }

    template <typename T>
    constexpr bool operator!=(const Matrix<3, 3, T>& _m1, const Matrix<3, 3, T>& _m2)
    {
        return (_m1[0] != _m2[0]) || (_m1[1] != _m2[1]) || (_m1[2] != _m2[2]);
    }
}
