#pragma once

#include "Quaternion.h"

namespace BE
{

	// Constructors ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Vec3<T>::Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z)
	{
	}

	template <typename T>
	Vec3<T>::Vec3(const glm::vec3& _v) : x(_v.x), y(_v.y), z(_v.z)
	{
	}

	template <typename T>
	Vec3<T>::Vec3() : x(0), y(0), z(0)
	{
	}

	template <typename T>
	Vec3<T>::Vec3(T horiAngle_Deg, T vertAngle_Deg)
	{
		x = Math::CosDeg(horiAngle_Deg) * Math::CosDeg(vertAngle_Deg);
		y = Math::SinDeg(vertAngle_Deg);
		z = Math::SinDeg(horiAngle_Deg) * Math::CosDeg(vertAngle_Deg);
	}
	template <typename T>
	constexpr Vec3<T>::Vec3(T _x) : x(_x), y(_x), z(_x)
	{
	}
	// Operators -------------------------------------------------------------------------------------------------------------

	template <typename T>
	Vec3<T> Vec3<T>::operator+(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	template <typename T>
	Vec3<T> Vec3<T>::operator-(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	template <typename T>
	Vec3<T> Vec3<T>::operator*(const T scalar) const
	{
		// This is only defined in Vector operation
		return Vec3<T>(x * scalar, y * scalar, z * scalar);
	}

	template <typename T>
	Vec3<T> Vec3<T>::operator*(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	template <typename T>
	Vec3<T> Vec3<T>::operator/(const T scalar) const
	{
		// This is only defined in Vector operation
		return Vec3<T>(x / scalar, y / scalar, z / scalar);
	}

	template <typename T>
	Vec3<T> Vec3<T>::operator/(const Vec3<T>& rhs) const
	{
		return Vec3<T>(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	template <typename T>
	Vec3<T> Vec3<T>::operator-() const
	{
		return Vec3<T>(-x, -y, -z);
	}

	template <typename T>
	Vec3<T>& Vec3<T>::operator+=(const Vec3<T>& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	template <typename T>
	Vec3<T>& Vec3<T>::operator-=(const Vec3<T>& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	template <typename T>
	Vec3<T>& Vec3<T>::operator*=(const T scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	template <typename T>
	Vec3<T>& Vec3<T>::operator*=(const Vec3<T>& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	template <typename T>
	Vec3<T>& Vec3<T>::operator/=(const T scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	template <typename T>
	Vec3<T>& Vec3<T>::operator/=(const Vec3<T>& rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	template <typename T>
	bool Vec3<T>::operator==(const Vec3<T>& rhs) const
	{
		return Math::IsEqual(x, rhs.x) && Math::IsEqual(y, rhs.y) && Math::IsEqual(z, rhs.z);
	}

	template <typename T>
	bool Vec3<T>::operator!=(const Vec3<T>& rhs) const
	{
		return !(*this == rhs);
	}

	template <typename T>
	T& Vec3<T>::operator[](unsigned int index)
	{
		return const_cast<T&>(static_cast<const Vec3<T>&>(*this)[index]);
	}

	template <typename T>
	const T& Vec3<T>::operator[](unsigned int index) const
	{
		switch (index)
		{
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		default:
			return x;
			break;
		}
	}

	// Functions that returns vector -----------------------------------------------------------------------------------------

	template <typename T>
	Vec3<T>& Vec3<T>::Normalise()
	{
		T magnitude = this->Mag();

		// Deals with 0 vectors
		if (Math::IsEqual(magnitude, T{}))
			return *this;

		x /= magnitude;
		y /= magnitude;
		z /= magnitude;

		return *this;
	}

	template <typename T>
	Vec3<T> Vec3<T>::Normalised() const
	{
		T magnitude = this->Mag();

		// Deals with 0 vectors
		if (Math::IsEqual(magnitude, T{}))
			return *this;

		return Vec3<T>(x / magnitude, y / magnitude, z / magnitude);
	}

	template <typename T>
	Vec3<T> Vec3<T>::Cross(const Vec3<T>& rhs) const
	{
		return Vec3<T>(y * rhs.z - rhs.y * z,
			z * rhs.x - rhs.z * x,
			x * rhs.y - rhs.x * y);
	}

	template <typename T>
	Vec3<T> Vec3<T>::VecTowards(const Vec3<T>& endPoint) const
	{
		return endPoint - *this;
	}

	template <typename T>
	Vec3<T> Vec3<T>::DirTowards(const Vec3<T>& endPoint) const
	{
		return this->VecTowards(endPoint).Normalise();
	}

	template <typename T>
	Vec3<T> Vec3<T>::ProjectOnto(const Vec3<T>& projOnto) const
	{
		return (this->Dot(projOnto) / projOnto.Dot(projOnto)) * projOnto;
	}

	template <typename T>
	Vec3<T> Vec3<T>::Rotate_Deg(const Vec3& axis, T angle) const
	{
		return Quaternion<T>(axis, angle) * *this;
	}

	template <typename T>
	Vec3<T> Vec3<T>::Rotate_Rad(const Vec3& axis, T angle) const
	{
		return Rotate_Deg(axis, Math::RadToDeg(angle));
	}
	template <typename T>
	Vec3<T> Vec3<T>::orthogonal() const
	{
		T _x = Math::Abs(x);
		T _y = Math::Abs(y);
		T _z = Math::Abs(z);

		Vec3<T> other = _x < _y ? (_x < _z ? Vec3<T>(T{ 1 }, 0, 0) : Vec3<T>(0, 0, T{ 1 })) : (_y < _z ? Vec3<T>(0, T{ 1 }, 0) : Vec3<T>(0, 0, T{ 1 }));
		return this->Cross(other);
	}
	// Functions that returns value ---------------------------------------------------------------------------------------------

	template <typename T>
	T Vec3<T>::Mag() const
	{
		return Math::Sqrt(x * x + y * y + z * z);
	}

	template <typename T>
	T Vec3<T>::SqMag() const
	{
		return x * x + y * y + z * z;
	}

	template <typename T>
	T Vec3<T>::Dot(const Vec3<T>& rhs) const
	{
		return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
	}

	template <typename T>
	T Vec3<T>::AngleBetween(const Vec3<T>& rhs, bool isDegree) const
	{
		T mag = this->Mag() * rhs.Mag();

		// If either vector is a 0 vector, then treat the angle between the 2 vectors to be 0
		if (Math::IsEqual(mag, T{}))
			return T{};

		if (isDegree)
			return Math::ACosDeg(this->Dot(rhs) / mag);
		else
			return Math::ACosRad(this->Dot(rhs) / mag);
	}

	template <typename T>
	T Vec3<T>::SignedAngleBetween(const Vec3& rhs, const Vec3& axis, bool isDegree)
	{
		T mag = this->Mag() * rhs.Mag();

		// If either vector is a 0 vector, then treat the angle between the 2 vectors to be 0
		if (Math::IsEqual(mag, T{}))
			return T{};

		if (isDegree)
			return Math::ATanDeg(this->Cross(rhs).Dot(axis), this->Dot(rhs));
		else
			return Math::ATanRad(this->Cross(rhs).Dot(axis), this->Dot(rhs));
	}


	template <typename T>
	T Vec3<T>::DistBetween(const Vec3<T>& endPoint) const
	{
		return this->VecTowards(endPoint).Mag();
	}
	template <typename T>
	T Vec3<T>::XZ_DistBetween(const Vec3& endPoint) const
	{
		Vec3<T> a = *this;
		Vec3<T> b = endPoint;
		a.y = 0.f; b.y = 0.f;
		return a.DistBetween(b);
	}
	template <typename T>
	T Vec3<T>::SqDistBetween(const Vec3<T>& endPoint) const
	{
		return this->VecTowards(endPoint).SqMag();
	}

	template <typename T>
	T Vec3<T>::XZ_FullAngleBetween(const Vec3& rhs) const
	{
		T dot_product = rhs.x * x + rhs.z * z;
		T mag_a = Math::Sqrt(rhs.x * rhs.x + rhs.z * rhs.z);
		T mag_b = Math::Sqrt(x * x + z * z);
		T cos_theta = dot_product / (mag_a * mag_b);
		T angle = Math::ACosDeg(cos_theta);
		if (dot_product < 0) {
			angle = 360 - angle;
		}
		return angle;
	}

	template <typename T>
	T Vec3<T>::CrossProductMag(const Vec3<T>& rhs) const
	{
		// | v X u | = |v||u| sin(A)
		return this->Mag() * rhs.Mag() * Math::SinRad(this->AngleBetween_Rad(rhs));
	}

	template <typename T>
	glm::vec<3, T> Vec3<T>::toGLM() const
	{
		return glm::vec<3, T>{x, y, z};
	}

	template <typename T>
	Quaternion<T> Vec3<T>::get_rotation_between(const Vec3& destination) const
	{
		// It is important that the inputs are of equal length when
		// calculating the half-way vector.
		Vec3<T> un = this->Normalised();
		Vec3<T> vn = destination.Normalised();

		// Unfortunately, we have to check for when u == -v, as u + v
		// in this case will be (0, 0, 0), which cannot be normalized.
		if (un == -vn)
		{
			// 180 degree rotation around any orthogonal vector
			Vec3f normOrth = un.orthogonal().Normalised();
			return Quatf(0, normOrth.x, normOrth.y, normOrth.z);
		}

		Vec3f half = (un + vn).Normalised();
		Vec3f cross = un.Cross(half);
		return Quatf(un.Dot(half), cross.x, cross.y, cross.z);
	}

	// Non member functions -----------------------------------------------------------------------------------------------------

	template <typename T>
	Vec3<T> operator*(T scalar, const Vec3<T>& rhs)
	{
		return rhs * scalar;
	}

	template <typename T>
	Vec3<T> operator/(T scalar, const Vec3<T>& rhs)
	{
		return Vec3<T>(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Vec3<T>& rhs)
	{
		os << "(  XYZ: ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.x << " ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.y << " ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.z << " )";
		return os;
	}

	template <typename T>
	float GetT(float t, float alpha, const Vec3<T>& p0, const Vec3<T>& p1)
	{
		Vec3<T> d = p1 - p0;
		float a = d.Dot(d); // Dot product
		float b = std::pow(a, alpha * .5f);
		return (b + t);
	}

	template <typename T>
	Vec3<T> CatmullRom(const Vec3<T>& p0, const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3, float t /* between 0 and 1 */, float alpha /* between 0 and 1 */)
	{
		float t0 = 0.0f;
		float t1 = GetT(t0, alpha, p0, p1);
		float t2 = GetT(t1, alpha, p1, p2);
		float t3 = GetT(t2, alpha, p2, p3);
		t = std::lerp(t1, t2, t);
		Vec3<T> A1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
		Vec3<T> A2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
		Vec3<T> A3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;
		Vec3<T> B1 = (t2 - t) / (t2 - t0) * A1 + (t - t0) / (t2 - t0) * A2;
		Vec3<T> B2 = (t3 - t) / (t3 - t1) * A2 + (t - t1) / (t3 - t1) * A3;
		Vec3<T> C = (t2 - t) / (t2 - t1) * B1 + (t - t1) / (t2 - t1) * B2;
		return C;
	}
}
