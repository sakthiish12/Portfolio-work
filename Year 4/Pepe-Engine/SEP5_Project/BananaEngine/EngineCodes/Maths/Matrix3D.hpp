#pragma once

namespace BE
{
	// Constructors ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Mat3<T>::Mat3(T _m00, T _m01, T _m02, T _m03,
				  T _m10, T _m11, T _m12, T _m13,
				  T _m20, T _m21, T _m22, T _m23,
				  T _m30, T _m31, T _m32, T _m33)
	{
		m[0][0] = _m00; m[0][1] = _m01; m[0][2] = _m02; m[0][3] = _m03,
		m[1][0] = _m10; m[1][1] = _m11; m[1][2] = _m12; m[1][3] = _m13,
		m[2][0] = _m20; m[2][1] = _m21; m[2][2] = _m22; m[2][3] = _m23;
		m[3][0] = _m30; m[3][1] = _m31; m[3][2] = _m32; m[3][3] = _m33;
	}

	template <typename T>
	Mat3<T>::Mat3(const Vec3<T>& v1, const Vec3<T>& v2, const Vec3<T>& v3, const Vec3<T>& v4)
	{
		m[0][0] = v1.x; m[0][1] = v2.x; m[0][2] = v3.x; m[0][3] = v4.x;
		m[1][0] = v1.y; m[1][1] = v2.y; m[1][2] = v3.y; m[1][3] = v4.y;
		m[2][0] = v1.z; m[2][1] = v2.z; m[2][2] = v3.z; m[2][3] = v4.z;
		m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 0;
	}

	template <typename T>
	Mat3<T>::Mat3()
	{
		memset(this->m, 0, sizeof(Mat3<T>));
	}

	//Operators --------------------------------------------------------------------------------------------------------------

	template <typename T> 
	Mat3<T> Mat3<T>::operator+(const Mat3<T>& rhs) const
	{
		Mat3<T> result{};

		for (int i = 0; i < matrixDimension; ++i)
			for (int j = 0; j < matrixDimension; ++j)
				result[i][j] = this->m[i][j] + rhs.m[i][j];

		return result;
	}

	template <typename T> 
	Mat3<T> Mat3<T>::operator-(const Mat3<T>& rhs) const
	{
		Mat3<T> result{};

		for (int i = 0; i < matrixDimension; ++i)
			for (int j = 0; j < matrixDimension; ++j)
				result.m[i][j] = this->m[i][j] - rhs.m[i][j];

		return result;
	}

	template <typename T> 
	Mat3<T> Mat3<T>::operator*(const Mat3<T>& rhs) const
	{
		Mat3<T> result{};
		
		for (int i = 0; i < matrixDimension; ++i)			//Iterate to fill the rows of new mtx
			for (int j = 0; j < matrixDimension; ++j)		//Iterate to fill the columns of new mtx
				for (int k = 0; k < matrixDimension; ++k)	//Iterate to add up the value for the current resultant slot
					result.m[i][j] += this->m[i][k] * rhs.m[k][j];
		
		return result;
	}

	template <typename T> 
	Mat3<T> Mat3<T>::operator*(const T scalar) const
	{
		Mat3<T> result{};
		
		for (int i = 0; i < matrixDimension; ++i)
			for (int j = 0; j < matrixDimension; ++j)
				result.m[i][j] = this->m[i][j] * scalar;
		
		return result;
	}

	template <typename T> 
	Mat3<T> Mat3<T>::operator/(const T scalar) const
	{
		Mat3<T> result{};

		for (int i = 0; i < matrixDimension; ++i)
			for (int j = 0; j < matrixDimension; ++j)
				result.m[i][j] = this->m[i][j] / scalar;

		return result;
	}

	template <typename T> 
	Mat3<T>& Mat3<T>::operator+=(const Mat3<T>& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	template <typename T> 
	Mat3<T>& Mat3<T>::operator-=(const Mat3<T>& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	template <typename T> 
	Mat3<T>& Mat3<T>::operator*=(const Mat3<T>& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	template <typename T> 
	Mat3<T>& Mat3<T>::operator*=(const T scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	template <typename T> 
	Mat3<T>& Mat3<T>::operator/=(const T scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	template <typename T> 
	Vec3<T> Mat3<T>::operator*(const Vec3<T>& v) const
	{
		Vec3<T> result{};

		result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
		result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
		result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];

		return result;
	}

	template <typename T> 
	bool Mat3<T>::operator==(const Mat3<T>& rhs) const
	{
		for (int i = 0; i < matrixDimension; ++i)
		{
			for (int j = 0; j < matrixDimension; ++j)
			{
				//As long as 1 element is different then its not the same matrix
				if (!Math::IsEqual(this->m[i][j], rhs.m[i][j]))
					return false;
			}
		}

		return true;
	}

	template <typename T> 
	bool Mat3<T>::operator!=(const Mat3<T>& rhs) const
	{
		return !(*this == rhs);
	}

	template <typename T> 
	typename Mat3<T>::Mat3Proxy Mat3<T>::operator[](unsigned int r)
	{
		return Mat3<T>::Mat3Proxy(*this, r);
	}

	template <typename T> 
	typename Mat3<T>::Mat3ProxyConst Mat3<T>::operator[](unsigned int r) const
	{
		return Mat3<T>::Mat3ProxyConst(*this, r);
	}


	// Proxy -----------------------------------------------------------------------------------------------------------------

	template <typename T>
	Mat3<T>::Mat3Proxy::Mat3Proxy(Mat3<T>& _mtx, unsigned int _r) : mtx(_mtx), row(_r)
	{
	}

	template <typename T>
	Mat3<T>::Mat3ProxyConst::Mat3ProxyConst(const Mat3<T>& _mtx, unsigned int _r) : mtx(_mtx), row(_r)
	{
	}

	template <typename T> 
	T& Mat3<T>::Mat3Proxy::operator[](unsigned int c)
	{
		return mtx.m[row][c];
	}

	template <typename T> 
	const T& Mat3<T>::Mat3ProxyConst::operator[](unsigned int c) const
	{
		return mtx.m[row][c];
	}


	// Transformation --------------------------------------------------------------------------------------------------------

	template <typename T>
	Mat3<T>& Mat3<T>::SetScale(T sX, T sY, T sZ)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[0][0] = sX;
		m[1][1] = sY;
		m[2][2] = sZ;
		m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetScale(const Vec3<T>& _scales)
	{
		return SetScale(_scales.x, _scales.y, _scales.z);
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetTranslation(T dX, T dY, T dZ)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
		m[0][3] = dX;
		m[1][3] = dY;
		m[2][3] = dZ;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetTranslation(const Vec3<T>& _trans)
	{
		return SetTranslation(_trans.x, _trans.y, _trans.z);
	}


	template <typename T>
	Mat3<T>& Mat3<T>::SetRotX_Deg(T angle)
	{
		return this->SetRotX_Rad(Math::DegToRad(angle));
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetRotY_Deg(T angle)
	{
		return this->SetRotY_Rad(Math::DegToRad(angle));
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetRotZ_Deg(T angle)
	{
		return this->SetRotZ_Rad(Math::DegToRad(angle));
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetRotX_Rad(T angle)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[0][0] = m[3][3] = 1;
		m[1][1] = m[2][2] = Math::CosRad(angle);
		m[2][1] = Math::SinRad(angle);
		m[1][2] = -m[2][1];
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetRotY_Rad(T angle)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[1][1] = m[3][3] = 1;
		m[0][0] = m[2][2] = Math::CosRad(angle);
		m[0][2] = Math::SinRad(angle);
		m[2][0] = -m[0][2];
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetRotZ_Rad(T angle)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[2][2] = m[3][3] = 1;
		m[0][0] = m[1][1] = Math::CosRad(angle);
		m[1][0] = Math::SinRad(angle);
		m[0][1] = -m[1][0];
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetRotation(const Quaternion<T>& _quat)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[0][0] = 1 - (2 * _quat.GetJ() * _quat.GetJ()) - (2 * _quat.GetK() * _quat.GetK());
		m[0][1] = 2 * (_quat.GetI() * _quat.GetJ() - _quat.GetK() * _quat.GetR());
		m[0][2] = 2 * (_quat.GetI() * _quat.GetK() + _quat.GetJ() * _quat.GetR());
		m[1][0] = 2 * (_quat.GetI() * _quat.GetJ() + _quat.GetK() * _quat.GetR());
		m[1][1] = 1 - (2 * _quat.GetI() * _quat.GetI()) - (2 * _quat.GetK() * _quat.GetK());
		m[1][2] = 2 * (_quat.GetJ() * _quat.GetK() - _quat.GetI() * _quat.GetR());
		m[2][0] = 2 * (_quat.GetI() * _quat.GetK() - _quat.GetJ() * _quat.GetR());
		m[2][1] = 2 * (_quat.GetJ() * _quat.GetK() + _quat.GetI() * _quat.GetR());
		m[2][2] = 1 - (2 * _quat.GetI() * _quat.GetI()) - (2 * _quat.GetJ() * _quat.GetJ());
		m[3][3] = 1;

		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetLineProj(const Vec3<T>& lineDirection)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		T sqMag = lineDirection.SqMag();

		m[0][0] =			(lineDirection.x * lineDirection.x) / sqMag;
		m[1][1] =			(lineDirection.y * lineDirection.y) / sqMag;
		m[2][2] =			(lineDirection.z * lineDirection.z) / sqMag;
		m[0][1] = m[1][0] = (lineDirection.x * lineDirection.y) / sqMag;
		m[0][2] = m[2][0] = (lineDirection.x * lineDirection.z) / sqMag;
		m[1][2] = m[2][1] = (lineDirection.y * lineDirection.z) / sqMag;
		m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetPlaneProj(const Vec3<T>& planeNormal)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		T sqMag = planeNormal.SqMag();

		m[0][0] = 1 - (planeNormal.x * planeNormal.x) / sqMag;
		m[1][1] = 1 - (planeNormal.y * planeNormal.y) / sqMag;
		m[2][2] = 1 - (planeNormal.z * planeNormal.z) / sqMag;
		m[0][1] = m[1][0] = -(planeNormal.x * planeNormal.y) / sqMag;
		m[0][2] = m[2][0] = -(planeNormal.x * planeNormal.z) / sqMag;
		m[1][2] = m[2][1] = -(planeNormal.y * planeNormal.z) / sqMag;
		m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetObliqueProj(const Vec3<T>& planeNormal, const Vec3<T>& direction)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		T dotProduct = planeNormal.Dot(direction);

		//If Dot Product is 0, then plane normal and projection direction are perpendicular
		//Which forms an infinite length "shadow". Don't do anything
		if (Math::IsEqual(dotProduct, T{}))
		{
			std::cout << "Warning: Matrix not set. Plane Normal and Projection Direction cannot be perpendicular\n";
			return *this;
		}

		m[0][0] = 1 - (direction.x * planeNormal.x) / dotProduct;
		m[0][1] = -(direction.x * planeNormal.y) / dotProduct;
		m[0][2] = -(direction.x * planeNormal.z) / dotProduct;
		m[1][0] = -(direction.y * planeNormal.x) / dotProduct;
		m[1][1] = 1 - (direction.y * planeNormal.y) / dotProduct;
		m[1][2] = -(direction.y * planeNormal.z) / dotProduct;
		m[2][0] = -(direction.z * planeNormal.x) / dotProduct;
		m[2][1] = -(direction.z * planeNormal.y) / dotProduct;
		m[2][2] = 1 - (direction.z * planeNormal.z) / dotProduct;
		m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetLineRefl(const Vec3<T>& lineDirection)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		T sqMag = lineDirection.SqMag();

		m[0][0] = ((2 * lineDirection.x * lineDirection.x) / sqMag) - 1;
		m[1][1] = ((2 * lineDirection.y * lineDirection.y) / sqMag) - 1;
		m[2][2] = ((2 * lineDirection.z * lineDirection.z) / sqMag) - 1;
		m[0][1] = m[1][0] = (2 * lineDirection.x * lineDirection.y) / sqMag;
		m[0][2] = m[2][0] = (2 * lineDirection.x * lineDirection.z) / sqMag;
		m[1][2] = m[2][1] = (2 * lineDirection.y * lineDirection.z) / sqMag;
		m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetPlaneRefl(const Vec3<T>& planeNormal)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		T sqMag = planeNormal.SqMag();

		m[0][0] = 1 - ((2 * planeNormal.x * planeNormal.x) / sqMag);
		m[1][1] = 1 - ((2 * planeNormal.y * planeNormal.y) / sqMag);
		m[2][2] = 1 - ((2 * planeNormal.z * planeNormal.z) / sqMag);
		m[0][1] = m[1][0] = -(2 * planeNormal.x * planeNormal.y) / sqMag;
		m[0][2] = m[2][0] = -(2 * planeNormal.x * planeNormal.z) / sqMag;
		m[1][2] = m[2][1] = -(2 * planeNormal.y * planeNormal.z) / sqMag;
		m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetObliqueRefl(const Vec3<T>& planeNormal, const Vec3<T>& direction)
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		T dotProduct = planeNormal.Dot(direction);

		//If Dot Product is 0, then plane normal and projection direction are perpendicular
		//Which forms a reflection at infinite distance. Don't do anything
		if (Math::IsEqual(dotProduct, T{}))
		{
			std::cout << "Warning: Matrix not set. Plane Normal and Reflection Direction cannot be perpendicular\n";
			return *this;
		}

		m[0][0] = 1 - (2 * direction.x * planeNormal.x) / dotProduct;
		m[0][1] = -(2 * direction.x * planeNormal.y) / dotProduct;
		m[0][2] = -(2 * direction.x * planeNormal.z) / dotProduct;
		m[1][0] = -(2 * direction.y * planeNormal.x) / dotProduct;
		m[1][1] = 1 - (2 * direction.y * planeNormal.y) / dotProduct;
		m[1][2] = -(2 * direction.y * planeNormal.z) / dotProduct;
		m[2][0] = -(2 * direction.z * planeNormal.x) / dotProduct;
		m[2][1] = -(2 * direction.z * planeNormal.y) / dotProduct;
		m[2][2] = 1 - (2 * direction.z * planeNormal.z) / dotProduct;
		m[3][3] = 1;
		return *this;
	}


	// Miscellaneous ------------------------------------------------------------------------------------------------------------
	template <typename T>
	Mat3<T>& Mat3<T>::SetIdentity()
	{
		memset(this->m, 0, sizeof(Mat3<T>));
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetInverse()
	{
		T det = this->Determinant();

		//If determinant is 0, there is no inverse
		if (Math::IsEqual(det, T{}))
		{
			std::cout << "Warning: Matrix not set. Original Matrix's Determinant is 0 so there is no inverse\n";
			return *this;
		}

		Mat3<T> temp = *this;
		
		//Linear part
		this->m[0][0] = ( temp.m[1][1] * temp.m[2][2] - temp.m[1][2] * temp.m[2][1] ) / det;
		this->m[0][1] = ( temp.m[0][2] * temp.m[2][1] - temp.m[0][1] * temp.m[2][2] ) / det;
		this->m[0][2] = ( temp.m[0][1] * temp.m[1][2] - temp.m[0][2] * temp.m[1][1] ) / det;
		this->m[1][0] = ( temp.m[1][2] * temp.m[2][0] - temp.m[1][0] * temp.m[2][2] ) / det;
		this->m[1][1] = ( temp.m[0][0] * temp.m[2][2] - temp.m[0][2] * temp.m[2][0] ) / det;
		this->m[1][2] = ( temp.m[0][2] * temp.m[1][0] - temp.m[0][0] * temp.m[1][2] ) / det;
		this->m[2][0] = ( temp.m[1][0] * temp.m[2][1] - temp.m[1][1] * temp.m[2][0] ) / det;
		this->m[2][1] = ( temp.m[0][1] * temp.m[2][0] - temp.m[0][0] * temp.m[2][1] ) / det;
		this->m[2][2] = ( temp.m[0][0] * temp.m[1][1] - temp.m[0][1] * temp.m[1][0] ) / det;

		//Translation part
		this->m[0][3] = -temp[0][3];
		this->m[1][3] = -temp[1][3];
		this->m[2][3] = -temp[2][3];

		//Remaining
		this->m[3][0] = 0;
		this->m[3][1] = 0;
		this->m[3][2] = 0;
		this->m[3][3] = 1;

		return *this;
	}

	template <typename T>
	Mat3<T>& Mat3<T>::SetTranspose()
	{
		auto swap = [this](T& lhs, T& rhs) 
		{
			T temp = lhs;
			lhs = rhs;
			rhs = temp;
		};

		swap(m[0][1], m[1][0]);
		swap(m[0][2], m[2][0]);
		swap(m[0][3], m[3][0]);
		swap(m[1][2], m[2][1]);
		swap(m[1][3], m[3][1]);
		swap(m[2][3], m[3][2]);

		return *this;
	}

	template <typename T>
	T Mat3<T>::Determinant() const
	{
		return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - 
			   m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) + 
			   m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
	}

	template <typename T>
	glm::mat<Mat3<T>::matrixDimension, Mat3<T>::matrixDimension, T> Mat3<T>::toGLM() const
	{
		return glm::mat<Mat3<T>::matrixDimension, Mat3<T>::matrixDimension, T>{m[0][0], m[0][1], m[0][2], m[0][3],
																			   m[1][0], m[1][1], m[1][2], m[1][3],
																			   m[2][0], m[2][1], m[2][2], m[2][3],
																			   m[3][0], m[3][1], m[3][2], m[3][3]};
	}


	// Non-member functions -----------------------------------------------------------------------------------------------------

	template <typename T>
	Mat3<T> operator*(T scalar, const Mat3<T>& rhs)
	{
		return rhs * scalar;
	}

	template <typename T>
	Mat3<T> operator/(T scalar, const Mat3<T>& rhs)
	{
		Mat3<T> result{};

		for (int i = 0; i < Mat3<T>::GetMatrixDimension(); ++i)
			for (int j = 0; j < Mat3<T>::GetMatrixDimension(); ++j)
				result[i][j] = scalar / rhs[i][j];

		return result;
	}

	template <typename T>
	Vec3<T> operator*(const Vec3<T>& v, const Mat3<T>& rhs)
	{
		return rhs * v;
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Mat3<T>& rhs)
	{
		for (int i = 0; i < Mat3<T>::GetMatrixDimension(); ++i)
		{
			os << "| ";
			for (int j = 0; j < Mat3<T>::GetMatrixDimension(); ++j)
			{
				os << std::fixed << std::setprecision(5) << std::setw(11) << rhs[i][j] << " ";
			}
			os << "|\n";
		}

		return os;
	}
}