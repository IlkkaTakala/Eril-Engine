#include "pch.h"
#include <iostream>
#include "defines.h"
#include <sstream>
#include <fstream>
//#include <windows.h>
#include "Export.h"

String someCppNativeFunction(String s, int i) {
	std::ostringstream repeated;
	std::fill_n(std::ostream_iterator<String>(repeated), i, s);
	return repeated.str();
}

REGISTER_FUNCTION(someCppNativeFunction, global, 2);

int main(int argc, char* argv[])
{
	//HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	//
	//COORD c = { 200, 5000 }; 

	////Change the internal buffer size:
	//SetConsoleScreenBufferSize(hStdout, c);
	String dir = "script.txt";
	if (argc > 0) {
		for (int i = 0; i < argc; i++)
			std::cout << argv[i] << std::endl;
	}

	while (true) {
		std::cout << "Input your code: \n";
		String data;

		std::ifstream in("script.txt");

		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();

		uint id = CompileScript(buffer.str().c_str());

		for (int i = 0; i < 1; i++)
			EvaluateScript(id);
		CleanScript(id);

		std::getline(std::cin, data);
	}
	return 0;
}