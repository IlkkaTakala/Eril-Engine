#include "pch.h"
#include <iostream>
#include "defines.h"
#include <sstream>
#include <fstream>
#include <windows.h>
#include "Export.h"

String someCppNativeFunction(String s, int i) {
	std::ostringstream repeated;
	std::fill_n(std::ostream_iterator<String>(repeated), i, s);
	return repeated.str();
}

REGISTER_FUNCTION(someCppNativeFunction, global, 2);

int main()
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	
	COORD c = { 200, 5000 }; 

	//Change the internal buffer size:
	SetConsoleScreenBufferSize(hStdout, c);
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