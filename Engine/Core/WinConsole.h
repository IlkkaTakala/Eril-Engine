#pragma once

namespace Console {

	void CreateOld();

	void Init();

	void Create();
	
	void Log(const char* line);

	void Error(const char* line);

	void Warning(const char* line);

	void GetLogs(int lines, char* data, size_t& datasize);

	void Flush();

	void Execute(const char* command);
}
