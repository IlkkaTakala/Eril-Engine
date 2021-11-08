#pragma once
#include <Core.h>

namespace Console {

	void Init();

	void Create();

	void Close();
	
	void Log(const String& line);

	void Error(const String& line);

	void Warning(const String& line);

	void GetLogs(int lines, char* data, size_t& datasize);

	void Flush();

	void Execute(const String& line);
}
