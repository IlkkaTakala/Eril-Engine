#include "ObjectManager.h"

long ObjectManager::counter = 1;
std::map<RecordInt, Record*> ObjectManager::ObjectRecords;
