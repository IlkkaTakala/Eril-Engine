#include "../defines.h"
#include "../Export.h"
#include <exception>

static char* script = R"~~~(

var testVar = 2;
testVar = 4;
testVar = "Hello";
var another = 50;
testVar = testVar + another;

)~~~";


int main() {

	EvaluateScript(CompileScript(script));


	if (isError()) {
		throw std::runtime_error("Assignment failed: " );
	}
}