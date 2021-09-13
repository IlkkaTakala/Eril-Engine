#pragma once
#include "BasicTypes.h"
#include "Objects/BaseObject.h"
#include "ObjectManager.h"
#include "IRender.h"

#define REN_UI 0x20
#define REN_REQUIRESBUILD 0x21

class GameLoop;
class INISettings;

extern GameLoop* Loop;
extern INISettings* INI;

void Exit();

template <class T>
class Ref : public RefHold
{
	mutable T* Pointer;
	mutable Data* DataPtr;
public:
	Ref() { Pointer = nullptr; DataPtr = nullptr; }
	Ref(T* ptr) : Pointer(ptr) { 
		DataPtr = dynamic_cast<Data*>(ptr);
		if (DataPtr == nullptr) {
			Pointer = nullptr;
			return;
		}
		ObjectManager::AddRef(dynamic_cast<RefHold*>(this));
	}

	~Ref() {
		if(Pointer != nullptr) ObjectManager::RemoveRef(dynamic_cast<RefHold*>(this));
	}

	virtual const void NullThis() const {
		Pointer = nullptr;
		DataPtr = nullptr;
	}

	virtual const long GetRecord() const override {
		return DataPtr->GetRecord();
	}
	
	Ref(const Ref& old) { 
		if (DataPtr != nullptr) ObjectManager::RemoveRef(dynamic_cast<const RefHold*>(this));
		Pointer = old.Pointer;
		DataPtr = dynamic_cast<Data*>(Pointer);
		ObjectManager::AddRef(dynamic_cast<const RefHold*>(this));
	}

	Ref& operator=(const Ref& old) {
		if (DataPtr != nullptr) ObjectManager::RemoveRef(dynamic_cast<const RefHold*>(this));
		Pointer = old.Pointer;
		DataPtr = dynamic_cast<Data*>(Pointer);
		ObjectManager::AddRef(dynamic_cast<const RefHold*>(this));
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
	base->BeginPlay();
	auto t = dynamic_cast<Tickable*>(next);
	ObjectManager::AddTick(t);
	return Ref<T>(next);
}

Data* GetObjectByName(const String& name);
