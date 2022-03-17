#include "BasicTypes.h"

#include <sstream>
#include <iterator>
#include <vector>
#include <random>

template <typename Out>
void split(const String& s, char delim, Out result) {
	std::istringstream iss(s);
	String item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}

std::vector<String> split(const String& s, char delim) {
	std::vector<String> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

std::random_device dev;
std::mt19937 rng(dev());

float RandomFloat()
{
	std::uniform_real_distribution<> dist6;
	return (float)dist6(rng);
}

float RandomFloatInRange(float min, float max)
{
	std::uniform_real_distribution<> dist6(min, max);
	return (float)dist6(rng);
}

Vector Vector::RandomUnitVectorInCone(Vector direction, float coneAngle, Vector up)
{

	float mat[3][3] = { 0.f };

	const float angle = RandomFloatInRange(-coneAngle, coneAngle);
	const float rot = RandomFloatInRange(0, 2 * PI);

	Vector xaxis = Vector::Cross(up, direction);
	xaxis = xaxis.Normalize();

	Vector yaxis = Vector::Cross(direction, xaxis);
	yaxis = yaxis.Normalize();

	mat[0][0] = xaxis.X;
	mat[0][1] = yaxis.X;
	mat[0][2] = direction.X;

	mat[1][0] = xaxis.Y;
	mat[1][1] = yaxis.Y;
	mat[1][2] = direction.Y;

	mat[2][0] = xaxis.Y;
	mat[2][1] = yaxis.Y;
	mat[2][2] = direction.Y;

	Vector dirZ = Vector(mat[0][0], mat[0][1], mat[0][2]).Normalize();
	Vector dirY = Vector(mat[1][0], mat[1][1], mat[1][2]).Normalize();

	Vector result = RotateByAxis(direction, dirY, radians(angle));
	result = RotateByAxis(result, direction, rot);

	return result.Normalize();
}

Vector Vector::Project(const Vector& other) const
{
	return (Dot(*this, other) / other.LengthSquared()) * other;
}

Vector Vector::toEuler(const Vector& in, double angle)
{
	Vector out;
	Vector edit = in;
	edit.Z = in.Y;
	edit.Y = in.Z;
	double s = sin(angle);
	double c = cos(angle);
	double t = 1 - c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(in.X*in.X + in.Y*in.Y + in.Z*in.Z);
	// if (magnitude==0) throw error;
	// in.X /= magnitude;
	// in.Y /= magnitude;
	// in.Z /= magnitude;
	if ((edit.X * edit.Y * t + edit.Z * s) > 0.998) {
		out.Y = 2 * (float)atan2(edit.X * sin(angle / 2), cos(angle / 2));
		out.Z = PI / 2;
		out.X = 0;
		out.X = degrees(out.X);
		out.Y = degrees(out.Y);
		out.Z = degrees(out.Z);
		return out;
	}
	if ((edit.X * edit.Y * t + edit.Z * s) < -0.998) {
		out.Y = -2 * (float)atan2(edit.X * sin(angle / 2), cos(angle / 2));
		out.Z = -PI / 2;
		out.X = 0;
		out.X = degrees(out.X);
		out.Y = degrees(out.Y);
		out.Z = degrees(out.Z);
		return out;
	}
	out.Y = (float)atan2(edit.Y * s - edit.X * edit.Z * t, 1 - (edit.Y * edit.Y + edit.Z * edit.Z) * t);
	out.Z = (float)asin(edit.X * edit.Y * t + edit.Z * s);
	out.X = (float)atan2(edit.X * s - edit.Y * edit.Z * t, 1 - (edit.X * edit.X + edit.Z * edit.Z) * t);
	out.X = degrees(out.X);
	out.Y = degrees(out.Y);
	out.Z = degrees(out.Z);
	return out;
}
