#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <Core.h>
#include <ECS/ComponentTypeStorage.h>

/// <summary>
/// ComponentManager holds all the different storage vectors for different types of components. 
/// It has template functions for adding new component types to the system, adding and removing of components.
/// </summary>
class ComponentManager
{
	//Allow other ECS system classes to access protected and private methods.
	friend class EntityManager;
	friend class SystemsManager;

public:
	ComponentManager() {};
	~ComponentManager() {};


	/// <summary>
	/// Return typeName as an integer.
	/// </summary>
	/// <param name="typeName">Container's TypeName</param>
	/// <returns>Container's type as int</returns>
	int GetTypeIdByName(String typeName)
	{
		std::map<String, int>::iterator it = TypeNames.find(typeName);
		if (it != TypeNames.end())
		{
			return it->second;
		}
		return -1;
	}

	/// <summary>
	/// Creates a new storage for given type of components.
	/// Note that the given typeName is the identifier which can be used to get that type of components later.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="String used to find this container"></param>
	template <typename T>
	void CreateComponentTypeStorage(String typeName)
	{
		TypeNames.insert(std::pair<String, int>(typeName, typeCount));
		componentStorages.push_back(new ComponentTypeStorage<T>(typeCount));
		typeCount++;
	}

	template <typename T>
	std::vector<T>* GetComponentVector(int typeID)
	{
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				return s->GetComponentVector();
			}
		}
		return nullptr;
	}

protected:

	/// <summary>
	/// Adds a new
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>Pointer to created component</returns>
	template <typename T>
	T* CreateComponent(int typeID)
	{
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				//component.Type = componentStorages.at(i)->GetType();
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				return s->CreateComponent();
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Gets a component of given type and ID. Returns nullptr if Component was not found.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="id">ID of the Component</param>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>Pointer to the Component if it exists</returns>
	template <typename T>
	T* GetComponent(int id, int typeID)
	{
		//Console::Log("GetComponent, ID,Type: " + std::to_string(id) + "," + std::to_string(id));
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				T* cT = s->GetComponent(id);
				//Console::Log("Found Component with ID: " + std::to_string(cT->GetID()));
				return cT;
			}
		}
		return nullptr;
	}
	 
	/// <summary>
	/// Removes a component with given ID and type. Note that because the components are stored in a continuous array, the component is only marked
	/// as disabled, and only overwritten after a new component is added.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="id">ID of the Component</param>
	/// <param name="typeID">Container's type as int</param>
	/// <returns>True if removing was successful.</returns>
	template <typename T>
	bool RemoveComponent(int id, int typeID)
	{
		for (int i = 0; i < componentStorages.size(); i++)
		{
			if (componentStorages.at(i)->GetType() == typeID)
			{
				ComponentTypeStorage<T>* s = static_cast<ComponentTypeStorage<T>*>(componentStorages.at(i));
				return s->RemoveComponent(id);
			}
		}
		return false;
	}

	

private:
	//Storage varables.
	std::vector<IComponentStorage*> componentStorages;
	std::map<String, int> TypeNames;
	int typeCount = 0;
};