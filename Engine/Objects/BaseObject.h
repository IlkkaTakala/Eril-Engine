#pragma once

class ObjectManager;
struct Record;

class RefHold
{
public:
	virtual const void NullThis() const = 0;
	virtual const long GetRecord() const = 0;
protected:
};

class Data
{
public:
	Data();
	void DestroyObject();
	virtual void OnDestroyed() = 0;
	long GetRecord() { return RecordNumber; }
	void SetRecord(long record) { RecordNumber = record; }
	void AddToRoot();
	void RemoveFromRoot();
protected:
	friend struct Record;
	virtual ~Data();
	bool bMarked;
private:
	friend class GC;
	friend class GameLoop;
	long RecordNumber;
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