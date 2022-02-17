#include <ScriptCore.h>
#include <stdexcept>
#include <iostream>

static char* script = R"~~~(
def execute(){

isEqual(2 * -2, -4);
isEqual(6 / 2, 3);
isEqual(6 - 2, 4);
isEqual(-6 + 2, -4);
#isEqualStr("hello " + "world", "hello world");
#isEqualStr("hello bro" - "hello ", "bro");
isEqualFloat(5.5 + 3.4, 8.9 );
isEqualFloat(-5.5 - 3.4, -8.9 );
isEqualFloat(5.5 * 3.45, 18.975 );
isEqualFloat(5.6 / 3.45, 1.6231884 );


}
)~~~";
int floatCalls;
int intCalls;
int stringCalls;
int greaterCalls;

int isEqualFloat(float i, float c, float maxRelDiff = FLT_EPSILON) {
	floatCalls++;
	// Calculate the difference.
	float diff = fabs(i - c);
	i = fabs(i);
	c = fabs(c);
	// Find the largest
	float largest = (c > i) ? c : i;

	if (diff <= largest * maxRelDiff) {	return 0; }
	else {
		throw std::runtime_error("Math failed: " + std::to_string(i) + ", " + std::to_string(c));
	}
}

int isEqual(int i, int c) {
	if (i != c) {
		throw std::runtime_error("Math failed: " + std::to_string(i) + ", " + std::to_string(c));
	}
	intCalls++;
	return 0;
}

int isEqualStr(std::string* i, std::string* c) {
	if (*i != *c) {
		throw std::runtime_error("Math failed: " + *i + ", " + *c);
	}
	stringCalls++;
	return 0;
}

REGISTER_FUNCTION(isEqual, global, 2);
REGISTER_FUNCTION(isEqualStr, global, 2);
REGISTER_FUNCTION(isEqualFloat, global, 3);


int main() {
	floatCalls = 0;
	std::cout << "Running test Math\n";

	using namespace ScriptCore;

	try {
		EvaluateScript(CompileScript(script));
		if (isError()) {
			throw std::runtime_error("Script compile failed");
		}
		if (floatCalls != 4) {
			throw std::runtime_error("Float calls failed");
		}
		if (intCalls != 4) {
			throw std::runtime_error("Int calls failed");
		}
		/*if (stringCalls != 2) {
			throw std::runtime_error("String calls failed");
		}*/
	}
	catch (std::runtime_error e) {
		std::cout << e.what();
		return 1;
	}
	
	std::cout << "Success\n";
	return 0;
}