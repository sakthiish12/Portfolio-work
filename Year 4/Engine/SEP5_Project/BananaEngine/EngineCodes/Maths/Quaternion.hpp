#pragma once

namespace BE
{
	// Constructors ----------------------------------------------------------------------------------------------------------

	template <typename T> 
	Quaternion<T>::Quaternion(T _r, T _i, T _j, T _k) : r(_r), i(_i), j(_j), k(_k)
	{
		this->Normalise();
	}

	template <typename T> 
	Quaternion<T>::Quaternion(const Vec3<T>& axisOfRotation, T rotationAngle_Deg)
	{
		Vec3<T> axis = axisOfRotation.Normalised();
		r = Math::CosDeg(rotationAngle_Deg / static_cast<T>(2));
		i = Math::SinDeg(rotationAngle_Deg / static_cast<T>(2)) * axis.x;
		j = Math::SinDeg(rotationAngle_Deg / static_cast<T>(2)) * axis.y;
		k = Math::SinDeg(rotationAngle_Deg / static_cast<T>(2)) * axis.z;
		this->Normalise();
	}

	template <typename T>
	Quaternion<T>::Quaternion(T xPitch_Deg, T yYaw_Deg, T zRoll_Deg)
	{
		//Y -> X -> Z
		//Yaw -> Pitch -> Roll
		xPitch_Deg = BEMath::Wrap(-180.0f, 180.0f, xPitch_Deg);
		yYaw_Deg   = BEMath::Wrap(-180.0f, 180.0f, yYaw_Deg);
		zRoll_Deg  = BEMath::Wrap(-180.0f, 180.0f, zRoll_Deg);

        //Old implementations
		T cy = Math::CosDeg<T>(yYaw_Deg / static_cast<T>(2));
		T sy = Math::SinDeg<T>(yYaw_Deg / static_cast<T>(2));

		T cp = Math::CosDeg<T>(xPitch_Deg / static_cast<T>(2));
		T sp = Math::SinDeg<T>(xPitch_Deg / static_cast<T>(2));		
		
		T cr = Math::CosDeg<T>(zRoll_Deg / static_cast<T>(2));
		T sr = Math::SinDeg<T>(zRoll_Deg / static_cast<T>(2));

		r = cr * cp * cy + sr * sp * sy;
		i = cr * sp * cy + sr * cp * sy;
		j = cr * cp * sy - sr * sp * cy;
		k = sr * cp * cy - cr * sp * sy;
#if 0
		r = cr * cp * cy + sr * sp * sy;
		i = cr * sp * cy - sr * cp * sy;
		j = cr * cp * sy + sr * sp * cy;
		k = sr * cp * cy - cr * sp * sy;
#endif
		this->Normalise();
    }

	template <typename T> 
	Quaternion<T>::Quaternion() : r(1), i(0), j(0), k(0)
	{

	}




