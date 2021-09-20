#include "BaseObject.h"
#include "../GarbageCollector.h"
#include "ObjectManager.h"

//void BaseObject::operator delete(void* ptr)
//{ 
//	//GC::RemoveObject(reinterpret_cast<BaseObject*>(ptr));
//	::delete ptr;
//}

void Data::DestroyObject()
{
	ObjectManager::DeleteRecord(RecordNumber);
	OnDestroyed();
	delete this;
}

Data::Data()
{
	RecordNumber = -1;
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
