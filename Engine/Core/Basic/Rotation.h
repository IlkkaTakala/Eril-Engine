#pragma once
#include <BasicTypes.h>
#include <math.h>

struct Rotator
{
public:
	Rotator() : W(0.f), X(quat.X = 0.f), Y(quat.Y = 0.f), Z(quat.Z = 1.f) { }
	Rotator(float a) : W((float)cos(radians(a) * 0.5)), X(quat.X = 0.f), Y(quat.Y = 0.f), Z(quat.Z = 1.f) { quat *= (float)sin(radians(a) * 0.5); }
	Rotator(float W, float X, float Y, float Z) : W(W), X(quat.X = X), Y(quat.Y = Y), Z(quat.Z = Z) { quat = quat.Normalize(); }
	Rotator(const Vector& euler) {
		float c1 = (float)cos(radians(euler.Z) / 2);
		float s1 = (float)sin(radians(euler.Z) / 2);
		float c2 = (float)cos(radians(euler.X) / 2);
		float s2 = (float)sin(radians(euler.X) / 2);
		float c3 = (float)cos(radians(euler.Y) / 2);
		float s3 = (float)sin(radians(euler.Y) / 2);
		float c1c2 = c1 * c2;
		float s1s2 = s1 * s2;
		W = c1c2 * c3 - s1s2 * s3;
		X = c1c2 * s3 + s1s2 * c3;
		Y = s1 * c2 * c3 + c1 * s2 * s3;
		Z = c1 * s2 * c3 - s1 * c2 * s3;
		quat = quat.Normalize();
	};
	Rotator(const String& in) {
		size_t off = 0;
		size_t o_off = 0;
		W = std::stof(in.substr(o_off, off = in.find(',', off)));
		o_off = off;
		X = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1)));
		o_off = off;
		Y = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1)));
		o_off = off;
		Z = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1)));
		quat = quat.Normalize();
	}

	static Vector RotatorToEuler(const Rotator& vec) {

	}

	float Yaw() const {
		return (float)atan2(2.0f * (quat.Y * quat.Z + W * quat.X), W * W - quat.X * quat.X - quat.Y * quat.Y + quat.Z * quat.Z);
	}
	float Pitch() const {
		return (float)asin(-2.0f * (quat.X * quat.Z - W * quat.Y));
	}
	float Roll() const {
		return (float)atan2(2.0f * (quat.X * quat.Y + W * quat.Z), W * W + quat.X * quat.X - quat.Y * quat.Y - quat.Z * quat.Z);
	}

	void AddYaw();
	void AddPitch();
	void AddRoll();

	static Rotator FromAxisAngle(float angle, const Vector& axis) {
		Rotator r;
		r.quat.X = axis.X * (float)sin(angle / 2.0f);
		r.quat.Y = axis.Y * (float)sin(angle / 2.0f);
		r.quat.Z = axis.Z * (float)sin(angle / 2.0f);
		r.W = (float)cos(angle / 2.0f);
		return r;
	}

	void RotateAroundAxis(float angle, const Vector& axis) {
		Rotator r;
		r.quat.X = axis.X * (float)sin(angle / 2.0f);
		r.quat.Y = axis.Y * (float)sin(angle / 2.0f);
		r.quat.Z = axis.Z * (float)sin(angle / 2.0f);
		r.W = (float)cos(angle / 2.0f);
		*this *= r;
	}

	friend Rotator operator*(Rotator lhs, const Rotator& rhs) {
		lhs *= rhs; return lhs;
	}
	
	Rotator& operator*=(const Rotator& rhs) {
		W = W * rhs.W - Vector::Dot(quat, rhs.quat);
		quat = Vector::Cross(quat, rhs.quat) + W * rhs.quat + quat * rhs.W;
		return *this;
	}

	friend Rotator operator+(Rotator lhs, const Rotator& rhs) {
		lhs += rhs; return lhs;
	}

	Rotator& operator+=(const Rotator& rhs) {
		W = W + rhs.W;
		quat = quat + rhs.quat;
		return *this;
	}

	friend Rotator operator-(Rotator lhs, const Rotator& rhs) {
		lhs -= rhs; return lhs;
	}

	Rotator& operator-=(const Rotator& rhs) {
		W = W - rhs.W;
		quat = quat - rhs.quat;
		return *this;
	}

	Rotator operator=(const Rotator& obj) { return Rotator(W = obj.W, X = obj.X, Y = obj.Y, Z = obj.Z); }

	friend bool operator==(const Rotator& obj, const Rotator& obj2) { return obj2.W == obj.W && obj2.X == obj.X && obj2.Y == obj.Y && obj2.Z == obj.Z; }



	float& operator[](std::size_t idx) {
		switch (idx)
		{
		case 0: return W;
		case 1: return X;
		case 2: return Y;
		case 3: return Z;
		default: return W;
		};
	}

	const float& operator[](std::size_t idx) const {
		switch (idx)
		{
		case 0: return W;
		case 1: return X;
		case 2: return Y;
		case 3: return Z;
		default: return W;
		};
	}
private:
	Vector quat;

public:
	float W;
	float& X = quat.X, Y = quat.Y, Z = quat.Z;
};

