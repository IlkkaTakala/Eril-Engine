#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021, Last Edited 12.11.2021
*/

#include <vector>
#include <ECS/Component.h>

class IComponentStorage
{
public:
	IComponentStorage(int type) : Type(type) {}

	int GetType() { return Type; }
	int GetComponentCount() { return ComponentCount; }

	virtual int AddComponent() = 0;
	virtual Component* GetComponent(int id) = 0;
	virtual bool RemoveComponent(int id) = 0;

protected:
	int Type;
	int ComponentCount = 0;
};

template <class T>
class ComponentTypeStorage : public IComponentStorage
{
public:

	ComponentTypeStorage(int type) : IComponentStorage(type)
	{ 
		Components = new std::vector<T>;
		IndexUsage = new std::vector<bool>;
	}

	~ComponentTypeStorage() 
	{
		delete Components;
		delete IndexUsage;
	}

	int AddComponent()
	{
		int usedIndex = -1;
		T component;
		component.SetType(Type);

		for (int i = 0; i < static_cast<int>(IndexUsage->size()); i++)
		{
			if (!(IndexUsage->at(i)))
			{
				usedIndex = i;
				break;
			}
		}
		if (usedIndex == -1)
		{
			usedIndex = static_cast<int>(IndexUsage->size());
			component.SetID(usedIndex);
			Components->push_back(component);
			Components->at(usedIndex).Init();
			IndexUsage->push_back(true);
		}
		else
		{
			component.SetID(usedIndex);
			Components->at(usedIndex) = component;
			Components->at(usedIndex).Init();
			IndexUsage->at(usedIndex) = true;
		}
		ComponentCount++;
		return usedIndex;
	}

	Component* GetComponent(int id)
	{
		if (id < static_cast<int>(Components->size()))
		{
			return Components->at(id).GetPointer();
		}
		return nullptr;
	}

	bool RemoveComponent(int id)
	{
		if (id < static_cast<int>(Components->size()))
		{
			IndexUsage->at(id) = false;
			return true;
		}
		return false;
	}

private:
	std::vector<T>* Components;
	std::vector<bool>* IndexUsage;
};