#include "InstancedObject.h"

InstancedObject::InstancedObject() : VisibleObject()
{

}

void InstancedObject::LoadWithParameters(const String& args)
{
	VisibleObject::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto ins = data.find("Instances");


	if (ins != data.end()) {
		auto it = split(ins->second, '-');
		Transformation* arr = new Transformation[it.size() / 3]();
		for (int i = 0; i < it.size() / 3; i++)
		{
			arr[i].Location = Vector(it[i * 3]);
			arr[i].Scale = Vector(it[i * 3 + 2]);
			arr[i].Rotation = Vector(it[i * 3 + 1]);
		}
		AddInstances(it.size() / 3, arr);
		delete[] arr;
	}
}

void InstancedObject::AddInstances(int count, Transformation* dispArray)
{
	if (RenderData != nullptr) RenderData->SetInstances(count, dispArray);
}
