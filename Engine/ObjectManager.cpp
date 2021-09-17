#include "ObjectManager.h"
#include "GameLoop.h"

long ObjectManager::counter = 1;
std::map<RecordInt, Record*> ObjectManager::ObjectRecords;

Record::~Record()
{
	for (const auto& p : pointerRefs) {
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
