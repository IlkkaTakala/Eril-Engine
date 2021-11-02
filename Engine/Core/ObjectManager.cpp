#include "ObjectManager.h"
#include "GameLoop.h"
#include <Core.h>

long ObjectManager::counter = 1;
std::map<RecordInt, Record*> ObjectManager::ObjectRecords;
std::list<RecordInt> ObjectManager::DeleteList;
std::mutex ObjectManager::delete_m;

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