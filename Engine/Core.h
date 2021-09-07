#pragma once
#include "BasicTypes.h"
#include "Objects/BaseObject.h"
#include "ObjectManager.h"
#include "GarbageCollector.h"

#define REN_UI 0x20
#define REN_REQUIRESBUILD 0x21

class GameLoop;
class INISettings;

extern GameLoop* Loop;
extern INISettings* INI;

void Exit();

template <class T>
class Ref
{
	T* Pointer;
public:
	Ref() { Pointer = nullptr; }
	Ref(T* ptr) : Pointer(ptr) { 
		Data* DataPtr = dynamic_cast<Data*>(ptr);
		if (DataPtr == nullptr) {
			Pointer = nullptr;
			return;
		}
		if (DataPtr->GetRecord() == 0) ObjectManager::CreateRecord(DataPtr);
		ObjectManager::AddRef(&Pointer);
	}
	~Ref() { ObjectManager::RemoveRef(&Pointer); }
	Ref(const Ref& old) { 
		Pointer = old.Pointer;
	}

	Ref& operator=(const Ref& old) {
		
		ObjectManager::RemoveRef(&Pointer);
		Pointer = old.Pointer;
		return *this;
	}
	T* operator->() const { return Pointer; }
	operator T* () const { return Pointer; }

	bool operator==(const Ref& other) { return Pointer == other.Pointer; }
	bool operator==(const void* other) { return Pointer == other; }
	bool operator!=(const Ref& other) { return Pointer != other.Pointer; }
	bool operator!=(const void* other) { return Pointer != other; }
};

template <class T = BaseObject>
Ref<T> SpawnObject()
{
	T* next = new T();
	BaseObject* base = dynamic_cast<BaseObject*>(next);
	if (base == nullptr) {
		delete next;
		return nullptr;
	}
	GC::AddObject(base);
	base->BeginPlay();
	return Ref<T>(next);
}

Data* GetObjectByName(const String& name);
