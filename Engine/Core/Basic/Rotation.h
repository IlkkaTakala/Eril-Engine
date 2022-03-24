#pragma once
#include <BasicTypes.h>
#include <math.h>

struct Rotator
{
public:
	Rotator() : W(0.f), X(0.f), Y(0.f), Z(0.f) { };
	Rotator(float a) : W(a), X(a), Y(a), Z(a) { };
	Rotator(float W, float X, float Y, float Z) : W(W), X(X), Y(Y), Z(Z) {};
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
	}

	static Vector RotatorToEuler(const Rotator& vec) {

	}

	friend Rotator operator+(const Rotator& obj, const Rotator& obj2) { return Rotator(obj2.W + obj.W, obj2.X + obj.X, obj2.Y + obj.Y, obj2.Z + obj.Z); }
	friend void operator+=(Rotator& obj, const Rotator& obj2) { obj.W += obj2.W, obj.X += obj2.X; obj.Y += obj2.Y; obj.Z += obj2.Z; }

	friend Rotator operator-(const Rotator& obj, const Rotator& obj2) { return Rotator(obj.W - obj2.W, obj.X - obj2.X, obj.Y - obj2.Y, obj.Z - obj2.Z); }
	friend void operator-=(Rotator& obj, const Rotator& obj2) { obj.W -= obj2.W, obj.X -= obj2.X; obj.Y -= obj2.Y; obj.Z -= obj2.Z; }

	friend Rotator operator*(Rotator& obj, const Rotator& obj2) { obj *= obj2; return obj; }
	Rotator& operator*=(const Rotator& obj) { W *= obj.W, X *= obj.X; Y *= obj.Y; Z *= obj.Z; return *this; }

	Rotator operator=(const Rotator& obj) { return Rotator(W = obj.W, X = obj.X, Y = obj.Y, Z = obj.Z); }

	friend Rotator operator/(const Rotator& obj, const Rotator& obj2) { return Rotator(obj.W / obj2.W, obj.X / obj2.X, obj.Y / obj2.Y, obj.Z / obj2.Z); }
	//Rotation operator() (const Rotation& obj) { return Rotation(obj); }


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

public:
	float W, X, Y, Z;
};

