#pragma once
#include <map>
#include <vector>
#include <Objects/BaseObject.h>

struct Record
{
	Record(void* o) : object(o), protection(0) {}
	Record(void* o, short p) : object(o), protection(p) {}
	~Record() {
		for (const auto& p : pointerRefs) {
			*p = nullptr;
		}
	}
	std::vector<void**> pointerRefs;
	short protection;
	void* object;
};

class ObjectManager
{
	static std::map<long, Record> ObjectRecords;
	
	template <class T>
	static T* GetByRecord(long record) {
		return dynamic_cast<T>(ObjectRecords.find(record)->second.object);
	}

	template <class T>
	static void AddRef(T** obj) {
		ObjectRecords[dynamic_cast<Data*>(*obj)->GetRecord()].pointerRefs.push_back(obj);
	}

	void CreateRecord(void* object, short protection = 0) {
		ObjectRecords.emplace(counter++, new Record(object, protection));
	}

private:
	static long counter;
};

