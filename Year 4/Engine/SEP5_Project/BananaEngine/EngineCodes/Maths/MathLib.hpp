#pragma once
#include <Debugger/Debugger.h>

namespace BE
{
	namespace Math
	{
        /*------------------------------------------------------------------------------------------------------------------------
            Single value functions
        ------------------------------------------------------------------------------------------------------------------------*/
        template <typename T>
        T Sqrt(T val)
        {
            return static_cast<T>(sqrt(val));
        }

        template <typename T>
        T Abs(T value)
        {
            return abs(value);
        }

        template <typename T>
        T Floor(T value)
        {
            return static_cast<T>(floor(value));
        }

        template <typename T>
        T Ceil(T value)
        {
            return static_cast<T>(ceil(value));
        }

        template <typename T>
        T Round(T value)
        {
            return static_cast<T>(round(value));
        }

        template <typename T>
        T Power(T base, T exponent)
        {
            return static_cast<T>(pow(base, exponent));
        }


        /*------------------------------------------------------------------------------------------------------------------------
            Multi value functions
        ------------------------------------------------------------------------------------------------------------------------*/
        template <typename T>
        T Clamp(T min, T max, T value)
        {
            AssertCore(min <= max, "Min cannot be higher than Max");
            if (value < min)
                return min;
            else if (value > max)
                return max;
            else
                return value;
        }

        template <typename T>
        T Wrap(T min, T max, T value)
        {
            AssertCore(min <= max, "Min cannot be higher than Max");
            return value - static_cast<T>(Floor((static_cast<double>(value) - static_cast<double>(min)) / (static_cast<double>(max) - static_cast<double>(min)))) * (max - min);
        }

        template <typename T>
        T PingPong(T min, T max, T value)
        {
            AssertCore(min <= max, "Min cannot be higher than Max");
            if (value < min)
                return min + Abs(Wrap(min - (max - min), max, value) - min);
            else if (value > max)
                return max - Abs(Wrap(min, max + (max - min), value) - max);
            else
                return value;
        }

        template <typename T>
        T Min(T val1, T val2)
        {
            return (val1 < val2) ? val1 : val2;
        }

        template <typename T>
        T Max(T val1, T val2)
        {
            return (val1 > val2) ? val1 : val2;
        }

        template <typename T>
        T Lerp(T start, T end, T t)
        {
            return start + (end - start) * t;
        }




        /*------------------------------------------------------------------------------------------------------------------------
            Checking functions
        ------------------------------------------------------------------------------------------------------------------------*/
        template <typename T>
        bool IsInRange(T min, T max, T value)
        {
            AssertCore(min <= max, "Min cannot be higher than Max");
            return (value >= min) && (value <= max);
        }

        template <typename T>
        bool IsEqual(T val1, T val2)
        {
            return Abs<T>(val1 - val2) <= static_cast<T>(EPSILON);
        }

        /*------------------------------------------------------------------------------------------------------------------------
            Angle conversion functions
        ------------------------------------------------------------------------------------------------------------------------*/
        template <typename T>
        T DegToRad(T deg)
        {
            return static_cast<T>(deg * (PI / 180.0));
        }

        template <typename T>
        T RadToDeg(T rad)
        {
            return static_cast<T>(rad * (180.0 / PI));
        }


        /*------------------------------------------------------------------------------------------------------------------------
            Trigonometry functions
        ------------------------------------------------------------------------------------------------------------------------*/

        template <typename T>
        T SinRad(T rad)
        {
            rad = Wrap<T>(0, static_cast<T>(PI_TIMES_2), rad);
            return static_cast<T>(sin(rad));
        }

        template <typename T>
        T CosRad(T rad)
        {
            rad = Wrap<T>(0, static_cast<T>(PI_TIMES_2), rad);
            return static_cast<T>(cos(rad));
        }

        template <typename T>
        T TanRad(T rad)
        {
            rad = Wrap<T>(0, static_cast<T>(PI), rad);
            return static_cast<T>(tan(rad));
        }

        template <typename T>
        T SinDeg(T deg)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return SinRad(DegToRad(deg));
            //For integral types
            else
                return static_cast<T>(SinRad(DegToRad<double>(deg)));
        }

        template <typename T>
        T CosDeg(T deg)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return CosRad(DegToRad(deg));
            //For integral types
            else
                return static_cast<T>(CosRad(DegToRad<double>(deg)));
        }

        template <typename T>
        T TanDeg(T deg)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return TanRad(DegToRad(deg));
            //For integral types
            else
                return static_cast<T>(TanRad(DegToRad<double>(deg)));
        }

        /*------------------------------------------------------------------------------------------------------------------------
           Inverse Trigonometry functions
        ------------------------------------------------------------------------------------------------------------------------*/

        template <typename T>
        T ASinRad(T value)
        {
            value = Clamp<T>(-1, 1, value);
            return static_cast<T>(asin(value));
        }

        template <typename T>
        T ACosRad(T value)
        {
            value = Clamp<T>(-1, 1, value);
            return static_cast<T>(acos(value));
        }

        template <typename T>
        T ATanRad(T value)
        {
            return static_cast<T>(atan(value));
        }

        template <typename T> 
        T ATanRad(T y, T x)
        {
            return static_cast<T>(atan2(y, x));
        }

        template <typename T>
        T ASinDeg(T value)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return RadToDeg<T>(ASinRad<T>(value));
            //For integral types
            else
                return static_cast<T>(RadToDeg(ASinRad<double>(value)));
        }

        template <typename T>
        T ACosDeg(T value)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return RadToDeg<T>(ACosRad<T>(value));
            //For integral types
            else
                return static_cast<T>(RadToDeg(ACosRad<double>(value)));
        }

        template <typename T>
        T ATanDeg(T value)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return RadToDeg<T>(ATanRad<T>(value));
            //For integral types
            else
                return static_cast<T>(RadToDeg(ATanRad<double>(value)));
        }

        template <typename T>
        T ATanDeg(T y, T x)
        {
            //For floating types
            if constexpr (std::is_floating_point<T>::value)
                return RadToDeg<T>(ATanRad<T>(y, x));
            //For integral types
            else
                return static_cast<T>(RadToDeg(ATanRad<double>(y, x)));
        }
	}
}