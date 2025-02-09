#pragma once
#include <Basic/Math.h>
#include <string>
#include <vector>

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

namespace Constants {
	namespace Record
	{
		constexpr uint8 LOADED = 0;
		constexpr uint8 SPAWNED = 1;
		constexpr uint8 CONSOLE = 2;
	}
}

struct RecordInt {
	uint64 record;

	/*
		Contains data about the object

		mod id						: 3
		server state & spawn type	: 1
		reserved					: 4
		object id					: 8 

	*/

	RecordInt(uint64 i) {
		record = i;
	}

	RecordInt() {
		record = 0;
	}

	String ToString() const {
		char hex_string[24];
		sprintf_s(hex_string, "0x%015llX", record);
		return hex_string;
	}

	RecordInt(uint32 ID, uint8 SpawnType, bool isServer, uint16 Mod = 0) {
		uint64 r = 0;
		if (isServer) r |= 1ULL << 51;
		r += (uint64)SpawnType << 48;
		r += ID;
		record = r;
	}

	uint GetModID() const {
		return uint(record >> 52);
	}

	bool GetIsServer() const {
		return uint8(record >> 48) & 0x08;
	}

	uint8 GetSpawnState() const {
		return uint8(record >> 48) & 0x07;
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

float RandomFloat();
float RandomFloatInRange(float min, float max);

struct Vector
{
	float X, Y, Z;

	Vector() : X(0.f), Y(0.f), Z(0.f) { }
	Vector(float a) : X(a), Y(a), Z(a) { }
	Vector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {};
	Vector(double X, double Y, double Z) : X((float)X), Y((float)Y), Z((float)Z) {};
	Vector(int X, int Y, int Z) : X((float)X), Y((float)Y), Z((float)Z) {};
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

	Vector Normalize() const { return *this * Math::Q_rsqrt(X * X + Y * Y + Z * Z); }
	Vector SafeNormalize() const { return Normalize(); }

	/*Vector Rotate(const Vector& In) 
	{
		Vector out;
		out = *this;
		double angleX = In.X * PI / 180.0;
		double angleY = -In.Y * PI / 180.0;
		double angleZ = In.Z * PI / 180.0;

		double s = sin(angleX);
		double c = cos(angleX);
		Vector copy = out;

		out.Y = copy.Y * c - copy.Z * s;
		out.Z = copy.Y * s + copy.Z * c;

		s = sin(angleY);
		c = cos(angleY);
		copy = out;

		out.X = copy.X * c + copy.Z * s;
		out.Z = copy.X * -s + copy.Z * c;

		s = sin(angleZ);
		c = cos(angleZ);
		copy = out;

		out.X = copy.X * c + copy.Y * -s;
		out.Y = copy.Y * c + copy.X * s;
		return out;
	}*/

	static Vector RotateByAxis(const Vector& in, const Vector& axis, float angle) {
		return in * cos(angle) + Cross(axis, in) * sin(angle) + axis * Dot(axis, in) * (1 - cos(angle));
	}

	static Vector RandomUnitVectorInCone(Vector direction, float coneAngle, Vector up = { 0, 0, 1 });

	Vector Project(const Vector& other) const;

	static Vector Cross(const Vector& x, const Vector& y) {
		return Vector(
			x.Y * y.Z - y.Y * x.Z,
			x.Z * y.X - y.Z * x.X,
			x.X * y.Y - y.X * x.Y);
	}
	static float Dot(const Vector& in1, const Vector& in2) { return in1.X* in2.X + in1.Y * in2.Y + in1.Z * in2.Z; }
	String ToString() const { return String(std::to_string(X) + ',' + std::to_string(Y) + ',' + std::to_string(Z)); }

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

	// Make Euler rotator from vector
	static Vector toEuler(const Vector& in, double angle);
};

struct Vector2D
{
	long int X, Y;

	Vector2D() { X = 0, Y = 0; }
	Vector2D(long X, long Y) : X(X), Y(Y) {};
	Vector2D(uint X, uint Y) : X(X), Y(Y) {};
	Vector2D(int X, int Y) : X(X), Y(Y) {};
	Vector2D(float X, float Y) : X(long(X)), Y(long(Y)) {};

	Vector2D operator+(const Vector2D& obj) { return Vector2D(X + obj.X, Y + obj.Y); }
	Vector2D operator-(const Vector2D& obj) { return Vector2D(X - obj.X, Y - obj.Y); }
	Vector2D operator*(const Vector2D& obj) { return Vector2D(X * obj.X, Y * obj.Y); }
	friend Vector2D operator*(const Vector2D& lhs, float rhs) { return Vector2D(lhs.X * rhs, lhs.Y * rhs); }
	friend Vector2D operator/(const Vector2D& lhs, const Vector2D& rhs) { return Vector2D(lhs.X / rhs.X, lhs.Y / rhs.Y); }
	friend Vector2D operator/(const Vector2D& lhs, const float rhs) { return Vector2D(lhs.X / rhs, lhs.Y / rhs); }

	friend bool operator==(const Vector2D& obj, const Vector2D& obj2) { return obj2.X == obj.X && obj2.Y == obj.Y; }
};