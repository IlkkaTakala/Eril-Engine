#pragma once
#include <BasicTypes.h>
#include <Basic/Rotation.h>


typedef struct Transformation
{
	Transformation() {
		Location = Vector(0.f);
		Rotation = Rotator(0.f);
		Scale = Vector(1.f);
	}

	Transformation(const Vector& Loc, const Rotator& Rot, const Vector& Sca) {
		Location = Loc;
		Rotation = Rot;
		Scale = Sca;
	}

	Vector Location;
	Rotator Rotation;
	Vector Scale;

	friend Transformation operator+(Transformation lhs, const Transformation& rhs) {
		lhs += rhs;
		return lhs;
	}

	friend void operator+=(Transformation& lhs, const Transformation& rhs) {
		lhs.Location += rhs.Location;
		lhs.Rotation *= rhs.Rotation;
		lhs.Rotation.FastNormalize();
		lhs.Scale *= rhs.Scale;
	}

	static _forceinline Transformation Interpolate(const Transformation& previous, const Transformation& target, float factor) {
		Transform out;
		out.Location = previous.Location * (1 - factor) + target.Location * factor;
		out.Rotation = Rotator::Slerp(previous.Rotation, target.Rotation, factor).FastNormalize();
		out.Scale = previous.Scale * (1 - factor) + target.Scale * factor;
		return out;
	}

} Transform;