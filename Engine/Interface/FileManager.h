#pragma once
#include <Core.h>

class FileManager
{
public:

	static bool RequestData(const String& file, String& data);
	static bool SaveData(const String& path, const String& data);
};

