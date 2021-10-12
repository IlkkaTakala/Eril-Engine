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
protected:
	mutable T* Pointer;
	mutable Data* DataPtr;

	Ref(T* ptr, bool weak) : Pointer(ptr) {
		this->DataPtr = dynamic_cast<Data*>(ptr);
		if (this->DataPtr == nullptr) {
			this->Pointer = nullptr;
			return;
		}
		bWeak = weak;
		ObjectManager::AddRef(GetRecord(), this);
	}

public:
	Ref() { Pointer = nullptr; DataPtr = nullptr; }
	Ref(T* ptr) : Pointer(ptr) { 
		this->DataPtr = dynamic_cast<Data*>(ptr);
		if (this->DataPtr == nullptr) {
			this->Pointer = nullptr;
			return;
		}
		ObjectManager::AddRef(GetRecord(), this);
	}

	virtual ~Ref() {
		if(Pointer != nullptr) ObjectManager::RemoveRef(GetRecord(), this);
		NullThis();
	}

	virtual const void NullThis() const {
		this->Pointer = nullptr;
		this->DataPtr = nullptr;
	}

	virtual const RecordInt GetRecord() const override {
		return this->DataPtr ? this->DataPtr->GetRecord() : RecordInt(0);
	}

	T* GetPointer() { return Pointer; }
	
	Ref(const Ref& old) { 
		ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
	}

	Ref& operator=(const Ref& old) {
		ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
		return *this;
	}

	T* operator->() const { return Pointer; }
	operator T* () const { return Pointer; }

	bool operator==(const Ref& other) { return Pointer == other.Pointer; }
	bool operator==(const void* other) { return Pointer == other; }
	bool operator!=(const Ref& other) { return Pointer != other.Pointer; }
	bool operator!=(const void* other) { return Pointer != other; }
};

template <class T>
class RefWeak : public Ref<T>
{
public:
	RefWeak(T* ptr) : Ref<T>(ptr, true) {}
	RefWeak() : Ref<T>() {}

	RefWeak(const RefWeak& old) {
		ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
	}

	RefWeak& operator=(const RefWeak& old) {
		ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
		return *this;
	}
};

template <class T = BaseObject>
T* SpawnObject()
{
	Ref<T> next = new T();
	BaseObject* base = dynamic_cast<BaseObject*>(next.GetPointer());
	if (base == nullptr) {
		next->DestroyObject();
		return nullptr;
	}
	base->BeginPlay();
	auto t = dynamic_cast<Tickable*>(next.GetPointer());
	ObjectManager::AddTick(t);
	return next;
}

Data* GetObjectByName(const String& name);
