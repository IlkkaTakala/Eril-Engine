#include <ScriptCore.h>
#include <stdexcept>
#include <iostream>

static char* script = R"~~~(
def execute(){

isEqualBoolean(1.0000000 < 1.0000001, true)
isEqualBoolean(3 * 2 > -2 * 4, true)
isEqualBoolean(-1 / 6 <= -4 / -5, true)
isEqualBoolean(7 + -6 >= -2 + -8, true)
isEqualBoolean(2.000 == 2.000, true)
isEqualBoolean(7.0001 != 7.0000, true)
isEqualBoolean(!false, true)
}
)~~~";

int booleanCalls;

int isEqualBoolean(bool i, bool c) {
	if (i != c) {
		throw std::runtime_error("Boolean failed: " + std::to_string(i) + ", " + std::to_string(c));
	}
	booleanCalls++;
	return 0;
}

REGISTER_FUNCTION(isEqualBoolean, global, 2);

int main() {
	std::cout << "Running test Boolean\n";

	using namespace ScriptCore;

	try {
		EvaluateScript(CompileScript(script));
		if (isError()) {
			throw std::runtime_error("Script compile failed");
		}
		if (booleanCalls != 7) {
			throw std::runtime_error("Boolean calls failed");
		}
	}
	catch (std::runtime_error e) {
		std::cout << e.what();
		return 1;
	}

	std::cout << "Success\n";
	return 0;
}