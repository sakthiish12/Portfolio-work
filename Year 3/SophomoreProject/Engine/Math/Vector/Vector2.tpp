// /******************************************************************************/
// /*!
// \file         Vector.tpp
// \project name Engine
// \author(s)    Sakthiish S/O Vijayadass
// \date         08th Oct, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        Logger
// \lines        189 Lines
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
    // constructors

    template <typename T>
    constexpr Vector<2, T>::Vector(T _x, T _y) :
        x{ _x},
        y{ _y }
    {
    }

    template <typename T>
    constexpr Vector<2, T>::Vector(T _scalar) :
        x{ _scalar },
        y{ _scalar }
    {
    }

    template <typename T>
    template <typename X, typename Y>
    constexpr Vector<2, T>::Vector(X _x, Y _y) :
        x{static_cast<T>(_x)},
        y{static_cast<T>(_y)}
    {
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>::Vector(const Vector<3, U>& _v) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) }
    {
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>::Vector(const Vector<2, U>& _v) :
        x{ static_cast<T>(_v.x) },
        y{ static_cast<T>(_v.y) }
    {
    }

    // unary arithmetic operators

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator=(const Vector<2, U>& _v)
    {
        this->x = static_cast<T>(_v.x);
        this->y = static_cast<T>(_v.y);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator+=(const Vector<2, U>& _v)
    {
        this->x += static_cast<T>(_v.x);
        this->y += static_cast<T>(_v.y);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator+=(U _scalar)
    {
        this->x += static_cast<T>(_scalar);
        this->y += static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator-=(const Vector<2, U>& _v)
    {
        this->x -= static_cast<T>(_v.x);
        this->y -= static_cast<T>(_v.y);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator-=(U _scalar)
    {
        this->x -= static_cast<T>(_scalar);
        this->y -= static_cast<T>(_scalar);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator*=(const Vector<2, U>& _v)
    {
        this->x *= static_cast<T>(_v.x);
        this->y *= static_cast<T>(_v.y);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator*=(U _scalar)
    {
        this->x *= static_cast<T>(_scalar);
        this->y *= static_cast<T>(_scalar);

        return *this;
    }

    // 
    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator/=(const Vector<2, U>& _v)
    {
        this->x /= static_cast<T>(_v.x);
        this->y /= static_cast<T>(_v.y);

        return *this;
    }

    template <typename T>
    template <typename U>
    constexpr Vector<2, T>& Vector<2, T>::operator/=(U _scalar)
    {
        this->x /= static_cast<T>(_scalar);
        this->y /= static_cast<T>(_scalar);

        return *this;
    }

    // unary operator

    template <typename T>
    constexpr Vector<2, T> operator+(const Vector<2, T>& _v)
    {
        return _v;
    }

    template <typename T>
    constexpr Vector<2, T>operator-(const Vector<2, T>& _v)
    {
        return Vector<2, T>{ -_v.x, -_v.y };
    }

    // binary operators
    template <typename T>
    constexpr Vector<2, T> operator+(const Vector<2, T>& _v, T _scalar)
    {
        return Vector<2, T>{ _v.x + _scalar, _v.y + _scalar };
    }

    template <typename T>
    constexpr Vector<2, T> operator+(T _scalar, const Vector<2, T>& _v)
    {
        return Vector<2, T>{ _scalar + _v.x , _scalar + _v.y };
    }

    template <typename T>
    constexpr Vector<2, T> operator+(const Vector<2, T>& _v1, const Vector<2, T>& _v2)
    {
        return Vector<2, T>{ _v1.x + _v2.x, _v1.y + _v2.y };
    }

    template <typename T>
    constexpr Vector<2, T> operator-(const Vector<2, T>& _v, T _scalar)
    {
        return Vector<2, T>{ _v.x - _scalar, _v.y - _scalar};
    }

    template <typename T>
    constexpr Vector<2, T> operator-(T _scalar, const Vector<2, T>& _v)
    {
        return Vector<2, T>{_scalar - _v.x, _scalar - _v.y};
    }

    template <typename T>
    constexpr Vector<2, T> operator-(const Vector<2, T>& _v1, const Vector<2, T>& _v2)
    {
        return Vector<2, T>{_v1.x - _v2.x, _v1.y - _v2.y};
    }

    template <typename T>
    constexpr Vector<2, T> operator*(const Vector<2, T>& _v, T _scalar)
    {
        return Vector<2, T>{ _v.x * _scalar, _v.y * _scalar};
    }

    template <typename T>
    constexpr Vector<2, T>operator*(T _scalar, const Vector<2, T>& _v)
    {
        return Vector<2, T>{ _scalar * _v.x, _scalar * _v.y};
    }

    template <typename T>
    constexpr Vector<2, T> operator*(const Vector<2,T>& _v1, const Vector<2, T>& _v2)
    {
        return Vector<2, T>{ _v1.x * _v2.x, _v1.y * _v2.y };
    }

    template <typename T>
    constexpr Vector<2, T> operator/(const Vector<2, T>& _v, T _scalar)
    {
        return Vector<2, T>{ _v.x / _scalar, _v.y / _scalar };
    }

    template <typename T>
    constexpr Vector<2, T> operator/(T _scalar, const Vector<2, T>& _v)
    {
        return Vector<2, T>{ _scalar / _v.x, _scalar / _v.y };
    }

    template <typename T>
    constexpr Vector<2, T> operator/(const Vector<2, T>& _v1, const Vector<2, T>& _v2)
    {
        return Vector<2, T>{ _v1.x / _v2.x, _v1.y / _v2.y };
    }

    // boolean operators
    template <typename T>
    constexpr bool operator==(const Vector<2, T>& _v1, const Vector<2, T>& _v2)
    {
        return fabs(static_cast<double>(_v1.x - _v2.x)) < std::numeric_limits<double>>::epsilon() && 
               fabs(static_cast<double>(_v1.y - _v2.y)) < std::numeric_limits<double>>::epsilon();
    }

    template <typename T>
    constexpr bool operator!=(const Vector<2, T>& _v1, const Vector<2, T>& _v2)
    {
        return !(_v1 == _v2);
    }
}
