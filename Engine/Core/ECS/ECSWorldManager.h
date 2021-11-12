#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/



class ECSWorldManager
{
public:
	ECSWorldManager();
	~ECSWorldManager();

	void Update(float deltaTime);

	

private:
	void SetupEngineSystems();

	
};

