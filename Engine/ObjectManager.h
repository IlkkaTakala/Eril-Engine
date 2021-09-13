#pragma once
#include <map>
#include <list>
#include <Objects/BaseObject.h>
#include <memory>

struct Record
{
	Record(Data* o) : object(o), protection(0), checkCount(0) {}
	Record(Data* o, short p) : object(o), protection(p), checkCount(0) {}
	~Record();
	std::list<const RefHold*> pointerRefs;
	short protection;
	short checkCount;
	Data* object;
};

class ObjectManager
{
public:
	template <class T>
	static T* GetByRecord(long record) {
		return dynamic_cast<T>(ObjectRecords.find(record)->second.object);
	}

	static void AddRef(const RefHold* obj) {
		ObjectRecords[obj->GetRecord()]->pointerRefs.push_back(obj);
	}

	static void RemoveRef(const RefHold* obj) {
		ObjectRecords.find(obj->GetRecord())->second->pointerRefs.remove(obj);
	}

	static void CreateRecord(Data* object, short protection = 0) {
		ObjectRecords.emplace(counter, new Record(object, protection));
		object->SetRecord(counter);
		counter++;
	}

	static void Protect(long record) {
		ObjectRecords.find(record)->second->protection = 1;
	}

	static void Unprotect(long record) {
		ObjectRecords.find(record)->second->protection = 0;
	}

	static void DeleteRecord(long record) {
		auto p = ObjectRecords.find(record);
		if (p != ObjectRecords.end()) {
			delete p->second;
			ObjectRecords.erase(p);
		}
	}

	static void CleanObjects() {
		while (ObjectRecords.size() > 0)
		{
			DeleteRecord(ObjectRecords.begin()->first);
		}
	}

	static void AddTick(Tickable*);

private:
	friend class GC;
	static long counter;
	static std::map<long, Record*> ObjectRecords;
};

