// /******************************************************************************/
// /*!
// \file         Vector4.tpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        264 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
namespace Mathf
{
    template <typename T>
    constexpr Vector<4, T>::Vector(T _x, T _y, T _z, T _w) :
        x{ _x },
        y{ _y },
        z{ _z },
        w{ _w }
    {

    }

    template <typename T>
    constexpr Vector<4, T>::Vector(T _scalar) :
        x{ _scalar },
        y{ _scalar },
        z{ _scalar },
        w{ _scalar }
    {
    }

    template <typename T>
    template <typename X, typename Y, typename Z, typename W>
    constexpr Vector<4, T>::Vector(X _x, Y _y, Z _z, W _w) :
        x{ static_cast<T>(_x) },
        y{ static_cast<T>(_y) },
        z{ static_cast<T>(_z) },
        w{ static_cast<T>(_w) }
    {
    }

    template <typename T>
    template <typename U, typename Z, typename W>
    constexpr Vector<4, T>::Vector(const Vector<2, U>& _v, Z _z, W _w) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) },
        z{ static_cast<T>(_z) },
        w{ static_cast<T>(_w) }
    {
    }

    template <typename T>
    template <typename X, typename U, typename W>
    constexpr Vector<4, T>::Vector(X _x, const Vector<2, U>& _v, W _w) :
        x{ static_cast<T>(_x) },
        y{ static_cast<T>(_v.x) },
        z{ static_cast<T>(_v.y) },
        w{ static_cast<T>(_w) }
    {
    }

    template <typename T>
    template <typename X, typename  Y, typename U>
    constexpr Vector<4, T>::Vector(X _x, Y _y, const Vector<2, U>& _v) :
        x{ static_cast<T>(_x) },
        y{ static_cast<T>(_y) },
        z{ static_cast<T>(_v.x) },
        w{ static_cast<T>(_v.y) }
    {
    }

    template <typename T>
    template <typename U, typename W>
    constexpr Vector<4, T>::Vector(const Vector<3, U>& _v, W _w) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) },
        z{ static_cast<T>(_v.z) },
        w{ static_cast<T>(_w) }
    {
    }

    template <typename T>
    template <typename U, typename X>
    constexpr Vector<4, T>::Vector(X _x, const Vector<3, U>& _v) :
        x{ static_cast<T>(_x) },
        y{ static_cast<T>(_v.x) },
        z{ static_cast<T>(_v.y) },
        w{ static_cast<T>(_v.z) }
    {
    }

    template <typename T>
    template <typename U, typename V>
    constexpr Vector<4, T>::Vector(const Vector<2, U>& _v1, const Vector<2, V>& _v2) :
        x{ static_cast<T>(_v1.x) },
        y{ static_cast<T>(_v1.y) },
        z{ static_cast<T>(_v2.x) },
        w{ static_cast<T>(_v2.y) }
    {
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>::Vector(const Vector<4, U>& _v) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) },
        z{ static_cast<T>(_v.z) },
        w{ static_cast<T>(_v.w) }
    {
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator=(const Vector<4, U>& _v)
    {
        this->x = static_cast<T>(_v.x);
        this->y = static_cast<T>(_v.y);
        this->z = static_cast<T>(_v.z);
        this->w = static_cast<T>(_v.w);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator+=(const Vector<4, U>& _v)
    {
        this->x += static_cast<T>(_v.x);
        this->y += static_cast<T>(_v.y);
        this->z += static_cast<T>(_v.z);
        this->w += static_cast<T>(_v.w);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator+=(U _scalar)
    {
        this->x += static_cast<T>(_scalar);
        this->y += static_cast<T>(_scalar);
        this->z += static_cast<T>(_scalar);
        this->w += static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator-=(const Vector<4, U>& _v)
    {
        this->x -= static_cast<T>(_v.x);
        this->y -= static_cast<T>(_v.y);
        this->z -= static_cast<T>(_v.z);
        this->w -= static_cast<T>(_v.w);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator-=(U _scalar)
    {
        this->x -= static_cast<T>(_scalar);
        this->y -= static_cast<T>(_scalar);
        this->z -= static_cast<T>(_scalar);
        this->w -= static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator*=(const Vector<4, U>& _v)
    {
        this->x *= static_cast<T>(_v.x);
        this->y *= static_cast<T>(_v.y);
        this->z *= static_cast<T>(_v.z);
        this->w *= static_cast<T>(_v.w);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator*=(U _scalar)
    {
        this->x *= static_cast<T>(_scalar);
        this->y *= static_cast<T>(_scalar);
        this->z *= static_cast<T>(_scalar);
        this->w *= static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator/=(const Vector<4, U>& _v)
    {
        this->x /= static_cast<T>(_v.x);
        this->y /= static_cast<T>(_v.y);
        this->z /= static_cast<T>(_v.z);
        this->w /= static_cast<T>(_v.w);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<4, T>& Vector<4, T>::operator/=(U _scalar)
    {
        this->x /= static_cast<T>(_scalar);
        this->y /= static_cast<T>(_scalar);
        this->z /= static_cast<T>(_scalar);
        this->w /= static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    constexpr Vector<4, T> operator+(const Vector<4, T>& _v)
    {
        return _v;
    }

    template <typename T>
    constexpr Vector<4, T> operator-(const Vector<4, T>& _v)
    {
        return Vector<4, T>{ -_v.x, -_v.y, -_v.z, -_v.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator+(const Vector<4, T>& _v, T _scalar)
    {
        return Vector<4, T>{ _scalar + _v.x, _scalar + _v.y , _scalar + _v.z,_scalar + _v.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator+(T _scalar, const Vector<4, T>& _v)
    {
        return Vector<4, T>{ _v.x + _scalar, _v.y + _scalar,  _v.z + _scalar, _v.w + _scalar };
    }

    template <typename T>
    constexpr Vector<4, T> operator+(const Vector<4, T>& _v1, const Vector<4, T>& _v2)
    {
        return Vector<4, T>{ _v1.x + _v2.x, _v1.y + _v2.y, _v1.z + _v2.z, _v1.w + _v2.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator-(const Vector<4, T>& _v, T _scalar)
    {
        return Vector<4, T>{ _v.x - _scalar, _v.y - _scalar, _v.z - _scalar, _v.w - _scalar };
    }

    template <typename T>
    constexpr Vector<4, T> operator-(T _scalar, const Vector<4, T>& _v)
    {
        return Vector<4, T>{ _scalar - _v.x, _scalar - _v.y, _scalar - _v.z, _scalar - _v.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator-(const Vector<4, T>& _v1, const Vector<4, T>& _v2)
    {
        return Vector<4, T>{ _v1.x - _v2.x, _v1.y - _v2.y, _v1.z - _v2.z, _v1.w - _v2.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator*(const Vector<4, T>& _v, T _scalar)
    {
        return Vector<4, T>{ _v.x * _scalar, _v.y * _scalar, _v.z * _scalar, _v.w * _scalar };
    }

    template <typename T>
    constexpr Vector<4, T> operator*(T _scalar, const Vector<4, T>& _v)
    {
        return Vector<4, T>{ _scalar * _v.x, _scalar * _v.y, _scalar * _v.z, _scalar * _v.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator*(const Vector<4, T>& _v1, const Vector<4, T>& _v2)
    {
        return Vector<4, T>{ _v1.x * _v2.x, _v1.y * _v2.y, _v1.z * _v2.z, _v1.w * _v2.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator/(const Vector<4, T>& _v, T _scalar)
    {
        return Vector<4, T>{ _v.x / _scalar, _v.y / _scalar, _v.z / _scalar, _v.w / _scalar };
    }

    template <typename T>
    constexpr Vector<4, T> operator/(T _scalar, const Vector<4, T>& _v)
    {
        return Vector<4, T>{ _scalar / _v.x, _scalar / _v.y, _scalar / _v.z, _scalar / _v.w };
    }

    template <typename T>
    constexpr Vector<4, T> operator/(const Vector<4, T>& _v1, const Vector<4, T>& _v2)
    {
        return Vector<4, T>{ _v1.x / _v2.x, _v1.y / _v2.y, _v1.z / _v2.z, _v1.w / _v2.w };
    }

    template <typename T>
    constexpr bool operator==(const Vector<4, T>& _v1, const Vector<4, T>& _v2)
    {
        return fabs(static_cast<double>(_v1.x - _v2.x)) < std::numeric_limits<double>::epsilon() &&
               fabs(static_cast<double>(_v1.y - _v2.y)) < std::numeric_limits<double>::epsilon() &&
               fabs(static_cast<double>(_v1.z - _v2.z)) < std::numeric_limits<double>::epsilon() &&
               fabs(static_cast<double>(_v1.w - _v2.w)) < std::numeric_limits<double>::epsilon();
    }

    template <typename T>
    constexpr bool operator!=(const Vector<4, T>& _v1, const Vector<4, T>& _v2)
    {
        return !(_v1 == _v2);
    }
}
