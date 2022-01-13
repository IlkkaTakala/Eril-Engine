#include "pch.h"
#include <iostream>
#include "defines.h"
#include <sstream>

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
		std::getline(std::cin, data);

		uint id = CompileScript(data.c_str());

		//std::cout << id;

		EvaluateScript(id);
		CleanScript(id);
	}
	return 0;
}