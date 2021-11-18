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

	static void CreateRecord(Data* object, short protection = 0);

	static void Protect(RecordInt record) {
		ObjectRecords.find(record)->second->protection = 1;
	}

	static void Unprotect(RecordInt record) {
		ObjectRecords.find(record)->second->protection = 0;
	}

	static void DeleteRecord(RecordInt record);

	static void CleanObjects();

	static void AddTick(Tickable*);

	static void ThreadSafeDelete(const RecordInt& record);

	static void DeleteListed();

	static bool PrepareRecord(const uint32 ID, const uint SpawnType = Constants::Record::SPAWNED, const bool isServer = false, const uint16 mod = 0);

	static std::map<String, BaseObject* (*)(const String&, uint32, uint, bool, uint16)> TypeList;
private:
	friend class GC;
	static long counter;
	static RecordInt Prepared;
	static std::map<RecordInt, Record*> ObjectRecords;
	static std::list<RecordInt> DeleteList;
	static std::mutex delete_m;
};

