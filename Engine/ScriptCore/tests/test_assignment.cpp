#include <ScriptCore.h>
#include <stdexcept>
#include <iostream>

static char* script = R"~~~(

var testVar = 2;
isEqual(testVar, 2);

testVar = 4;
isEqual(testVar, 4);

testVar = "Hello";
isEqualStr(testVar, "Hello");

var another = 50;
isEqual(another, 50);

testVar = testVar + another;
isEqualStr(testVar, "Hello50");

)~~~";

int isEqual(int i, int c) {
	if (i != c) {
		throw std::runtime_error("Assignment failed: " + std::to_string(i) + ", " + std::to_string(c));
	}
	return 0;
}

int isEqualStr(std::string* i, std::string* c) {
	if (*i != *c) {
		throw std::runtime_error("Assignment failed: " + *i + ", " + *c);
	}
	return 0;
}

REGISTER_FUNCTION(isEqual, global, 2);
REGISTER_FUNCTION(isEqualStr, global, 2);

int main() {
	std::cout << "Running test assignment\n";
	
	using namespace ScriptCore;

	try {
		EvaluateScript(CompileScript(script));
	}
	catch (std::runtime_error e) {
		std::cout << e.what();
		return 1;
	}
	std::cout << "Success\n";
	return 0;
}