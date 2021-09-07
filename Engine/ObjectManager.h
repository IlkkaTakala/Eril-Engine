#pragma once
#include <map>
#include <list>
#include <Objects/BaseObject.h>

struct Record
{
	Record(Data* o) : object(o), protection(0) {}
	Record(Data* o, short p) : object(o), protection(p) {}
	~Record() {
		for (const auto& p : pointerRefs) {
			*p = nullptr;
		}
		object->DestroyObject();
	}
	std::list<void**> pointerRefs;
	short protection;
	Data* object;
};

class ObjectManager
{
public:
	template <class T>
	static T* GetByRecord(long record) {
		return dynamic_cast<T>(ObjectRecords.find(record)->second.object);
	}

	template <class T>
	static void AddRef(T** obj) {
		ObjectRecords[dynamic_cast<Data*>(*obj)->GetRecord()]->pointerRefs.push_back(reinterpret_cast<void**>(obj));
	}

	template <class T>
	static void RemoveRef(T** obj) {
		ObjectRecords.find(dynamic_cast<Data*>(*obj)->GetRecord())->second->pointerRefs.remove(reinterpret_cast<void**>(obj));
	}

	static void CreateRecord(Data* object, short protection = 0) {
		ObjectRecords.emplace(counter, new Record(object, protection));
		dynamic_cast<Data*>(object)->SetRecord(counter);
		counter++;
	}

private:
	static long counter;
	static std::map<long, Record*> ObjectRecords;
};

