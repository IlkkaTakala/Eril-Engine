#include "Rotation.h"

Rotator Rotator::LookAt(const Vector& sourcePoint, const Vector& destPoint, const Vector& front, const Vector& up)
{
	Vector toVector = (destPoint - sourcePoint).Normalize();

	//compute rotation axis
	Vector rotAxis = Vector::Cross(front, toVector).Normalize();
	if (rotAxis.LengthSquared() == 0)
		rotAxis = up;

	//find the angle around rotation axis
	float dot = Vector::Dot(front, toVector);
	float ang = acosf(dot);

	//convert axis angle to quaternion
	return FromAxisAngle(ang, rotAxis);
}
