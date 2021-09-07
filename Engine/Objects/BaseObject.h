#pragma once

class Data
{
public:
	Data() { RecordNumber = 0; bMarked = false; }
	virtual void DestroyObject() = 0;
	long GetRecord() { return RecordNumber; }
	void SetRecord(long record) { RecordNumber = record; }
protected:
	 virtual ~Data() {}
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
	virtual void DestroyObject() override;
	void SetParent(BaseObject* obj) { Parent = obj; }

protected:

	BaseObject* Parent;

	virtual ~BaseObject() {};

};