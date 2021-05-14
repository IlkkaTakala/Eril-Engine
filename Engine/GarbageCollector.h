#pragma once
#include "BasicTypes.h"
#include <map>
#include <thread>
#include "IRender.h"

class BaseObject;
class Mesh;

class GC
{
public:
	GC();
	~GC();
	static void AddObject(BaseObject* obj);
	static void RemoveObject(BaseObject* obj);
	void Quit();
	static std::map<String, Data*> Pointers;
private:

	std::string ActiveDir;
	std::thread Cleaner;
	bool bQuitting;
	void CleanRunner();
};