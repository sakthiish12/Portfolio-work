#pragma once
#ifndef	Quaternion_H
#define Quaternion_H

#include "Vector3D.h"
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
namespace BE
{
	//This quaternion only stores unit quaternions
	template <typename T>
	class Quaternion
	{
		T r;	//Real part
		T i;	//Imaginary part
		T j;	//Imaginary part
		T k;	//Imaginary part

	public:
		//Constructors
		//Quaternion with i, j, k, and r component set individually, will normalise internally
		Quaternion(T _r, T _i, T _j, T _k);

		//Quaternion with rotation axis and angle in degree. Angle will be divided by 2 internally
		Quaternion(const Vec3<T>& axisOfRotation, T rotationAngle_Deg);

		//Quaternion from euler angles in degree, in YXZ order. WARNING: X cannot exceed +/-90 deg
		Quaternion(T xPitch_Deg, T yYaw_Deg, T zRoll_Deg);

		//Default unit quaternion at [1, 0, 0, 1]. r = 1.
		Quaternion();

		//Operators
		Quaternion operator*(const Quaternion& rhs) const;		// Multiply a Quaternion by another Quaternion
		Quaternion& operator*=(const Quaternion& rhs);			// Multiply a Quaternion by another Quaternion
		Vec3<T> operator*(const Vec3<T>& vec) const;			// Multiply a Quaternion by a vector

		bool operator==(const Quaternion& rhs) const;
		bool operator!=(const Quaternion& rhs) const;
		const T& operator[](unsigned int index) const;			// 0 -> r, 1 -> i, 2 -> j, 3 -> k

		//Functions
		Quaternion& Normalise();
		Quaternion Normalised() const;
		Quaternion Inverse() const;
		Quaternion Lerp(const Quaternion& dest, T scalar) const;
		T Mag() const;
		T SqMag() const;
		Vec3<T> ToVector() const;
		

		//Changes the quaternion rotation axis
		Quaternion& SetAxis(const Vec3<T>& axis);
		Quaternion& SetAxis(T x, T y, T z);
		Quaternion& SetAxis(T axisHoriAngle_Deg, T axisVertAngle_Deg);

		//Changes the quaternion rotation angle
		Quaternion& SetRotation_Deg(T angle);
		Quaternion& SetRotation_Rad(T angle);

		//Set quaternions
		Quaternion& Set(const Vec3<T>& axis, T angle_Deg);			//Angle axis
		Quaternion& Set(T xPitch_Deg, T yYaw_Deg, T zRoll_Deg);		//Euler angles in degree
		Quaternion& Set(T _r, T _i, T _j, T _k);

		//Get info about the quaternion
		Vec3<T> GetAxis() const;			//Get the axis of rotation
		T GetRotation_Deg() const;			//Get the angle of rotation about its current axis
		T GetRotation_Rad() const;			//Get the angle of rotation about its current axis
		Vec3<T> GetEulerAngles() const;		//Get the euler angle of this quaternion rotation in degree

		//
		inline T GetI() const { return i; };
		inline T GetJ() const { return j; };
		inline T GetK() const { return k; };
		inline T GetR() const { return r; };

		//Convert to GLM
		glm::qua<T> toGLM() const;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
		//Prevent compiler from optimzing my cpp away
	private:
		
		//Used internally for operator*, do not use
		Quaternion MultiplyWithoutNormalise(const Quaternion& rhs) const;
		
		//Used internally for operator*, do not use
		Quaternion(T _r, T _i, T _j, T _k, bool);

		static_assert(std::is_arithmetic<T>(), "Static Assertion failed: Only supports number types");
		//RTTR
		
	};

	template <typename T = float>
	Vec3<T> operator*(const Vec3<T>& vec, const Quaternion<T>& quat);

	template <typename T = float>
	std::ostream& operator<<(std::ostream& os, const Quaternion<T>& rhs); // For debug purpose

	//template instantiation
	using Quatf = Quaternion<float>;

	template <typename T>
	using GLMQuat = glm::qua<T>;

	//Typename alias
	extern const bool serialiseQuat;
}

#include "Quaternion.hpp"

#endif