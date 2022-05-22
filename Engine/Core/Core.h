#pragma once
#include "BasicTypes.h"
#include <Basic/Rotation.h>
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

	T* GetPointer() const { return Pointer; }
	
	Ref(const Ref& old) { 
		this->bWeak = old.bWeak;
		if (old != nullptr) ObjectManager::AddRef(old->GetRecord(), this);
		//if (this->DataPtr != nullptr && this->DataPtr == this->Pointer) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
	}

	Ref& operator=(const Ref& old) {
		this->bWeak = old.bWeak;
		if (old != nullptr) ObjectManager::AddRef(old->GetRecord(), this);
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
		if (old != nullptr) ObjectManager::AddRef(old->GetRecord(), this);
		//if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
	}

	RefWeak& operator=(const RefWeak& old) {
		this->bWeak = old.bWeak;
		if (old != nullptr) ObjectManager::AddRef(old->GetRecord(), this);
		if (this->DataPtr != nullptr) ObjectManager::RemoveRef(this->GetRecord(), this);
		this->Pointer = old.Pointer;
		this->DataPtr = old.DataPtr;
		return *this;
	}
};

namespace helpers{
	
	void SpawnHelper(BaseObject* obj, Scene* spawn, const String& args);

	template <typename T>
	BaseObject* invokeCreate(Scene* spawner, const String& args, uint32 ID, uint SpawnType, bool isServer, uint16 mod) {
		if (ID != 0) {
			ObjectManager::PrepareRecord(ID, SpawnType, isServer, mod);
		}
		Ref<T> next = new T();
		BaseObject* base = dynamic_cast<BaseObject*>(next.GetPointer());
		if (base == nullptr) {
			next->DestroyObject();
		}
		if (SpawnType != Constants::Record::LOADED) SpawnHelper(base, spawner, args);
		return next;
	}

}

template <typename T>
bool do_register(String name) {
	ObjectManager::TypeList().emplace(name, &helpers::invokeCreate<T>);
	return true;
}

#define REGISTER(CLASSNAME)				private: inline static bool Registered = do_register<CLASSNAME>(#CLASSNAME);
#define REGISTER_NAME(CLASSNAME, NAME)	private: inline static bool Registered = do_register<CLASSNAME>(#NAME);

//#define GET_MACRO(_1,_2,_3,NAME,...) NAME
//#define FOO(...) GET_MACRO(__VA_ARGS__, REGISTER_NAME, REGISTER)(__VA_ARGS__)

template <class T = BaseObject>
T* SpawnObjectWithID(BaseObject* spawner, String args, uint32 ID)
{
	if (ID != 0) ObjectManager::PrepareRecord(ID);
	Ref<T> next = new T();
	//ObjectManager::CreateRecord(next, 0, Constants::Record::SPAWNED);
	BaseObject* base = dynamic_cast<BaseObject*>(next.GetPointer());
	if (base == nullptr) {
		next->DestroyObject();
	}
	helpers::SpawnHelper(base, spawner ? spawner->GetScene() : nullptr, args);
	return next;
}

template <class T = BaseObject, typename... Args>
T* SpawnObject(BaseObject* spawner, Args&&... args)
{
	Ref<T> next = new T(std::forward<Args>(args)...);
	//ObjectManager::CreateRecord(next, 0, Constants::Record::SPAWNED);
	BaseObject* base = dynamic_cast<BaseObject*>(next.GetPointer());
	if (base == nullptr) {
		next->DestroyObject();
	}
	helpers::SpawnHelper(base, spawner ? spawner->GetScene() : nullptr, "");
	return next;
}

BaseObject* SpawnObject(String type);

GameState* GetGameState();

BaseObject* FindObjectByRecord(const RecordInt& record);