#include "GameLoop.h"
#include "Objects/VisibleObject.h"
#include <fstream>
#include "GarbageCollector.h"
#include "ObjectManager.h"
#include <Gameplay/Scene.h>

GC::GC()
{
	bQuitting = false;
	Cleaner = std::thread(&GC::CleanRunner, this);
}

GC::~GC()
{

}

void GC::Quit()
{
	bQuitting = true;
	if (Cleaner.joinable()) Cleaner.join();
}

void GC::CleanRunner()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	while (!bQuitting) {
		auto start = std::chrono::steady_clock::now();
		auto time = std::chrono::milliseconds(5000) - duration;
		if (time.count() > 0) std::this_thread::sleep_for(time);
		if (Scene::isLoading()) continue;
		std::vector<RecordInt> removal;
		for (auto const& d : ObjectManager::ObjectRecords) {
			if (d.second->pointerRefs.size() < 1 && !d.second->protection) {
				if (d.second->checkCount > 3) {
					removal.push_back(d.first);
				}
				else {
					d.second->checkCount++;
				}
			}
			else {
				d.second->checkCount = 0;
			}
		}
		for (RecordInt d : removal) {
			ObjectManager::ThreadSafeDelete(d);
			Console::Log("Record " + d.ToString() + " is pending destroy");
		}

		//MI->MarkUnused(); // TODO

		duration = std::chrono::steady_clock::now() - start;
	}
}
