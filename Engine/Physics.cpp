#include <Objects/VisibleObject.h>
#include <Objects/MovementComponent.h>
#include "Physics.h"

namespace Physics
{
	namespace
	{
		std::vector<RefWeak<VisibleObject>> Statics;
		std::vector<RefWeak<MovementComponent>> Movables;
	}

	void CheckCollisions()
	{
		//printf("Colliding objects...\n");
		//printf("Statics: %u, Movables: %u\n", Statics.size(), Movables.size());

		for (const auto& o : Movables) {
			o->DesiredState;
			for (const auto& s : Statics) {
				float rad = s->GetModel()->GetAABB();
				Vector loc = s->GetLocation();
			}
		}
	}

	void AddStatic(VisibleObject* obj)
	{
		Statics.push_back(obj);
	}

	void RemoveStatic(VisibleObject* obj)
	{
		for (auto i = Statics.begin(); i != Statics.end(); i++) {
			if (i->GetPointer() == obj) { Statics.erase(i); break; }
		}
	}

	void AddMovable(MovementComponent* obj)
	{
		Movables.push_back(obj);
	}

	void RemoveMovable(MovementComponent* obj)
	{
		for (auto i = Movables.begin(); i != Movables.end(); i++) {
			if (i->GetPointer() == obj) Movables.erase(i);
		}
	}
}
