#include "Vector3.h"
// /******************************************************************************/
// /*!
// \file         Vector3.h
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        212 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include <limits>
#include <cmath>
namespace Mathf
{
    template <typename T>
    constexpr Vector<3, T>::Vector(T _x, T _y, T _z) :
        x{_x},
        y{_y},
        z{_z}
    {
    }

    template <typename T>
    constexpr Vector<3, T>::Vector(T _scalar) :
        x{ _scalar },
        y{ _scalar },
        z{ _scalar }
    {
    }

    

    template <typename T>
    template <typename X, typename Y, typename Z>
    constexpr Vector<3, T>::Vector(X _x, Y _y, Z _z) :
        x{ static_cast<T>(_x) },
        y{ static_cast<T>(_y) },
        z{ static_cast<T>(_z) }
    {
    }

    template <typename T>
    template <typename U, typename V>
    constexpr Vector<3, T>::Vector(const Vector<2, U>& _v, V _scalar) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) },
        z{ static_cast<T>(_scalar) }
    {
    }

    template <typename T>
    template <typename U, typename V>
    constexpr Vector<3, T>::Vector(U _scalar, const Vector<2, V>& _v) :
        x{ static_cast<T>(_scalar) },
        y{ static_cast<T>(_v.y) },
        z{ static_cast<T>(_v.z) }
    {
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>::Vector(const Vector<3, U>& _v) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) },
        z{ static_cast<T>(_v.z) }
    {
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator=(const Vector<3, U>& _v)
    {
        this->x = static_cast<T>(_v.x);
        this->y = static_cast<T>(_v.y);
        this->z = static_cast<T>(_v.z);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator+=(const Vector<3, U>& _v)
    {
        this->x += static_cast<T>(_v.x);
        this->y += static_cast<T>(_v.y);
        this->z += static_cast<T>(_v.z);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator+=(U _scalar)
    {
        this->x += static_cast<T>(_scalar);
        this->y += static_cast<T>(_scalar);
        this->z += static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator-=(const Vector<3, U>& _v)
    {
        this->x -= static_cast<T>(_v.x);
        this->y -= static_cast<T>(_v.y);
        this->z -= static_cast<T>(_v.z);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator-=(U _scalar)
    {
        this->x -= static_cast<T>(_scalar);
        this->y -= static_cast<T>(_scalar);
        this->z -= static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator*=(const Vector<3, U>& _v)
    {
        this->x *= static_cast<T>(_v.x);
        this->y *= static_cast<T>(_v.y);
        this->z *= static_cast<T>(_v.z);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator*=(U _scalar)
    {
        this->x *= static_cast<T>(_scalar);
        this->y *= static_cast<T>(_scalar);
        this->z *= static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator/=(const Vector<3, U>& _v)
    {
        this->x /= static_cast<T>(_v.x);
        this->y /= static_cast<T>(_v.y);
        this->z /= static_cast<T>(_v.z);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<3, T>& Vector<3, T>::operator/=(U _scalar)
    {
        this->x /= static_cast<T>(_scalar);
        this->y /= static_cast<T>(_scalar);
        this->z /= static_cast<T>(_scalar);

        return *this;
    }

    template<typename T>
    Vector<2, T> Vector<3, T>::getVec2()
    {
        return Vector<2, T>(x, y);
    }

    template <typename T>
    constexpr Vector<3, T> operator+(const Vector<3, T>& _v)
    {
        return _v;
    }

    template <typename T>
    constexpr Vector<3, T> operator-(const Vector<3, T>& _v)
    {
        return Vector<3, T>{ -_v.x, -_v.y, -_v.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator+(const Vector<3, T>& _v, T _scalar)
    {
        return Vector<3, T>{ _scalar + _v.x, _scalar + _v.y , _scalar + _v.z};
    }

    template <typename T>
    constexpr Vector<3, T> operator+(T _scalar, const Vector<3, T>& _v)
    {
        return Vector<3, T>{ _v.x + _scalar, _v.y + _scalar,  _v.z + _scalar };
    }

    template <typename T>
    constexpr Vector<3, T> operator+(const Vector<3, T>& _v1, const Vector<3, T>& _v2)
    {
        return Vector<3, T>{ _v1.x + _v2.x, _v1.y + _v2.y, _v1.z + _v2.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator-(const Vector<3, T>& _v, T _scalar)
    {
        return Vector<3, T>{ _v.x - _scalar, _v.y - _scalar, _v.z - _scalar };
    }

    template <typename T>
    constexpr Vector<3, T> operator-(T _scalar, const Vector<3, T>& _v)
    {
        return Vector<3, T>{ _scalar - _v.x, _scalar - _v.y, _scalar - _v.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator-(const Vector<3, T>& _v1, const Vector<3, T>& _v2)
    {
        return Vector<3, T>{ _v1.x - _v2.x, _v1.y - _v2.y, _v1.z - _v2.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator*(const Vector<3, T>& _v, T _scalar)
    {
        return Vector<3, T>{ _v.x * _scalar, _v.y * _scalar, _v.z * _scalar };
    }

    template <typename T>
    constexpr Vector<3, T> operator*(T _scalar, const Vector<3, T>& _v)
    {
        return Vector<3, T>{ _scalar * _v.x, _scalar * _v.y, _scalar * _v.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator*(const Vector<3, T>& _v1, const Vector<3, T>& _v2)
    {
        return Vector<3, T>{ _v1.x * _v2.x, _v1.y * _v2.y, _v1.z * _v2.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator/(const Vector<3, T>& _v, T _scalar)
    {
        return Vector<3, T>{ _v.x / _scalar, _v.y / _scalar, _v.z / _scalar };
    }

    template <typename T>
    constexpr Vector<3, T> operator/(T _scalar, const Vector<3, T>& _v)
    {
        return Vector<3, T>{ _scalar / _v.x, _scalar / _v.y, _scalar / _v.z };
    }

    template <typename T>
    constexpr Vector<3, T> operator/(const Vector<3, T>& _v1, const Vector<3, T>& _v2)
    {
        return Vector<3, T>{ _v1.x / _v2.x, _v1.y / _v2.y, _v1.z / _v2.z };
    }

    template <typename T>
    constexpr bool operator==(const Vector<3, T>& _v1, const Vector<3, T>& _v2)
    {
        return( fabs(static_cast<double>(_v1.x - _v2.x)) < std::numeric_limits<double>::epsilon() && 
               fabs(static_cast<double>(_v1.y - _v2.y)) < std::numeric_limits<double>::epsilon() &&
               fabs(static_cast<double>(_v1.z - _v2.z)) < std::numeric_limits<double>::epsilon());
    }

    template <typename T>
    constexpr bool operator!=(const Vector<3, T>& _v1, const Vector<3, T>& _v2)
    {
        return !(_v1 == _v2);
    }
}
