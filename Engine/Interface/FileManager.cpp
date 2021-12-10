#include "FileManager.h"
#include <filesystem>
#include <fstream>

bool FileManager::RequestData(const String& file, String& data)
{
	namespace fs = std::filesystem;
	if (!fs::exists(file)) return false;
	std::ifstream t(file);
	std::stringstream buffer;
	buffer << t.rdbuf();
	data = buffer.str();
	return true;
}

bool FileManager::SaveData(const String& path, const String& data)
{
	return false;
}
