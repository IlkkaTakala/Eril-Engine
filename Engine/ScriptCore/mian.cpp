#include "pch.h"
#include <iostream>
#include "defines.h"
#include <sstream>
#include <fstream>

#include "Export.h"

String funner(String s, int i) {
	std::ostringstream repeated;
	std::fill_n(std::ostream_iterator<String>(repeated), i, s);
	return repeated.str();
}

int main()
{
	while (true) {
		std::cout << "Input your code: \n";
		String data;
		//std::getline(std::cin, data);

		std::ifstream in("script.txt");

		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();

		uint id = CompileScript(buffer.str().c_str());

		//std::cout << id;

		EvaluateScript(id);
		CleanScript(id);

		std::getline(std::cin, data);
	}
	return 0;
}