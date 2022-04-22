#pragma once
#include <BasicTypes.h>
#include <map>

class ObjectManager;
class Scene;
class InputComponent;
class ColliderComponent;
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
	//void operator delete(void* ptr);
	BaseObject();
	virtual void RegisterInputs(InputComponent* IC);
	virtual void SetColliders(ColliderComponent* CC);
	virtual void BeginPlay() = 0;
	virtual void OnDestroyed() {};
	Scene* GetScene() const { return World; }
	void SetScene(Scene* scene) { World = scene; }
	virtual void LoadWithParameters(const String& args) {}

protected:
	virtual ~BaseObject() {};

	static const std::map<String, String> ParseOptions(const String& args);
private:
	Scene* World;
};