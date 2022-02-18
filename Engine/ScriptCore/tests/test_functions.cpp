#include <ScriptCore.h>
#include <stdexcept>
#include <iostream>

static char* script = R"~~~(
def sampleFunc(i, c){
	wasCalled(i, c);
	return i + c;

}
def execute(){
	var i = 4; 
	var c = 6; 
	isEqual(sampleFunc(i, c), 10);

}
)~~~";
int funcCalls;
int wasCalled(int i, int c) {
	funcCalls++;
	
	return 0;
}

int isEqual(int i, int c) {
	if (i != c) {
		throw std::runtime_error("Functions failed: " + std::to_string(i) + ", " + std::to_string(c));
	}
	return 0;
}

REGISTER_FUNCTION(wasCalled, global, 2);
REGISTER_FUNCTION(isEqual, global, 2);

int main() {
	std::cout << "Running test Functions\n";
	funcCalls = 0;

	using namespace ScriptCore;

	try {
		EvaluateScript(CompileScript(script));
		if (isError()) {
			throw std::runtime_error("Script compile failed");
		}
		if (funcCalls != 1) {
			throw std::runtime_error("Function calls failed");
		}
	}
	catch (std::runtime_error e) {
		std::cout << e.what();
		return 1;
	}

	std::cout << "Success\n";
	return 0;
}