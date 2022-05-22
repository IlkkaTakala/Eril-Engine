#pragma once
#include <math.h>

constexpr double PI_D = 3.14159265359;
constexpr float PI = 3.14159265359f;

inline double radians(const double& v) {
	return v * PI_D / 180.0;
}

inline double degrees(const double& v) {
	return v / PI_D * 180.0;
}

inline float radians(const float& v) {
	return v * PI / 180.f;
}

inline float degrees(const float& v) {
	return v / PI * 180.f;
}

namespace Math
{
	inline float Clamp(float value, float min, float max) {
		if (value <= min) return min;
		if (value >= max) return max;
		return value;
	}

	inline bool Equals(float a, float b, float epsilon = 0.00001) {
		return a <= b + epsilon && a >= b - epsilon;
	}

	// Input angle in degrees
	inline float Sin(float a) { return (float)sin(radians(a)); }
	// Input angle in degrees
	inline float Cos(float a) { return (float)cos(radians(a)); }
	// Input angle in degrees
	inline float Tan(float a) { return (float)tan(radians(a)); }
	// Output angle in degrees
	inline float Asin(float a) { return (float)degrees(asinf(a)); }
	// Output angle in degrees
	inline float Acos(float a) { return (float)degrees(acosf(a)); }
	// Output angle in degrees
	inline float Atan(float a) { return (float)degrees(atanf(a)); }

	static float Q_rsqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck? 
		y = *(float*)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
		//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
		return y;
	}
}