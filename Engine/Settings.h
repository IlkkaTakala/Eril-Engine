#pragma once
#include <Core.h>
#include <istream>
#include <unordered_map>
#include <fstream>

class INISettings
{
public:
	INISettings(const String& File);
	~INISettings() { file.close(); }
	bool IsValid() { return map.size() > 0; }
	const std::unordered_map<String, String>& GetSection(const String& section) { return map[section]; }
	const String GetValue(const String& section, const String& name) { return map[section][name]; }
	void SetValue(const String& section, const String& name, const String& value) { map[section][name] = value; Save(); }
private:
	std::ifstream file;
	String Path;
	std::unordered_map<String, std::unordered_map<String, String>> map;
	void parse(std::istream& in);
	void Save();
};