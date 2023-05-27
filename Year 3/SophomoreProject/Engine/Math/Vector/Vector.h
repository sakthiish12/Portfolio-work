// /******************************************************************************/
// /*!
// \file         Vector.h
// \project name Engine
// \author(s)    Ho Zi Xing
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        56 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#pragma once
#include <math.h>

namespace Mathf
{
    template<size_t C, typename T>
    struct Vector
    {
    };

    template<typename T>
    T Dot(Vector<2, T> _v1, Vector<2, T> _v2)
    {
        return _v1.x * _v2.x + _v1.y * _v2.y;
    }

    template<typename T>
    T Dot(Vector<3, T> _v1, Vector<3, T> _v2)
    {
        return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z;
    }

    //Rotate vector about its 0,0
    template<typename T>
    Vector<2, T> Rotate2D(Vector<2, T> vector, float _radian)
    {
        float cosRot = cos(_radian);
        float sinRot = sin(_radian);
        Vector<2, T> result = { cosRot * vector.x - sinRot * vector.y, sinRot * vector.x + cosRot * vector.y };
        return result;
    }

    template <typename T>
    T Length(const Vector<2, T>& _v)
    {
        return static_cast<T>(sqrt(_v.x * _v.x + _v.y * _v.y));
    }

    template <typename T>
    T LengthSqr(const Vector<2, T>& _v)
    {
        return _v.x * _v.x + _v.y * _v.y;
    }

    template <typename T>
    T LengthSqr(const Vector<2, T>& _v1, const Vector<2, T>& _v2)
    {
        return (_v1.x - _v2.x) * (_v1.x - _v2.x) +
               (_v1.y - _v2.y) * (_v1.y - _v2.y);
    }

    template <typename T>
    T Length(const Vector<3, T>& _v)
    {
        return sqrt(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z);
    }

    template <typename T>
    T LengthSqr(const Vector<3, T>& _v)
    {
        return _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
    }

    template <typename T>
    Vector<2, T> Normalize(const Vector<2, T>& _v)
    {
        const T length = Length(_v);
        if (length == static_cast<T>(0))
            return Vector<2, T>{static_cast<T>(0), static_cast<T>(0)};
        else
            return Vector<2, T>{_v.x / length, _v.y / length};
    }

    template <typename T>
    Vector<3, T> Normalize(const Vector<3, T>& _v)
    {
        const T length = Length(_v);
        return Vector<3, T>{_v.x / length, _v.y / length, _v.z / length};
    }
};
