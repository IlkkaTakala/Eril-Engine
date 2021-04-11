#pragma once
#include "BasicTypes.h"
#include <string>
#include <list>
#include <map>
#include <thread>
#include "IRender.h"

class BaseObject;
class Mesh;

class GC
{
public:
	GC(std::string dir);
	~GC();
	static void AddObject(BaseObject* obj);
	static void RemoveObject(BaseObject* obj);
	//bool LoadObj(std::string Name, MeshData* Data);
	void Quit();
	static std::list<Data*> Pointers;
private:
	//static std::map<std::string, MeshData*> LoadedMeshes;
	std::map<std::string, std::ifstream*> ModelStreams;
	std::string ActiveDir;
	std::thread Cleaner;
	bool bQuitting;
	void CleanRunner();
};