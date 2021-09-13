#include "ObjectManager.h"

long ObjectManager::counter = 1;
std::map<long, Record*> ObjectManager::ObjectRecords;
