#include <Objects/VisibleObject.h>
#include <Objects/MovementComponent.h>
#include "Physics.h"

namespace Physics
{
	namespace
	{
		std::vector<RefWeak<VisibleObject>> Statics;
		std::vector<RefWeak<MovementComponent>> Movables;

		bool intersect(AABB a, AABB b) {
			return (a.mins.X <= b.maxs.X && a.maxs.X >= b.mins.X) &&
				(a.mins.Y <= b.maxs.Y && a.maxs.Y >= b.mins.Y) &&
				(a.mins.Z <= b.maxs.Z && a.maxs.Z >= b.mins.Z);
		}

	}

#pragma optimize("", off)
	void CheckCollisions()
	{
		//printf("Colliding objects...\n");
		//printf("Statics: %u, Movables: %u\n", Statics.size(), Movables.size());

		for (const auto& o : Movables) {
			if (o->GetTarget() == nullptr || o->GetTarget()->GetModel() == nullptr) continue;
			Vector d_loc = o->DesiredState.location;
			Vector o_loc = o->OldState.location;
			AABB d_AABB = o->GetTarget()->GetModel()->GetAABB();

			AABB final_o;
			final_o.mins = d_AABB.mins + o_loc;
			final_o.maxs = d_AABB.maxs + o_loc;

			AABB final_d;
			final_d.mins = d_AABB.mins + d_loc;
			final_d.maxs = d_AABB.maxs + d_loc;

			d_AABB.mins.X = final_o.mins.X < final_d.mins.X ? final_o.mins.X : final_d.mins.X;
			d_AABB.mins.Y = final_o.mins.Y < final_d.mins.Y ? final_o.mins.Y : final_d.mins.Y;
			d_AABB.mins.Z = final_o.mins.Z < final_d.mins.Z ? final_o.mins.Z : final_d.mins.Z;

			d_AABB.maxs.X = final_o.maxs.X > final_d.maxs.X ? final_o.maxs.X : final_d.maxs.X;
			d_AABB.maxs.Y = final_o.maxs.Y > final_d.maxs.Y ? final_o.maxs.Y : final_d.maxs.Y;
			d_AABB.maxs.Z = final_o.maxs.Z > final_d.maxs.Z ? final_o.maxs.Z : final_d.maxs.Z;

			std::vector<VisibleObject*> Collisions;

			for (const auto& s : Statics) {
				AABB bounds = s->GetModel()->GetAABB();
				Vector loc = s->GetLocation();

				bounds.mins += loc;
				bounds.maxs += loc;

				/*float d1x = bounds.mins.X - final_d.maxs.X;
				float d2x = final_d.mins.X - bounds.maxs.X;
				float d1y = bounds.mins.Y - final_d.maxs.Y;
				float d2y = final_d.mins.Y - bounds.maxs.Y;
				float d1z = bounds.mins.Z - final_d.maxs.Z;
				float d2z = final_d.mins.Z - bounds.maxs.Z;

				if (d1x <= 0.f) printf("Collision\n");
				if (d2x <= 0.f) printf("Collision\n");
				if (d1y <= 0.f) printf("Collision\n");
				if (d2y <= 0.f) printf("Collision\n");
				if (d1z <= 0.f) printf("Collision\n");
				if (d2z <= 0.f) printf("Collision\n");*/

				if (intersect(d_AABB, bounds)) {
					float d1x = bounds.mins.X - final_d.maxs.X;
					float d2x = final_d.mins.X - bounds.maxs.X;
					float d1y = bounds.mins.Y - final_d.maxs.Y;
					float d2y = final_d.mins.Y - bounds.maxs.Y;
					float d1z = bounds.mins.Z - final_d.maxs.Z;
					float d2z = final_d.mins.Z - bounds.maxs.Z;

					if (d1x >= 0.f) {
						o->DesiredState.location.X += d1x;
						o->DesiredState.velocity.X = 0.f;
					}
					if (d2x >= 0.f) {
						o->DesiredState.location.X += d2x;
						o->DesiredState.velocity.X = 0.f;
					}
					if (d1y >= 0.f) {
						o->DesiredState.location.Y += d1y;
						o->DesiredState.velocity.Y = 0.f;
					}
					if (d2y >= 0.f) {
						o->DesiredState.location.Y += d2y;
						o->DesiredState.velocity.Y = 0.f;
					}
					if (d1z >= 0.f) {
						o->DesiredState.location.Z += d1z;
						o->DesiredState.velocity.Z = 0.f;
					}
					if (d2z >= 0.f) {
						o->DesiredState.location.Z += d2z;
						o->DesiredState.velocity.Z = 0.f;
						o->DesiredState.gravity = 0.f;
					}
				}
			}
		}

		for (const auto& m : Movables) {
			m->ApplyMovement();
		}
	}
#pragma optimize("", on)

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
