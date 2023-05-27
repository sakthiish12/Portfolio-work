// /******************************************************************************/
// /*!
// \file         Matrix.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        8 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once

namespace Mathf
{
    template<size_t C, size_t R, typename T>
    struct Matrix
    {
    };

 /*   template<typename T>
    Matrix<4, 4, T> Translate(const Matrix<4, 4, T>& _m, const Vector<3, T>& _v)
    {
        Matrix<4, 4, T> result{ _m };
        result[3] = _m[0] * _v[0] + _m[1] * _v[1] + _m[2] * _v[2] + _m[3];
        return result;
    }

    template<typename T>
    Matrix<4, 4, T> Rotate(const Matrix<4, 4, T>& _m, const T _angle, const Vector<3, T>& _v)
    {
        T const c = cos(_angle);
        T const s = sin(_angle);

        Vector<3, T> axis(normalize(_v));
        Vector<3, T> temp((T(1) - c) * axis);

        Matrix<4, 4, T> rotation;
        rotation[0][0] = c + temp[0] * axis[0];
        rotation[0][1] = temp[0] * axis[1] + s * axis[2];
        rotation[0][2] = temp[0] * axis[2] - s * axis[1];

        rotation[1][0] = temp[1] * axis[0] - s * axis[2];
        rotation[1][1] = c + temp[1] * axis[1];
        rotation[1][2] = temp[1] * axis[2] + s * axis[0];

        rotation[2][0] = temp[2] * axis[0] + s * axis[1];
        rotation[2][1] = temp[2] * axis[1] - s * axis[0];
        rotation[2][2] = c + temp[2] * axis[2];

        Matrix<4, 4, T> result;
        result[0] = _m[0] * rotation[0][0] + _m[1] * rotation[0][1] + _m[2] * rotation[0][2];
        result[1] = _m[0] * rotation[1][0] + _m[1] * rotation[1][1] + _m[2] * rotation[1][2];
        result[2] = _m[0] * rotation[2][0] + _m[1] * rotation[2][1] + _m[2] * rotation[2][2];
        result[3] = _m[3];
        return result;
    }

    template<typename T>
    Matrix<4, 4, T> Scale(const Matrix<4, 4, T>& _m, const Vector<3, T>& _v)
    {
        Matrix<4, 4, T> result{};
        result[0] = _m[0] * _v[0];
        result[1] = _m[1] * _v[1];
        result[2] = _m[2] * _v[2];
        result[3] = _m[3];
        return result;
    }

    template<typename T>
    Matrix<4, 4, T> Ortho(T _left, T _right, T _bottom, T _top)
    {
        Matrix<4, 4, T> result(static_cast<T>(1));
        result[0][0] = static_cast<T>(2) / (_right - _left);
        result[1][1] = static_cast<T>(2) / (_top - _bottom);
        result[2][2] = -static_cast<T>(1);
        result[3][0] = -(_right + _left) / (_right - _left);
        result[3][1] = -(_top + _bottom) / (_top - _bottom);
        return result;
    }*/
}