#pragma once
#include <math.h>
#include <string>
#include <vector>
constexpr auto PI = 3.14159265359f;

typedef unsigned int uint;

typedef unsigned char		uint8;
typedef unsigned __int16	uint16;
typedef unsigned __int32	uint32;
typedef unsigned __int64	uint64;

typedef char		int8;
typedef __int16		int16;
typedef __int32		int32;
typedef __int64		int64;

typedef std::string	String;
std::vector<String> split(const String& s, char delim);

struct RecordInt {
	uint64 record;

	RecordInt(uint64 i) {
		record = i;
	}

	RecordInt() {
		record = 0;
	}

	unsigned int GetModID() const {
		return record >> 51;
	}

	operator uint64() const {
		return record;
	}

	inline RecordInt& operator=(uint64 i) {
		record = i;
		return *this;
	}

	inline RecordInt& operator=(RecordInt& i) {
		record = i;
		return *this;
	}

	template <typename T>
	inline RecordInt& operator=(T& i) {
		record = reinterpret_cast<uint64>(i);
		return *this;
	}

	inline bool operator==(const RecordInt& rhs) {
		return record == rhs.record;
	}

	inline bool operator!=(const RecordInt& rhs) {
		return !(record == rhs.record);
	}

	template <typename T>
	inline bool operator==(const T& rhs) {
		return record == rhs;
	}

	template <typename T>
	inline bool operator!=(const T& rhs) {
		return !(record == rhs);
	}
};

inline float radians(const float& v) {
	return v * PI / 180.f;
}

struct Vector
{
	float X, Y, Z;

