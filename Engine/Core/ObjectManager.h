#pragma once
#include <map>
#include <list>
#include "BasicTypes.h"
#include <Objects/BaseObject.h>
#include <mutex>

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

	static void AddRef(const RecordInt record, const RefHold* obj);

	static void RemoveRef(const RecordInt record, const RefHold* obj);

	static void CreateRecord(Data* object, short protection = 0, uint SpawnType = Constants::Record::SPAWNED, bool isServer = false) {
		uint64 r = 0;
		if (isServer) r |= 1ULL << 51;
		r += (uint64)SpawnType << 48;
		r += counter;
		ObjectRecords.emplace(r, new Record(object, protection));
		object->SetRecord(r);
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

	static void ThreadSafeDelete(const RecordInt& record);

	static void DeleteListed();

	static std::map<String, BaseObject* (*)()> TypeList;
private:
	friend class GC;
	static long counter;
	static std::map<RecordInt, Record*> ObjectRecords;
	static std::list<RecordInt> DeleteList;
	static std::mutex delete_m;
};

