#include "InstancedObject.h"
#include "Terrain.h"
#include "Mesh.h"

InstancedObject::InstancedObject() : VisibleObject()
{

}

void InstancedObject::LoadWithParameters(const String& args)
{
	VisibleObject::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto ins = data.find("Instances");
	auto ran = data.find("Randomize");
	auto sca = data.find("InScale");
	auto rot = data.find("InRotation");
	auto ter = data.find("InTerrain");

	int randCount = 0;
	int instanceCount = 0;
	std::vector<String> it;
	Vector Scale = Vector(1, 1, 1);
	Vector Rotation = Vector(0, 0, 0);
	Terrain* Terra = nullptr;

	if (ran != data.end()) randCount = atoi(ran->second.c_str());
	if (sca != data.end()) Scale = Vector(sca->second);
	if (rot != data.end()) Rotation = Vector(rot->second);
	if (ter != data.end()) Terra = ObjectManager::GetByRecord<Terrain>(std::stoul(ter->second, nullptr, 16));
	if (ins != data.end()) {
		it = split(ins->second, '-');
		instanceCount = it.size() / 3;
	}
		Transformation* arr = new Transformation[randCount + instanceCount]();
		for (int i = 0; i < randCount; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;

			arr[i].Location = Vector(x, y, Terra == nullptr ? 0.f : Terra->GetHeight(x, y));
			arr[i].Scale = Scale;
			arr[i].Rotation = Rotation;
		}

		for (int i = 0; i < instanceCount ; i++)
		{
			arr[i + randCount].Location = Vector(it[i * 3]);
			arr[i + randCount].Scale = Vector(it[i * 3 + 2]);
			arr[i + randCount].Rotation = Vector(it[i * 3 + 1]);
		}

		AddInstances(randCount + instanceCount, arr);
		delete[] arr;
}


void InstancedObject::AddInstances(int count, Transformation* dispArray)
{
	if (RenderData != nullptr) RenderData->SetInstances(count, dispArray);
}
