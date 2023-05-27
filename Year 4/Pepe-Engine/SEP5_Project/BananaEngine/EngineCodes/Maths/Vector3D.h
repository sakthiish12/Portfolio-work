#pragma once
#ifndef Vector3D_H
#define Vector3D_H
#include "MathLib.h"
namespace BE
{
	// This vector uses Left Hand Coordinates with Left = +X, Up = +Y, Forward = +Z

	template <typename T>
	class Vec3
	{
	public:
		T x;
		T y;
		T z;

		// Constructors
		Vec3(T x, T y, T z);						// Creates a vector by components
		Vec3(const glm::vec3& _v);							// Creates a vector from glm
		Vec3(T horiAngle_Deg, T vertAngle_Deg);		// Creates a unit vector from a given horizontal and vertical angle.
		Vec3();										// Default vector
		constexpr Vec3(T x);

		// Operators
		Vec3 operator+(const Vec3& rhs) const;				// Add 2 vectors
		Vec3 operator-(const Vec3& rhs) const;				// Subtract 2 vectors
		Vec3 operator*(const T scalar) const;				// Scalar multiply a vector by a scalar
		Vec3 operator*(const Vec3& rhs) const;				// Scalar multiply a vector component-wise
		Vec3 operator/(const T scalar) const;				// Scalar divide a vector by a scalar
		Vec3 operator/(const Vec3& rhs) const;				// Scalar divide a vector component-wise
		Vec3 operator-() const;								// Flip the direction of a vector
		Vec3& operator+=(const Vec3& rhs);
		Vec3& operator-=(const Vec3& rhs);
		Vec3& operator*=(const T scalar);
		Vec3& operator*=(const Vec3& rhs);
		Vec3& operator/=(const T scalar);
		Vec3& operator/=(const Vec3& rhs);
		bool operator==(const Vec3& rhs) const;
		bool operator!=(const Vec3& rhs) const;
		T &operator[](unsigned int index);
		const T &operator[](unsigned int index) const;

		// Functions that returns vector
		Vec3& Normalise();												// Normalise an input vector. This will modify the input vector.
		Vec3 Normalised() const;										// Returns a normalised vector. This will NOT modify the input vector.
		Vec3 Cross(const Vec3& rhs) const;								// Returns the cross product of the 2 vectors
		Vec3 VecTowards(const Vec3& endPoint) const;					// Find the vector from 1 point to another point
		Vec3 DirTowards(const Vec3& endPoint) const;					// Find the direction (normalised vector) from 1 point to another
		Vec3 ProjectOnto(const Vec3& projectOnto) const;				// Project a vector onto another vector projOnto
		Vec3 Rotate_Deg(const Vec3& axis, T angle_deg) const;			// Returns a rotated version of the vector by an axis and angle
		Vec3 Rotate_Rad(const Vec3& axis, T angle_rad) const;			// Returns a rotated version of the vector by an axis and angle
		Vec3 orthogonal()	const;										// Returns any vector orthogonal to the given vector

		// Functions that returns value
		T Mag() const;													// Find the magnitude of the vector
		T SqMag() const;												// Find the square magnitude of the vector
		T Dot(const Vec3& rhs) const;									// Find the dot product of 2 vectors
		T AngleBetween(const Vec3& rhs, bool isDegree = true) const;	// Find the angle between 2 vectors
		T SignedAngleBetween(const Vec3& rhs, const Vec3& axis, bool isDegree = true);		// Find the signed angle between 2 vectors, axis must be orthogonal to *this and rhs
		T DistBetween(const Vec3& endPoint) const;						// Find the distance between 2 vectors
		T XZ_DistBetween(const Vec3& endPoint) const;
		T SqDistBetween(const Vec3& endPoint) const;					// Find the square distance between 2 vectors
		T XZ_FullAngleBetween(const Vec3& rhs) const;									// Find full 360 angle on the XZ plane between 2 vectors
		T CrossProductMag(const Vec3& rhs) const;						// Find the cross product magnitude given the 2 original vectors

		//Converts to glm
		glm::vec<3, T> toGLM() const;

		// Functions that returns Quat
		Quaternion<T> get_rotation_between(const Vec3& destination) const;

		// Static functions - intended to be used like Vector3D::Left() etc
		static inline Vec3 Left() { return Vec3(-1, 0, 0); }
		static inline Vec3 Right() { return Vec3(1, 0, 0); }
		static inline Vec3 Up() { return Vec3(0, 1, 0); }
		static inline Vec3 Down() { return Vec3(0, -1, 0); }
		static inline Vec3 Forward() { return Vec3(0, 0, 1); }
		static inline Vec3 Backward() { return Vec3(0, 0, -1); }
		static inline Vec3 One() { return Vec3(1, 1, 1); }
		RTTR_ENABLE()
	private:
		static_assert(std::is_arithmetic<T>(), "Static Assertion failed: Only supports number types");
		
	};

	// Non member function overloads
	template <typename T = float>
	Vec3<T> operator*(T scalar, const Vec3<T>& rhs);

	template <typename T = float>
	Vec3<T> operator/(T scalar, const Vec3<T>& rhs);

	template <typename T = float>
	std::ostream& operator<<(std::ostream &os, const Vec3<T>& rhs); // For debug purpose


	//Template instantiation
	using Vec3f = Vec3<float>;

	template <typename T>
	using GLMVec = glm::vec<3, T>;

	extern const bool serialiseVec;

	template <typename T = float>
	float GetT(float t, float alpha, const Vec3<T>& p0, const Vec3<T>& p1);

	template <typename T = float>
	Vec3<T> CatmullRom(const Vec3<T>& p0, const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3, float t, float alpha = 0.0f);
}
#include "Vector3D.hpp"

#endif