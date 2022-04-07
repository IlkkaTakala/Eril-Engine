#pragma once
#include <BasicTypes.h>
#include <Basic/Math.h>

struct Rotator
{
public:
	Rotator() : W(0.f), X(0.f), Y(0.f), Z(1.f) { }
	Rotator(float a) : W((float)cos(radians(a) * 0.5)), X(0.f), Y(0.f), Z(1.f) { float d = (float)sin(radians(a) * 0.5); X *= d; Y *= d; Z *= d; Normalize(); }
	Rotator(float W, float X, float Y, float Z) : W(W), X(X), Y(Y), Z(Z) { Normalize(); }
	Rotator(float real, Vector imaginary) : W(real), X(imaginary.X), Y(imaginary.Y), Z(imaginary.Z) { }
	Rotator(const Vector& euler) {
		double c1 = cos(radians((double)euler.Z) * 0.5);
		double s1 = sin(radians((double)euler.Z) * 0.5);
		double c2 = cos(radians((double)euler.Y) * 0.5);
		double s2 = sin(radians((double)euler.Y) * 0.5);
		double c3 = cos(radians((double)euler.X) * 0.5);
		double s3 = sin(radians((double)euler.X) * 0.5);
		double c1c2 = c1 * c2;
		double s1s2 = s1 * s2;
		W = float(c1c2 * c3 - s1s2 * s3);
		X = float(c1c2 * s3 + s1s2 * c3);
		Y = float(s1 * c2 * c3 + c1 * s2 * s3);
		Z = float(c1 * s2 * c3 - s1 * c2 * s3);
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

	// Roll, Pitch, Yaw
	Vector AsEuler() const
	{
		Vector euler;
		const float sqw = W * W;
		const float sqx = X * X;
		const float sqy = Y * Y;
		const float sqz = Z * Z;
		const float test = 2.f * (Y * W - X * Z);
		
		if (Math::Equals(test, 1.0))
		{
			euler.Z = (float)(-2.0 * atan2(X, W));
			euler.X = 0;
			euler.Y = (float)(PI * 0.5);
		}
		else if (Math::Equals(test, -1.0))
		{
			euler.Z = (float)(2.0 * atan2(X, W));
			euler.X = 0;
			euler.Y = (float)(PI * -0.5);
		}
		else
		{
			euler.Z = (float)atan2(2.0 * (X * Y + Z * W), (sqx - sqy - sqz + sqw));
			euler.X = (float)atan2(2.0 * (Y * Z + X * W), (-sqx - sqy + sqz + sqw));
			euler.Y = (float)asin(Math::Clamp(test, -1.0, 1.0));
		}
		return euler;
	}

	inline Vector GetImaginary() const
	{
		return Vector(X, Y, Z);
	}

	Rotator& Normalize() {
		float d = Math::Q_rsqrt(W * W + X * X + Y * Y + Z * Z);
		W *= d;
		X *= d;
		Y *= d;
		Z *= d;
		return *this;
	}

	static float Dot(const Rotator& lhs, const Rotator& rhs)
	{
		return (lhs.X * rhs.X) + (lhs.Y * rhs.Y) + (lhs.Z * rhs.Z) + (lhs.W * rhs.W);
	}

	inline static Rotator Lerp(Rotator lhs, Rotator rhs, float time)
	{
		const float scale = 1.0f - time;
		return (lhs * scale) + (rhs * time);
	}	

	inline static Rotator Slerp(Rotator lhs, Rotator rhs, float time, float threshold)
	{
		float angle = Dot(lhs, rhs);
		if (angle < 0.0f)
		{
			lhs *= -1.0f;
			angle *= -1.0f;
		}
		if (angle <= (1 - threshold)) // spherical interpolation
		{
			const float theta = acosf(angle);
			const float invsintheta = 1 / (sinf(theta));
			const float scale = sinf(theta * (1.0f - time)) * invsintheta;
			const float invscale = sinf(theta * time) * invsintheta;
			return (lhs * scale) + (rhs * invscale);
		}
		else // linear interploation
		return Lerp(lhs, rhs, time);
	}

	static Vector RotatorToEuler(const Rotator& vec) {

	}

	inline double Yaw() const {
		double siny_cosp = 2 * (W * Z + X * Y);
		double cosy_cosp = 1 - 2 * (Y * Y + Z * Z);
		return atan2(siny_cosp, cosy_cosp);
	}

	inline double Pitch() const {
		double sinp = 2 * (W * Y - Z * X);
		if (std::abs(sinp) >= 1)
			return copysign(PI / 2, sinp);
		else
			return asin(sinp);
	}

	inline double Roll() const {
		double sinr_cosp = 2 * (W * X + Y * Z);
		double cosr_cosp = 1 - 2 * (X * X + Y * Y);
		return atan2(sinr_cosp, cosr_cosp);
	}

	inline float YawDegrees() const {
		return (float)degrees(Yaw());
	}
	inline float PitchDegrees() const {
		return (float)degrees(Pitch());
	}
	inline float RollDegrees() const {
		return (float)degrees(Roll());
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
		return *this * vector;
	}

	static Rotator FromAxisAngle(float angle, const Vector& axis) {
		Rotator r;
		float theta = sinf(angle * 0.5f);
		r.X = axis.X * theta;
		r.Y = axis.Y * theta;
		r.Z = axis.Z * theta;
		r.W = cosf(angle * 0.5f);
		return r.Normalize();
	}

	inline Rotator RotateAroundAxis(float angle, const Vector& axis) const {
		return (*this * FromAxisAngle(angle, axis)).Normalize();
	}

	friend Rotator operator*(Rotator lhs, const Rotator& rhs) {
		lhs *= rhs; return lhs;
	}

	friend Rotator operator*(Rotator lhs, float rhs) {
		lhs *= rhs; return lhs;
	}

	Rotator& operator*=(float rhs) {
		W *= rhs;
		X *= rhs;
		Y *= rhs;
		Z *= rhs;
		return *this;
	}
	
	Rotator& operator*=(const Rotator& rhs) {
		W = (rhs.W * W) - (rhs.X * X) - (rhs.Y * Y) - (rhs.Z * Z);
		X = (rhs.W * X) + (rhs.X * W) + (rhs.Y * Z) - (rhs.Z * Y);
		Y = (rhs.W * Y) + (rhs.Y * W) + (rhs.Z * X) - (rhs.X * Z);
		Z = (rhs.W * Z) + (rhs.Z * W) + (rhs.X * Y) - (rhs.Y * X);
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

