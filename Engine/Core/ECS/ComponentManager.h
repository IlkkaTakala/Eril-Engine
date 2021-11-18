#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 18.11.2021
*/

#include <Core.h>
#include <ECS/ComponentTypeStorage.h>

/// <summary>
/// Componentmanager holds all the different storage vectors for different types of components. 
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
	/// Creates a new storage for given type of components. Note that the given typeName is the identifier which can be used to
	/// get that type of components later.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="base"></param>
	/// <param name="typeName"></param>
	template <typename T>
	void CreateComponentTypeStorage(T &baseType, String typeName)
	{
		Console::Log("CreateComponentTypeStorage");
		TypeNames.insert(std::pair<String, int>(typeName, typeCount));
		componentStorages.push_back(new ComponentTypeStorage<T>(typeCount));
		typeCount++;
	}

	/// <summary>
	/// Return typeName as an integer.
	/// </summary>
	/// <param name="typeName"></param>
	/// <returns></returns>
	int GetTypeIdByName(String typeName);
	

protected:
	/// <summary>
	/// Adds a new component. Note that when adding a new component the system does not copy the component data.
	/// It only initializes its type and id and calls the component's Init()-method.
	/// </summary>
	/// <param name="typeName"></param>
	/// <returns>ID of the created component.</returns>
	int CreateNewComponentOfType(String typeName);

	/// <summary>
	/// Adds a new component. Note that when adding a new component the system does not copy the component data.
	/// It only initializes its type and id and calls the component's Init()-method.
	/// </summary>
	/// <param name="typeName"></param>
	/// <returns>ID of the created component.</returns>
	int CreateNewComponentOfType(int typeID);

	/// <summary>
	/// Returns the component of given id and type
	/// </summary>
	/// <param name="id"></param>
	/// <param name="typeName"></param>
	/// <returns></returns>
	Component* GetComponent(int id, String typeName);

	/// <summary>
	/// Returns the component of given id and type
	/// </summary>
	/// <param name="id"></param>
	/// <param name="typeID"></param>
	/// <returns></returns>
	Component* GetComponent(int id, int typeID);

	/// <summary>
	/// Removes a component with given ID and type. Note that because the components are stored in a continuous array, the component is only marked
	/// as disabled, and only overwritten after a new component is added.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="typeName"></param>
	/// <returns></returns>
	bool RemoveComponent(int id, String typeName);

	/// <summary>
	/// Removes a component with given ID and type. Note that because the components are stored in a continuous array, the component is only marked
	/// as disabled, and only overwritten after a new component is added.
	/// </summary>
	/// <param name="id"></param>
	/// <param name="typeID"></param>
	/// <returns></returns>
	bool RemoveComponent(int id, int typeID);

private:
	//Storage varables.
	std::vector<IComponentStorage*> componentStorages;
	std::map<String, int> TypeNames;
	int typeCount = 0;
};