#pragma once
#ifndef Matrix3D_H
#define Matrix3D_H

#include "Vector3D.h"
#include "Quaternion.h"

namespace BE
{
	//Column major matrix
	template <typename T>
	class Mat3
	{
		//Row then Column
		constexpr static int matrixDimension{ 4 };
		T m[matrixDimension][matrixDimension];

	public:
		//Constructors
		//Create a 3D matrix by individual value
		Mat3(T m00, T m01, T m02,T m03,
			 T m10, T m11, T m12, T m13,
			 T m20, T m21, T m22, T m23,
			 T m30 = 0, T m31 = 0, T m32 = 0, T m33 = 1);
		//Row 4 is defaulted to [0 0 0 1]

		//Create a matrix by 4 vectors
		Mat3(const Vec3<T>& v1, 
			 const Vec3<T>& v2, 
			 const Vec3<T>& v3, 
			 const Vec3<T>& v4 = Vec3<T>());

		//Zero matrix
		Mat3();

		//Operators
		Mat3 operator+(const Mat3& rhs) const;						//Add 2 matrices
		Mat3 operator-(const Mat3& rhs) const;						//Subtract 2 matrices
		Mat3 operator*(const Mat3& rhs) const;						//Multiply 3 matrices
		Mat3 operator*(const T scalar) const;						//Scalar multiplication of a matrix
		Mat3 operator/(const T scalar) const;						//Scalar division of a matrix
		Mat3& operator+=(const Mat3& rhs);							//Add 2 matrices
		Mat3& operator-=(const Mat3& rhs);							//Subtract 2 matrices
		Mat3& operator*=(const Mat3& rhs);							//Multiply 3 matrices
		Mat3& operator*=(const T scalar);							//Scalar multiplication of a matrix
		Mat3& operator/=(const T scalar);							//Scalar division of a matrix
		Vec3<T> operator*(const Vec3<T>& v) const;					//Vector multiplication with a matrix
		bool operator==(const Mat3& rhs) const;
		bool operator!=(const Mat3& rhs) const;
		
		class Mat3Proxy;
		class Mat3ProxyConst;
		Mat3Proxy operator[](unsigned int r);						//Access the specific row of the matrix
		Mat3ProxyConst operator[](unsigned int r) const;			//Access the specific row of the const matrix

		//Transformation
		Mat3& SetScale(T sX, T sY, T sZ);							//Set a scaling matrix
		Mat3& SetScale(const Vec3<T>& _scales);						//Set a scaling matrix

		Mat3& SetTranslation(T dX, T dY, T dZ);						//Set a translation matrix
		Mat3& SetTranslation(const Vec3<T>& _trans);				//Set a translation matrix
		
		Mat3& SetRotX_Deg(T angle);									//Set a rotation matrix about X axis with degree angle.
		Mat3& SetRotY_Deg(T angle);									//Set a rotation matrix about Y axis with degree angle.
		Mat3& SetRotZ_Deg(T angle);									//Set a rotation matrix about Z axis with degree angle.
		Mat3& SetRotX_Rad(T angle);									//Set a rotation matrix about X axis with radian angle.
		Mat3& SetRotY_Rad(T angle);									//Set a rotation matrix about Y axis with radian angle.
		Mat3& SetRotZ_Rad(T angle);									//Set a rotation matrix about Z axis with radian angle.
		Mat3& SetRotation(const Quaternion<T>& _quat);				//Set a rotation matrix with quaternion


		Mat3& SetLineProj   (const Vec3<T>& lineDirection);							//Set a projection matrix across a line
		Mat3& SetPlaneProj  (const Vec3<T>& planeNormal);							//Set a projection matrix across a plane defined by the normal
		Mat3& SetObliqueProj(const Vec3<T>& planeNormal, const Vec3<T>& direction);	//Set a oblique projection matrix across a plane defined by the normal

		Mat3& SetLineRefl   (const Vec3<T>& lineDirection);							//Set a reflection matrix across a line
		Mat3& SetPlaneRefl  (const Vec3<T>& planeNormal);							//Set a reflection matrix across a plane defined by the normal
		Mat3& SetObliqueRefl(const Vec3<T>& planeNormal, const Vec3<T>& direction);	//Set a oblique reflection matrix across a plane defined by the normal

		//Miscellaneous
		Mat3& SetIdentity();		//Set an identity matrix
		Mat3& SetInverse();			//Set the inverse of this matrix
		Mat3& SetTranspose();		//Set the transpose of this matrix
		T Determinant() const;		//Return the determinant of this matrix

		//Convert to glm
		glm::mat<Mat3<T>::matrixDimension, Mat3<T>::matrixDimension, T> toGLM() const;

		static inline int GetMatrixDimension() { return matrixDimension; }

	private:
		static_assert(std::is_arithmetic<T>(), "Static Assertion failed: Only supports number types");

		//Proxies definition
		class Mat3Proxy
		{
			Mat3<T>& mtx;		//The matrix itself
			unsigned int row;		//Which row the matrix is being accessed

		public:
			Mat3Proxy(Mat3<T>& _mtx, unsigned int _r);
			T& operator[](unsigned int c);						//Access the specific column of the matrix
		};

		class Mat3ProxyConst
		{
			const Mat3<T>& mtx;	//The matrix itself
			unsigned int row;		//Which row the matrix is being accessed

		public:
			Mat3ProxyConst(const Mat3<T>& _mtx, unsigned int _r);
			const T& operator[](unsigned int c) const;			//Access the specific column of the const matrix
		};
	};

	//Non-member operator overloads
	template <typename T = float>
	Mat3<T> operator*(T scalar, const Mat3<T>& rhs);					//Scalar multiplication of a matrix

	template <typename T = float>
	Mat3<T> operator/(T scalar, const Mat3<T>& rhs);					//Scalar division of a matrix

	template <typename T = float>
	Vec3<T> operator*(const Vec3<T>& v, const Mat3<T>& rhs);			//Vector multiplication with a matrix

	template <typename T = float>
	std::ostream& operator<<(std::ostream& os, const Mat3<T>& rhs);		//For debug purpose
	
	//template instantiation
	using Mat3f = Mat3<float>;

	//Shorter name
	template <typename T>
	using GLMMat = glm::mat<Mat3<T>::matrixDimension, Mat3<T>::matrixDimension, T>;
}

#include "Matrix3D.hpp"

#endif