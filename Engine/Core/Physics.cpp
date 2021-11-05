#include <Objects/VisibleObject.h>
#include <Objects/MovementComponent.h>
#include <limits>
#include "Physics.h"
#include <cassert>

namespace Physics
{
	namespace
	{
		std::list<RefWeak<VisibleObject>> Statics;
		std::list<RefWeak<MovementComponent>> Movables;
		// -------------------------------------------------------------------> alla oleva koodi on alkuperäistä
		/*bool intersect(AABB a, AABB b) {
			return (a.mins.X <= b.maxs.X && a.maxs.X >= b.mins.X) &&
				(a.mins.Y <= b.maxs.Y && a.maxs.Y >= b.mins.Y) &&
				(a.mins.Z <= b.maxs.Z && a.maxs.Z >= b.mins.Z);
		}*/

		float sign(float v) {
			return v < 0.f ? -1.0f : 1.0f;
		}

		struct Hit {
			AABB collider;
			bool collided;
			Vector pos;
			Vector delta;
			Vector normal;
			Vector time;
		};

		struct Sweep {
			Hit hit;
			Vector pos;
			float time;
		};
		/*
		Hit intersectSegment(const AABB& box, Vector pos, Vector delta, Vector padding)
		{
			Vector scale = Vector(1.0) / delta;
			Vector vsign = Vector(sign(scale.X), sign(scale.Y), sign(scale.Z));
			Vector nearTime = (box.position - vsign * (hwidth + padding) - pos) * scale;
			Vector farTime = (position + vsign * (hwidth + padding) - pos) * scale;
			if ((nearTime.X > farTime.Y) || (nearTime.X > farTime.Z) || (nearTime.Y > farTime.X) || (nearTime.Y > farTime.Z) || (nearTime.Z > farTime.X) || (nearTime.Z > farTime.Y))
				return Hit(false);

			float near = std::max(std::max(nearTime.X, nearTime.Y), nearTime.Z);
			float far = std::min(std::min(farTime.X, farTime.Y), farTime.Z);

			if ((near >= 1) || (far <= 0))
				return Hit(false);

			Hit hit;
			hit.time = near;//glm::clamp(near , 0.f, 1.f);
			if ((nearTime.X > nearTime.Y) && (nearTime.X > nearTime.Z))
			{
				hit.normal = Vector(-vsign.X, 0, 0);
			}
			else if ((nearTime.Y > nearTime.X) && (nearTime.Y > nearTime.Z))
			{
				hit.normal = Vector(0, -vsign.Y, 0);
			}
			else if ((nearTime.Z > nearTime.Y) && (nearTime.Z > nearTime.X))
			{
				hit.normal = Vector(0, 0, -vsign.Z);
			}

			hit.delta = hit.time * delta;

			hit.pos = pos + hit.delta;
			hit.time = nearTime;

			return hit;

		}
		Sweep sweepAABB(AABB first, AABB box, Vector delta)
		{
			if (delta == Vector(0, 0, 0))
			{
				Sweep s;
				s.hit.collided = false;
				s.time = 1;
				return s;
			}
			Sweep sweep;
			sweep.hit = intersectSegment(first, box.position, delta, box.hwidth);

			if (sweep.hit.collided)
			{
				sweep.time = glm::clamp(sweep.hit.time - EPSILON, 0.f, 1.f);
				sweep.pos = box.position + (delta * sweep.time);

				Vector direction = delta.Normalize();

				sweep.hit.pos += direction * box.hwidth;
				sweep.item = box.getSimple();
				sweep.other = getSimple();
			}
			else
			{
				sweep.pos = box.position + delta;
				sweep.time = 1;
			}
			return sweep;
		}*/
	}

#pragma optimize("", off)
	void CheckCollisions()
	{
		{
			auto it = Movables.remove_if([](const auto& v) { return v == nullptr; });
			auto it2 = Statics.remove_if([](const auto& v) { return v == nullptr; });
		}

		/*for (const auto& o : Movables) {
			if (o->GetTarget() == nullptr || o->GetTarget()->GetModel() == nullptr) continue;
			Vector& d_loc = o->DesiredState.location;
			const Vector& o_loc = o->OldState.location;
			const AABB& d_AABB = o->GetTarget()->GetModel()->GetAABB();



			std::vector<VisibleObject*> Collisions;
			for (const auto& s : Statics) {
				const AABB& bounds = s->GetModel()->GetAABB();
				const Vector& loc = s->GetLocation();
				
			}
		}*/

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
	// poitettu käytöstä 11.11 koitetaan toista tapaa ---> muista poistaa nämä kun saat collisionin toimimaan!
	//// testataan collison AABB ---> https://studiofreya.com/3d-math-and-physics/simple-aabb-vs-aabb-collision-detection/ ei toimi vielä
	//// TESTI PÄIVÄ 5.11 ->>>> tekijä: MARKUS

	//struct AABB
	//{
	//	AABB() : c(), r() {}

	//	AABB(const Point& center, const Point& halfwidths)
	//		: c(center)
	//		, r(halfwidths)
	//	{}

	//	Point c;        // center point
	//	Point r;        // halfwidths
	//};

	//struct Point
	//{
	//	Point() {}
	//	Point(double x, double y, double z)
	//		: x(x)
	//		, y(y)
	//		, z(z)
	//	{}
	//	double x = 0.0;
	//	double y = 0.0;
	//	double z = 0.0;
	//	double w = 0.0;

	//	const double operator[](const int idx) const
	//	{
	//		if (idx == 0) return x;
	//		if (idx == 1) return y;
	//		if (idx == 2) return z;
	//		if (idx == 3) return w;

	//		assert(0);
	//	}
	//};

	//double Abs(double a)
	//{
	//	return std::fabs(a);
	//}

	//bool testAABBAABB(const AABB& a, const AABB& b)
	//{
	//	if (Abs(a.c[0] - b.c[0]) > (a.r[0] + b.r[0])) return false;
	//	if (Abs(a.c[1] - b.c[1]) > (a.r[1] + b.r[1])) return false;
	//	if (Abs(a.c[2] - b.c[2]) > (a.r[2] + b.r[2])) return false;

	//	// We have an overlap
	//	return true;
	//};


	//bool testAABBAABB_SIMD(const AABB& a, const AABB& b)
	//{
	//	// SIMD optimized AABB-AABB test
	//	// Optimized by removing conditional branches
	//	bool x = Abs(a.c[0] - b.c[0]) <= (a.r[0] + b.r[0]);
	//	bool y = Abs(a.c[1] - b.c[1]) <= (a.r[1] + b.r[1]);
	//	bool z = Abs(a.c[2] - b.c[2]) <= (a.r[2] + b.r[2]);

	//	return x && y && z;
	//}


	};


