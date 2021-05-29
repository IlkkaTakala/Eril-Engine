#include "GameLoop.h"
#include "Objects/VisibleObject.h"
#include <fstream>
#include "GarbageCollector.h"

std::map<String, Data*> GC::Pointers;

GC::GC()
{
	bQuitting = false;
	//Cleaner = std::thread(&GC::CleanRunner, this);
}

GC::~GC()
{
	if (Pointers.size() > 0) {
		for (auto const& i : Pointers) {
			delete i.second;
		}
	}
	
	Pointers.clear();
}

void GC::AddObject(BaseObject* obj)
{
	String name;
	do {
		name = "Object_"  + std::to_string(rand());
	} while (Pointers.find(name) != Pointers.end());
	Pointers.emplace(name, obj);
	Tickable* t = dynamic_cast<Tickable*>(obj);
	if (t != nullptr) Loop->AddToTick(t);
}

void GC::RemoveObject(BaseObject* obj)
{
	Tickable* t = dynamic_cast<Tickable*>(obj);
	if (t != nullptr) Loop->RemoveFromTick(t);
	obj->bMarked = true;
}

void GC::Quit()
{
	bQuitting = true;
	//if (Cleaner.joinable()) Cleaner.join();
}

void GC::CleanRunner()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	while (!bQuitting) {
		auto start = std::chrono::steady_clock::now();
		auto time = std::chrono::milliseconds(2500) - duration;
		if (time.count() > 0) std::this_thread::sleep_for(time);

		std::vector<String> removal;
		for (auto d : Pointers) {
			if (d.second->ReferenceCount < 1) {
				if (d.second->bMarked) {
					VisibleObject* v = dynamic_cast<VisibleObject*>(d.second);
					if (v != nullptr) {
						/*if (v->Model == nullptr) {
							delete d;
							removal.push_back(d);
						}*/
					}
					else {
						delete d.second;
						removal.push_back(d.first);
					}
					
				}
				else {
					d.second->DestroyObject();
				}
			}
			else {
				d.second->bMarked = false;
			}
		}
		for (String d : removal) {
			Pointers.erase(d);
		}

		std::vector<std::string> removed;
		/*for (auto d : LoadedMeshes) {
			if (d.second->ReferenceCount < 1) {
				delete d.second;
				removed.push_back(d.first);
			}
		}
		for (std::string s : removed) {
			LoadedMeshes.erase(s);
		}*/

		duration = std::chrono::steady_clock::now() - start;
	}
}
