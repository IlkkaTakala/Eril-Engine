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
	void Quit();
private:
	std::thread Cleaner;
	bool bQuitting;
	void CleanRunner();
};