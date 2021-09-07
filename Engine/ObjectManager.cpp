#include "ObjectManager.h"

long ObjectManager::counter = 0;
std::map<long, Record*> ObjectManager::ObjectRecords;
