#pragma once
#include <Core.h>

namespace Console {

	void Create();

	bool IsOpen();

	void Close();
	
	void Log(const String& line);

	void Error(const String& line);

	void Warning(const String& line);

	void GetLogs(int lines, std::vector<String> data);

	void Execute(const String& line);
}
