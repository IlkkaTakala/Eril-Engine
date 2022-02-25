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
	return dist6(rng);
}

float RandomFloatInRange(float min, float max)
{
	std::uniform_real_distribution<> dist6(min, max);
	return dist6(rng);
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
