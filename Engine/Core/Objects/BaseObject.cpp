#include "BaseObject.h"
#include "GarbageCollector.h"
#include "ObjectManager.h"
#include <GameLoop.h>
#include <locale>

//void BaseObject::operator delete(void* ptr)
//{ 
//	//GC::RemoveObject(reinterpret_cast<BaseObject*>(ptr));
//	::delete ptr;
//}

void Data::DestroyObject()
{
	OnDestroyed();
	ObjectManager::DeleteRecord(RecordNumber);
}

Data::Data()
{
	RecordNumber = 0;
	bMarked = false;
	ObjectManager::CreateRecord(this);
}

Data::~Data()
{

}

void Data::AddToRoot()
{
	ObjectManager::Protect(RecordNumber);
}

void Data::RemoveFromRoot()
{
	ObjectManager::Unprotect(RecordNumber);
}

BaseObject::BaseObject()
{
	World = Loop->World;
}

void BaseObject::RegisterInputs(InputComponent* IC)
{

}

const std::map<String, String> BaseObject::ParseOptions(const String& args)
{
	std::map<String, String> options;
	String temp = args;
	temp.erase(std::remove_if(temp.begin(), temp.end(),
		[](char& c) {
			return std::isspace<char>(c, std::locale::classic());
		}), temp.end());
	auto out = split(temp, ';');

	for (const auto& s : out) {
		auto d = split(s, ':');
		if (d.size() == 2) {
			options.emplace(d[0], d[1]);
		}
	}

	return options;
}
