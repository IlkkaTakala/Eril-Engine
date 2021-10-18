#pragma once
#include <BasicTypes.h>

class ObjectManager;
struct Record;

class RefHold
{
public:
	RefHold() : bWeak(false) {}
	virtual const void NullThis() const = 0;
	virtual const RecordInt GetRecord() const = 0;
protected:
	friend class ObjectManager;
	bool bWeak;
};

class Data
{
public:
	Data();
	void DestroyObject();
	virtual void OnDestroyed() = 0;
	RecordInt GetRecord() { return RecordNumber; }
	void AddToRoot();
	void RemoveFromRoot();
protected:
	friend struct Record;
	virtual ~Data();
	bool bMarked;
private:
	friend class ObjectManager;
	friend class GC;
	friend class GameLoop;
	RecordInt RecordNumber;
	void SetRecord(RecordInt record) { RecordNumber = record; }
};

class Tickable
{
public:
	virtual void Tick(float Delta) = 0;
};

class BaseObject : public Data
{
public:
	BaseObject() : Data() { Parent = nullptr; }
	//void operator delete(void* ptr);

	virtual void BeginPlay() = 0;
	virtual void OnDestroyed() {};
	void SetParent(BaseObject* obj) { Parent = obj; }

protected:

	BaseObject* Parent;

	virtual ~BaseObject() {};

};