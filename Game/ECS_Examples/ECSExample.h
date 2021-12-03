#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <Core.h>
#include <Objects/Actor.h>


class ECSExample : public Actor
{
public:
	ECSExample();
	virtual void Tick(float) override;
private:

};


