#pragma once
#include <map>
#include <list>
#include "BasicTypes.h"
#include <Objects/BaseObject.h>

struct Record
{
	Record(Data* o) : object(o), protection(0), checkCount(0) {}
	Record(Data* o, short p) : object(o), protection(p), checkCount(0) {}
	~Record();
	std::list<const RefHold*> pointerRefs;
	std::list<const RefHold*> weakRefs;
	short protection;
	short checkCount;
	Data* object;
};

class ObjectManager
{
public:
	template <class T>
	static T* GetByRecord(RecordInt record) {
		return dynamic_cast<T>(ObjectRecords.find(record)->second.object);
	}

	static void AddRef(const uint64 record, const RefHold* obj) {
		if (obj->bWeak) ObjectRecords[record]->weakRefs.push_back(obj);
		else ObjectRecords[record]->pointerRefs.push_back(obj);
	}

	static void RemoveRef(const uint64 record, const RefHold* obj) {
		if (obj->bWeak) ObjectRecords.find(record)->second->weakRefs.remove(obj);
		else ObjectRecords.find(record)->second->pointerRefs.remove(obj);
	}

	static void CreateRecord(Data* object, short protection = 0) {
		ObjectRecords.emplace(counter, new Record(object, protection));
		object->SetRecord(counter);
		counter++;
	}

	static void Protect(RecordInt record) {
		ObjectRecords.find(record)->second->protection = 1;
	}

	static void Unprotect(RecordInt record) {
		ObjectRecords.find(record)->second->protection = 0;
	}

	static void DeleteRecord(RecordInt record) {
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
	static std::map<RecordInt, Record*> ObjectRecords;
};