	Vector() { X = 0.f, Y = 0.f, Z = 0.f; }
	Vector(float a) { X = a, Y = a, Z = a; }
	Vector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {};
	Vector(String in) { 
		size_t off = 0;
		size_t o_off = 0;
		X = std::stof(in.substr(o_off, off = in.find(',', off)));
		o_off = off;
		Y = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1))); 
		o_off = off;
		Z = std::stof(in.substr(o_off + 1, off = in.find(',', off + 1)));
	}

	float Length() const { return (float)sqrt(X * X + Y * Y + Z * Z); }
	float LengthSquared() const { return (float)(X * X + Y * Y + Z * Z); }
	/*Vector Normalize() { float mag = Length(); return Vector(X / mag, Y / mag, Z / mag); }*/

	//float Length() { return; }

	Vector Normalize() const { return *this * Q_rsqrt(X * X + Y * Y + Z * Z); }
	Vector SafeNormalize() const { return Normalize(); }

	Vector Rotate(const Vector& In) 
	{
		Vector out;
		out = *this;
		float angleX = In.X * PI / 180.f;
		float angleY = -In.Y * PI / 180.f;
		float angleZ = In.Z * PI / 180.f;

		float s = (float)sin(angleX);
		float c = (float)cos(angleX);
		Vector copy = out;

		out.Y = copy.Y * c - copy.Z * s;
		out.Z = copy.Y * s + copy.Z * c;

		s = (float)sin(angleY);
		c = (float)cos(angleY);
		copy = out;

		out.X = copy.X * c + copy.Z * s;
		out.Z = copy.X * -s + copy.Z * c;

		s = (float)sin(angleZ);
		c = (float)cos(angleZ);
		copy = out;

		out.X = copy.X * c + copy.Y * -s;
		out.Y = copy.Y * c + copy.X * s;
		return out;
	}

	static Vector RotateByAxis(const Vector& in, const Vector& axis, float angle) {
		return in * cos(angle) + Cross(axis, in) * sin(angle) + axis * Dot(axis, in) * (1 - cos(angle));
	}

	static Vector Cross(const Vector& x, const Vector& y) {
		return Vector(
			x.Y * y.Z - y.Y * x.Z,
			x.Z * y.X - y.Z * x.X,
			x.X * y.Y - y.X * x.Y);
	}
	static float Dot(const Vector& in1, const Vector& in2) { return in1.X* in2.X + in1.Y * in2.Y + in1.Z * in2.Z; }
	String ToString() { return String(std::to_string(X) + ',' + std::to_string(Y) + ',' + std::to_string(Z)); }

	friend Vector operator+(const Vector& obj, const Vector& obj2) { return Vector(obj2.X + obj.X, obj2.Y + obj.Y, obj2.Z + obj.Z); }
	friend void operator+=(Vector& obj, const Vector& obj2) { obj.X += obj2.X; obj.Y += obj2.Y; obj.Z += obj2.Z; }

	friend Vector operator-(const Vector& obj, const Vector& obj2) { return Vector(obj.X - obj2.X, obj.Y - obj2.Y, obj.Z - obj2.Z); }
	friend Vector operator-(const Vector& obj) { return Vector(-obj.X, -obj.Y, -obj.Z); }
	friend void operator-=(Vector& obj, const Vector& obj2) { obj.X -= obj2.X; obj.Y -= obj2.Y; obj.Z -= obj2.Z; }

	Vector& operator*=(const Vector& obj) { X *= obj.X; Y *= obj.Y; Z *= obj.Z; return *this; }
	friend Vector operator*(Vector obj, const Vector& obj2) { obj *= obj2; return obj; }

	Vector& operator*=(const int& obj) { X *= obj; Y *= obj; Z *= obj; return *this; }
	friend Vector operator*(Vector obj, const int& obj2) { obj *= obj2; return obj; }
	friend Vector operator*(const int& obj, Vector obj2) { obj2 *= obj; return obj2; }

	Vector& operator*=(const float& obj) { X *= obj; Y *= obj; Z *= obj; return *this; }
	friend Vector operator*(Vector obj, const float& obj2) { obj *= obj2; return obj; }
	friend Vector operator*(const float& obj, Vector obj2) { obj2 *= obj; return obj2; }

	Vector& operator*=(const double& obj) { X *= (float)obj; Y *= (float)obj; Z *= (float)obj; return *this; }
	friend Vector operator*(Vector obj, const double& obj2) { obj *= obj2; return obj; }
	friend Vector operator*(const double& obj, Vector obj2) { obj2 *= obj; return obj2; }

	friend Vector operator/(const Vector& obj, const Vector& obj2) { return Vector(obj.X / obj2.X, obj.Y / obj2.Y, obj.Z / obj2.Z); }
	friend Vector operator/(const Vector& obj, const int obj2) { return Vector(obj.X / obj2, obj.Y / obj2, obj.Z / obj2); }
	friend Vector operator/(const Vector& obj, const float obj2) { return Vector(obj.X / obj2, obj.Y / obj2, obj.Z / obj2); }

	friend bool operator==(const Vector& obj, const Vector& obj2) { return obj2.X == obj.X && obj2.Y == obj.Y && obj2.Z == obj.Z; }

	float& operator[](std::size_t idx) {
		switch (idx)
		{
		case 0: return X;
		case 1: return Y;
		case 2: return Z;
		default: return X;
		};
	}

	const float& operator[](std::size_t idx) const {
		switch (idx)
		{
		case 0: return X;
		case 1: return Y;
		case 2: return Z;
		default: return X;
		};
	}

	float Q_rsqrt(float number) const
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

	static Vector toEuler(const Vector& in, double angle) {
		Vector out;
		double s = sin(angle);
		double c = cos(angle);
		double t = 1 - c;
		//  if ain.Xis is not alreadin.Y normalised then uncomment this
		// double magnitude = Math.sqrt(in.X*in.X + in.Y*in.Y + in.Z*in.Z);
		// if (magnitude==0) throw error;
		// in.X /= magnitude;
		// in.Y /= magnitude;
		// in.Z /= magnitude;
		if ((in.X * in.Y * t + in.Z * s) > 0.998) { // north pole singularitin.Y detected
			out.Y = 2 * (float)atan2(in.X * sin(angle / 2), cos(angle / 2));
			out.Z = PI / 2;
			out.X = 0;
			return 0.f;
		}
		if ((in.X * in.Y * t + in.Z * s) < -0.998) { // south pole singularitin.Y detected
			out.Y = -2 * (float)atan2(in.X * sin(angle / 2), cos(angle / 2));
			out.Z = -PI / 2;
			out.X = 0;
			return 0.f;
		}
		out.Y = (float)atan2(in.Y * s - in.X * in.Z * t, 1 - (in.Y * in.Y + in.Z * in.Z) * t);
		out.Z = (float)asin(in.X * in.Y * t + in.Z * s);
		out.X = (float)atan2(in.X * s - in.Y * in.Z * t, 1 - (in.X * in.X + in.Z * in.Z) * t);

		return out;
	}
};

struct Vector2D
{
	long int X, Y;

	Vector2D() { X = 0, Y = 0; }
	Vector2D(long X, long Y) : X(X), Y(Y) {};

	Vector2D operator+(const Vector2D& obj) { return Vector2D(X + obj.X, Y + obj.Y); }
	Vector2D operator-(const Vector2D& obj) { return Vector2D(X - obj.X, Y - obj.Y); }
	Vector2D operator*(const Vector2D& obj) { return Vector2D(X * obj.X, Y * obj.Y); }
	Vector2D operator/(const Vector2D& obj) { return Vector2D(X / obj.X, Y / obj.Y); }

	friend bool operator==(const Vector2D& obj, const Vector2D& obj2) { return obj2.X == obj.X && obj2.Y == obj.Y; }
};

struct Transformation 
{
	Vector Location;
	Vector Rotation;
	Vector Scale;
};