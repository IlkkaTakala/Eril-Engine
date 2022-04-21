#include "Math.h"

inline float Math::Clamp(float value, float min, float max)
{
	if (value <= min) return min;
	if (value >= max) return max;
	return value;

}

inline bool Math::Equals(float a, float b, float epsilon)
{
	return a <= b + epsilon && a >= b - epsilon;
}