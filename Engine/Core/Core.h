#pragma once
#include "BasicTypes.h"
#include "Objects/BaseObject.h"
#include "ObjectManager.h"
#include "Interface/IRender.h"
#include <WinConsole.h>

class GameLoop;
class INISettings;
class GameState;

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
	Ref() { Pointer = nullptr; DataPtr = nullptr; bWeak = false; }
	Ref(T* ptr) : Pointer(ptr) { 
		this->DataPtr = dynamic_cast<Data*>(ptr);
		if (this->DataPtr == nullptr) {
			this->Pointer = nullptr;
			return;
		}
		bWeak = false;
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
		this->bWeak = old.bWeak;
		ObjectManager::AddRef(old->GetRecord(), this);
		//if (this->DataPtr != nullptr && this->DataPtr == this->Pointer) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
	}

	Ref& operator=(const Ref& old) {
		this->bWeak = old.bWeak;
		ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr && this->DataPtr == this->Pointer) ObjectManager::RemoveRef(this->GetRecord(), this);
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
		this->bWeak = old.bWeak;
		ObjectManager::AddRef(old->GetRecord(), this);
		//if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
	}

	RefWeak& operator=(const RefWeak& old) {
		this->bWeak = old.bWeak;
		ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
		return *this;
	}
};

namespace helpers{
	
	bool helper(BaseObject* base);

	template <typename T>
	BaseObject* invokeCreate(uint32 ID = 0, uint SpawnType = Constants::Record::SPAWNED, bool isServer = false, uint16 mod = 0) {
		if (ID != 0) ObjectManager::PrepareRecord(ID, SpawnType, isServer, mod);
		Ref<T> next = new T();
		//ObjectManager::CreateRecord(next, 0, Constants::Record::LOADED);
		BaseObject* base = dynamic_cast<BaseObject*>(next.GetPointer());
		if (!helper(base)) next->DestroyObject();
		return next;
	}
}

template <typename T>
bool do_register(String name) {
	ObjectManager::TypeList[name] = &helpers::invokeCreate<T>;
	return true;
}

#define REGISTER(CLASSNAME)				private: inline static bool Registered = do_register<CLASSNAME>(#CLASSNAME);
#define REGISTER_NAME(CLASSNAME, NAME)	private: inline static bool Registered = do_register<CLASSNAME>(#NAME);

//#define GET_MACRO(_1,_2,_3,NAME,...) NAME
//#define FOO(...) GET_MACRO(__VA_ARGS__, REGISTER_NAME, REGISTER)(__VA_ARGS__)

template <class T = BaseObject>
T* SpawnObject(uint32 ID = 0)
{
	if (ID != 0) ObjectManager::PrepareRecord(ID);
	Ref<T> next = new T();
	//ObjectManager::CreateRecord(next, 0, Constants::Record::SPAWNED);
	BaseObject* base = dynamic_cast<BaseObject*>(next.GetPointer());
	if (!helpers::helper(base)) next->DestroyObject();
	return next;
}

GameState* GetGameState();