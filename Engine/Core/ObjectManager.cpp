#include "ObjectManager.h"
#include "GameLoop.h"
#include <Core.h>

long ObjectManager::counter = 1;
RecordInt ObjectManager::Prepared = 0;
std::map<RecordInt, Record*> ObjectManager::ObjectRecords;
std::map<String, BaseObject* (*)(const String&, uint32, uint, bool, uint16)> ObjectManager::TypeList;
std::list<RecordInt> ObjectManager::DeleteList;
std::mutex ObjectManager::delete_m;

constexpr uint32 MaxIndex = 0xffffffff;

Record::~Record()
{
	for (const auto& p : pointerRefs) {
		p->NullThis();
	}
	for (const auto& p : weakRefs) {
		p->NullThis();
	}
	auto t = dynamic_cast<Tickable*>(object);
	if (t != nullptr) Loop->RemoveFromTick(t);
	delete object;
}

void ObjectManager::DeleteRecord(RecordInt record)
{
	auto p = ObjectRecords.find(record);
	if (p != ObjectRecords.end()) {
		delete p->second;
		ObjectRecords.erase(p);
	}
}

void ObjectManager::CleanObjects()
{
	while (ObjectRecords.size() > 0)
	{
		DeleteRecord(ObjectRecords.begin()->first);
	}
}

void ObjectManager::AddTick(Tickable* t)
{
	if (t != nullptr) Loop->AddToTick(t);
}


void ObjectManager::AddRef(const RecordInt record, const RefHold* obj)
{
	if (obj->bWeak) ObjectRecords[record]->weakRefs.push_back(obj);
	else ObjectRecords[record]->pointerRefs.push_back(obj);
}

void ObjectManager::RemoveRef(const RecordInt record, const RefHold* obj)
{
	if (obj->bWeak) ObjectRecords.find(record)->second->weakRefs.remove(obj);
	else ObjectRecords.find(record)->second->pointerRefs.remove(obj);
}
#pragma optimize("", off)
void ObjectManager::CreateRecord(Data* object, short protection)
{
	if (Prepared == 0) {
		RecordInt lower(1, Constants::Record::SPAWNED, false);
		uint32 id = 0;
		if (ObjectRecords.find(lower) == ObjectRecords.end())
		{
			id = 1;
		}
		else {
			RecordInt upper(MaxIndex, Constants::Record::SPAWNED, false);
			auto high = ObjectRecords.upper_bound(upper);
			if (ObjectRecords.size() > 0) id = uint32((--high)->first + 1);
			else id = 1;
		}
		
		RecordInt r(id, Constants::Record::SPAWNED, false);
		ObjectRecords.emplace(r, new Record(object, protection));
		object->SetRecord(r);
	}
	else {
		ObjectRecords.emplace(Prepared, new Record(object, protection));
		object->SetRecord(Prepared);
		Prepared = 0;
	}
}
#pragma optimize("", on)

void ObjectManager::ThreadSafeDelete(const RecordInt& record)
{
	std::unique_lock<std::mutex> lock(delete_m);
	DeleteList.push_back(record);
}

void ObjectManager::DeleteListed()
{
	std::unique_lock<std::mutex> lock(delete_m);
	for (const auto& r : DeleteList) {
		DeleteRecord(r);
	}
	DeleteList.clear();
}

bool ObjectManager::PrepareRecord(const uint32 ID, const uint SpawnType, const bool isServer, const uint16 mod)
{
	RecordInt r(ID, SpawnType, isServer, mod);
	if (ObjectRecords.find(r) == ObjectRecords.end()) {
		Prepared = r;
		return true;
	}
	Prepared = 0;
	return false;
}
