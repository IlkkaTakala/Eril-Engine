#pragma once
#include <BasicTypes.h>
#include <math.h>

struct Rotator
{
public:
	Rotator() : W(0.f), X(0.f), Y(0.f), Z(1.f) { }
	Rotator(float a) : W((float)cos(radians(a) * 0.5)), X(0.f), Y(0.f), Z(1.f) { float d = (float)sin(radians(a) * 0.5); X *= d; Y *= d; Z *= d; }
	Rotator(float W, float X, float Y, float Z) : W(W), X(X), Y(Y), Z(Z) { Normalize(); }
	Rotator(const Vector& euler) {
		float c1 = (float)cos(radians(euler.Z) / 2);
		float s1 = (float)sin(radians(euler.Z) / 2);
		float c2 = (float)cos(radians(euler.Y) / 2);
		float s2 = (float)sin(radians(euler.Y) / 2);
		float c3 = (float)cos(radians(euler.X) / 2);
		float s3 = (float)sin(radians(euler.X) / 2);
		float c1c2 = c1 * c2;
		float s1s2 = s1 * s2;
		W = c1c2 * c3 - s1s2 * s3;
		X = c1c2 * s3 + s1s2 * c3;
		Y = s1 * c2 * c3 + c1 * s2 * s3;
		Z = c1 * s2 * c3 - s1 * c2 * s3;
		Normalize();
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
		Normalize();
	}

	Rotator& Normalize() {
		float d = Vector::Q_rsqrt(W * W + X * X + Y * Y + Z * Z);
		W *= d;
		X *= d;
		Y *= d;
		Z *= d;
		return *this;
	}

	static Vector RotatorToEuler(const Rotator& vec) {

	}

	inline float Yaw() const {
		return (float)asin(-2.0f * (X * Z - W * Y));
	}

	inline float Pitch() const {
		return (float)atan2(2.0f * (Y * Z + W * X), W * W - X * X - Y * Y + Z * Z);
	}

	inline float Roll() const {
		return (float)atan2(2.0f * (X * Y + W * Z), W * W + X * X - Y * Y - Z * Z);
	}

	inline float YawDegrees() const {
		return degrees(Yaw());
	}
	inline float PitchDegrees() const {
		return degrees(Pitch());
	}
	inline float RollDegrees() const {
		return degrees(Roll());
	}

	inline Vector GetUpVector() {
		return RotateVector({ 0, 0, 1 }).Normalize();
	}

	inline Vector GetRightVector() {
		return RotateVector({ 1, 0, 0 }).Normalize();
	}

	inline Vector GetForwardVector() {
		return RotateVector({ 0, 1, 0 }).Normalize();
	}

	inline Vector RotateVector(const Vector& vector) {
		return *this * vector * Rotator(W, -X, -Y, -Z);
	}

	static Rotator FromAxisAngle(float angle, const Vector& axis) {
		Rotator r;
		r.X = axis.X * (float)sin(angle / 2.0f);
		r.Y = axis.Y * (float)sin(angle / 2.0f);
		r.Z = axis.Z * (float)sin(angle / 2.0f);
		r.W = (float)cos(angle / 2.0f);
		return r;
	}

	inline Rotator RotateAroundAxis(float angle, const Vector& axis) const {
		Rotator r;
		float a = sinf(angle / 2.0f);
		r.X = axis.X * a;
		r.Y = axis.Y * a;
		r.Z = axis.Z * a;
		r.W = cosf(angle / 2.0f);
		return (*this * r).Normalize();
	}

	friend Rotator operator*(Rotator lhs, const Rotator& rhs) {
		lhs *= rhs; return lhs;
	}
	
	Rotator& operator*=(const Rotator& rhs) {
		W = W * rhs.W - X * rhs.X - Y * rhs.Y - Z * rhs.Z;
		X = W * rhs.X + X * rhs.W + Y * rhs.Z - Z * rhs.Y;
		Y = W * rhs.Y - X * rhs.Z + Y * rhs.W + Z * rhs.X;
		Z = W * rhs.Z + X * rhs.Y - Y * rhs.X + Z * rhs.W;
		return *this;
	}

	friend Vector operator*(const Rotator& lhs, const Vector& rhs) {
		const Rotator& q = lhs;
		Vector const QuatVector(q.X, q.Y, q.Z);
		Vector const uv(Vector::Cross(QuatVector, rhs));
		Vector const uuv(Vector::Cross(QuatVector, uv));

		return rhs + ((uv * q.W) + uuv) * 2.f;
	}

	friend Vector operator*(const Vector& lhs, const Rotator& rhs) {
		Vector temp;

		temp.X = (rhs.W * lhs.X + rhs.X * 0 + rhs.Y * lhs.Z - rhs.Z * lhs.Y);
		temp.Y = (rhs.W * lhs.Y - rhs.X * lhs.Z + rhs.Y * 0 + rhs.Z * lhs.X);
		temp.Z = (rhs.W * lhs.Z + rhs.X * lhs.Y - rhs.Y * lhs.X + rhs.Z * 0);
		return temp;
	}

	friend Rotator operator+(Rotator lhs, const Rotator& rhs) {
		lhs += rhs; return lhs;
	}

	Rotator& operator+=(const Rotator& rhs) {
		W += rhs.W;
		X += rhs.X;
		Y += rhs.Y;
		Z += rhs.Z;
		return *this;
	}

	friend Rotator operator-(Rotator lhs, const Rotator& rhs) {
		lhs -= rhs; return lhs;
	}

	Rotator& operator-=(const Rotator& rhs) {
		W -= rhs.W; 
		X -= rhs.X;
		Y -= rhs.Y;
		Z -= rhs.Z;
		return *this;
	}

	Rotator& operator=(const Rotator& obj) { W = obj.W, X = obj.X, Y = obj.Y, Z = obj.Z; return *this; }

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

public:
	float W;
	float X, Y, Z;
};