	// Operators ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
	{
		Vec3<T> v1(this->i, this->j, this->k);
		Vec3<T> v2(rhs.i, rhs.j, rhs.k);

		Vec3<T> v3 = this->r * v2;
		Vec3<T> v4 = rhs.r * v1;
		Vec3<T> v5 = v1.Cross(v2);
		
		return Quaternion<T>(this->r * rhs.r - v1.Dot(v2),
							v3.x + v4.x + v5.x,
							v3.y + v4.y + v5.y,
							v3.z + v4.z + v5.z);
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	template <typename T>
	Vec3<T> Quaternion<T>::operator*(const Vec3<T>& vec) const
	{
		return this->MultiplyWithoutNormalise(Quaternion<T>(0, vec.x, vec.y, vec.z, false)).MultiplyWithoutNormalise(this->Inverse()).ToVector();
	}

	template <typename T>
	bool Quaternion<T>::operator==(const Quaternion& rhs) const
	{
		return Math::IsEqual(i, rhs.i) && Math::IsEqual(j, rhs.j) && 
			   Math::IsEqual(k, rhs.k) && Math::IsEqual(r, rhs.r);
	}

	template <typename T>
	bool Quaternion<T>::operator!=(const Quaternion& rhs) const
	{
		return !(*this == rhs);
	}

	template <typename T>
	const T& Quaternion<T>::operator[](unsigned int index) const
	{
		switch (index)
		{
		case 0:
			return r;
			break;
		case 1:
			return i;
			break;
		case 2:
			return j;
			break;
		case 3:
			return k;
			break;
		default:
			return r;
			break;
		}
	}



	// Functions ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Quaternion<T>& Quaternion<T>::Normalise()
	{
		T magnitude = this->Mag();

		// Deals with 0 quaternions
		if (Math::IsEqual(magnitude, T{}))
			return *this;

		r /= magnitude;
		i /= magnitude;
		j /= magnitude;
		k /= magnitude;

		return *this;
	}

	template <typename T>
	Quaternion<T> Quaternion<T>::Normalised() const
	{
		T magnitude = this->Mag();

		// Deals with 0 quaternions
		if (Math::IsEqual(magnitude, T{}))
			return *this;

		//Will be normalised by Quaternion's constructor
		return Quaternion<T>(r, i, j ,k );
	}

	template <typename T>
	Quaternion<T> Quaternion<T>::Inverse() const
	{
		return Quaternion<T>(r, -i, -j, -k);
	}

	template <typename T>
	Quaternion<T> Quaternion<T>::Lerp(const Quaternion& dest, T scalar) const
	{
		return Quaternion<T>( r + scalar * (dest.r - r),
							  i + scalar * (dest.i - i),
							  j + scalar * (dest.j - j),
							  k + scalar * (dest.k - k));
		//return Quaternion<T>((T{ 1 } - scalar) * r + scalar * dest.r,
		//					(T{ 1 } - scalar) * i + scalar * dest.i,
		//					(T{ 1 } - scalar) * j + scalar * dest.j,
		//					(T{ 1 } - scalar) * k + scalar * dest.k);
	}

	template <typename T>
	T Quaternion<T>::Mag() const
	{
		return Math::Sqrt<T>(i * i + j * j + k * k + r * r);
	}

	template <typename T>
	T Quaternion<T>::SqMag() const
	{
		return i * i + j * j + k * k + r * r;
	}

	template <typename T>
	Vec3<T> Quaternion<T>::ToVector() const
	{
		return Vec3<T>(i, j, k);
	}


	// Getters and Setters ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Quaternion<T>& Quaternion<T>::SetAxis(const Vec3<T>& _axis)
	{
		//Get the angle first
		T angle = this->GetRotation_Rad();

		//Normalise the axis
		Vec3<T> axis = _axis.Normalised();

		i = Math::SinRad(angle / static_cast<T>(2)) * axis.x;
		j = Math::SinRad(angle / static_cast<T>(2)) * axis.y;
		k = Math::SinRad(angle / static_cast<T>(2)) * axis.z;

		return *this;
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::SetAxis(T _x, T _y, T _z)
	{
		return this->SetAxis(Vec3<T>(_x, _y, _z));
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::SetAxis(T _axisHoriAngle_Deg, T _axisVertAngle_Deg)
	{
		return this->SetAxis(Vec3<T>(_axisHoriAngle_Deg, _axisVertAngle_Deg));
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::SetRotation_Deg(T angle)
	{
		//Get the axis first
		Vec3<T> axis = this->GetAxis();

		r = Math::CosDeg(angle / static_cast<T>(2));
		i = Math::SinDeg(angle / static_cast<T>(2)) * axis.x;
		j = Math::SinDeg(angle / static_cast<T>(2)) * axis.y;
		k = Math::SinDeg(angle / static_cast<T>(2)) * axis.z;
		
		return *this;
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::SetRotation_Rad(T angle)
	{
		//Get the axis first
		Vec3<T> axis = this->GetAxis();

		r = Math::CosRad(angle / static_cast<T>(2));
		i = Math::SinRad(angle / static_cast<T>(2)) * axis.x;
		j = Math::SinRad(angle / static_cast<T>(2)) * axis.y;
		k = Math::SinRad(angle / static_cast<T>(2)) * axis.z;
		
		return *this;
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::Set(const Vec3<T>& axis, T angle_Deg)
	{
		*this = Quaternion<T>::Quaternion(axis, angle_Deg);
		return *this;
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::Set(T xPitch_Deg, T yYaw_Deg, T zRoll_Deg)
	{
		*this = Quaternion<T>::Quaternion(xPitch_Deg, yYaw_Deg, zRoll_Deg);
		return *this;
	}

	template <typename T>
	Quaternion<T>& Quaternion<T>::Set(T _r, T _i, T _j, T _k)
	{
		*this = Quaternion<T>::Quaternion(_r, _i, _j, _k);
		return *this;
	}

	template <typename T>
	Vec3<T> Quaternion<T>::GetAxis() const
	{
		return this->ToVector().Normalised();
	}

	template <typename T>
	T Quaternion<T>::GetRotation_Deg() const
	{
		return Math::ACosDeg(r) * static_cast<T>(2);
	}

	template <typename T>
	T Quaternion<T>::GetRotation_Rad() const
	{
		return Math::ACosRad(r) * static_cast<T>(2);
	}


	template <typename T>
	Vec3<T> Quaternion<T>::GetEulerAngles() const
	{
#if 0
        //Roll Z
        T t0 = 2 * (r * i + j * k);
        T t1 = 1 - 2 * (i * i + j * j);

        //Pitch X
        T t2 = 2 * (r * j - k * i);
        t2 = Math::Clamp<T>(-1, 1, t2);

        //Yaw Y
        T t3 = 2 * (r * k + i * j);
        T t4 = 1 - 2 * (j * j + k * k);

        return Vec3<T>(Math::ASinDeg<T>(t2),
                       Math::ATanDeg<T>(t3, t4),
                       Math::ATanDeg<T>(t0, t1));
#endif

		//Roll Z
		T t0 = 2 * (r * k + i * j);
		T t1 = 1 - 2 * (k * k + i * i);

		//Pitch X
		T t2 = 2 * (r * i - k * j);
		t2 = Math::Clamp<T>(-1, 1, t2);

		//Yaw Y
		T t3 = 2 * (r * j + k * i);
		T t4 = 1 - 2 * (i * i + j * j);

		return Vec3<T>(Math::ASinDeg<T>(t2),
					   Math::ATanDeg<T>(t3, t4),
					   Math::ATanDeg<T>(t0, t1));
	}

	template <typename T>
	glm::qua<T> Quaternion<T>::toGLM() const
	{
		return glm::qua<T>{i, j, k, r};
	}

	// Private helpers ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Quaternion<T> Quaternion<T>::MultiplyWithoutNormalise(const Quaternion& rhs) const
	{
		Vec3<T> v1(this->i, this->j, this->k);
		Vec3<T> v2(rhs.i, rhs.j, rhs.k);

		Vec3<T> v3 = this->r * v2;
		Vec3<T> v4 = rhs.r * v1;
		Vec3<T> v5 = v1.Cross(v2);
		
		return Quaternion<T>(this->r * rhs.r - v1.Dot(v2),
							v3.x + v4.x + v5.x,
							v3.y + v4.y + v5.y,
							v3.z + v4.z + v5.z,
							false);
	}

	template <typename T>
	Quaternion<T>::Quaternion(T _r, T _i, T _j, T _k, bool) : r(_r), i(_i), j(_j), k(_k)
	{
		
	}


	// Non-member overloads ----------------------------------------------------------------------------------------------------------

	template <typename T>
	Vec3<T> operator*(const Vec3<T>& vec, const Quaternion<T>& quat)
	{
		return quat * vec;
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Quaternion<T>& rhs)
	{
		os << "[ RIJK: ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.GetR() << " ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.GetI() << " ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.GetJ() << " ";
		os << std::fixed << std::setprecision(5) << std::setw(11) << rhs.GetK() << " ]";
		return os;
	}
}