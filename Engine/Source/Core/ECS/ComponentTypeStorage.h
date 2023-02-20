#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <vector>
#include <ECS/Component.h>



/// <summary>
/// This is used in storing all the components in different vectors.
/// </summary>
class IComponentStorage
{
public:
	IComponentStorage(int type) : Type(type) {}

	int GetType() { return Type; }
	int GetComponentCount() { return ComponentCount; }
	virtual void Reset() = 0;

protected:
	int Type;
	int ComponentCount = 0;
};



/// <summary>
/// This is used to get to store one type of component. It has methods for adding, removing and getting a single componet.
/// </summary>
/// <typeparam name="T"></typeparam>
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

	/// <summary>
	/// Creates a new component to the vector.
	/// </summary>
	/// <returns>Pointer of the added component</returns>
	T* CreateComponent()
	{
		int usedIndex = -1;
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
			Components->emplace_back();
			IndexUsage->push_back(true);
		}
		else
		{
			Components->at(usedIndex) = T();
			IndexUsage->at(usedIndex) = true;
		}
		Components->at(usedIndex).SetID(usedIndex);
		Components->at(usedIndex).SetType(Type);
		ComponentCount++;

		//Console::Log("Added Component of type "+ std::to_string(Type) + " To ID: " + std::to_string(usedIndex));

		return &Components->at(usedIndex);
	}

	/// <summary>
	/// Gets a specific component based on given ID.
	/// </summary>
	/// <param name="id">ID of the Component</param>
	/// <returns>Component of given ID, nullptr if the component of that ID was not found or if the component was disabled.</returns>
	T* GetComponent(int id)
	{
		if (id < static_cast<int>(Components->size()))
		{
			if (IndexUsage->at(id))
			{
				if (!Components->at(id).GetDisabled())
				{
					return &Components->at(id);
				}
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Removes a component with given ID. Note that because the components are stored in a continuous array, the component is only marked
	/// as disabled, and only overwritten after a new component is added.
	/// </summary>
	/// <param name="id">ID of the component</param>
	/// <returns>Boolean based on if the component was removed successfully.</returns>
	bool RemoveComponent(int id)
	{
		if (id < static_cast<int>(Components->size()))
		{
			if (IndexUsage->at(id))
			{
				IndexUsage->at(id) = false;
				ComponentCount--;
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// Returns a pointer to the component container
	/// </summary>
	/// <returns>Component vector</returns>
	std::vector<T, std::allocator<T>>* GetComponentVector()
	{
		return Components;
	}

	void Reset()
	{
		Components->clear();
		IndexUsage->clear();
		ComponentCount = 0;
	}
	
private:
	std::vector<T, std::allocator<T>>* Components;
	std::vector<bool>* IndexUsage;
};


