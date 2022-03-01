#include "BasicTypes.h"

#include <sstream>
#include <iterator>
#include <vector>

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

Vector Vector::Project(const Vector& other) const
{
	return (Dot(*this, other) / other.LengthSquared()) * other;
}
