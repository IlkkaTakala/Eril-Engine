#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include <vector>
#include <map>
#include "Core.h"
#include "Component.h"

class iComponentStorage
{
public:
	iComponentStorage(int type) : Type(type) {}

	int GetType() { return Type; }
	int GetComponentCount() { return ComponentCount; }

	virtual void AddComponent() = 0;
	virtual Component* GetComponent(int id) = 0;
	virtual void RemoveComponent(int id) = 0;

protected:
	int Type;
	int ComponentCount = 0;
};


template <class T>
class ComponentTypeStorage : public iComponentStorage
{
	friend class ECSWorldManager;
	friend class EntityManager;
public:
	ComponentTypeStorage(int type) : iComponentStorage(type)
	{ 
		Components = new std::vector<T>;
		IndexUsage = new std::vector<bool>;
	}
	~ComponentTypeStorage() 
	{
		delete Components;
		delete IndexUsage;
	}

	void AddComponent()
	{
		int usedIndex = -1;
		T component;
		component.SetType(Type);

		for (int i = 0; i < IndexUsage->size(); i++)
		{
			if (!(IndexUsage->at(i)))
			{
				usedIndex = i;
				break;
			}
		}
		if (usedIndex == -1)
		{
			usedIndex = IndexUsage->size();
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
	}

	/*
	int AddComponent(T& component)
	{
		int usedIndex = -1;

		for (int i = 0; i < IndexUsage->size(); i++)
		{
			if (!(IndexUsage->at(i)))
			{
				usedIndex = i;
				break;
			}
		}
		if (usedIndex == -1)
		{
			usedIndex = IndexUsage->size();
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

		return usedIndex;
	}
	*/

	Component* GetComponent(int id)
	{
		if (id < Components->size())
		{
			return Components->at(id).GetPointer();
		}
		return nullptr;
	}

	void RemoveComponent(int id)
	{
		if (id < Components->size())
		{
			IndexUsage->at(id) = false;
			//return true;
		}
		//return false;
	}

private:
	std::vector<T>* Components;
	std::vector<bool>* IndexUsage;
};