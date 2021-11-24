#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <Core.h>
#include <Objects/Actor.h>


class ECSTesting : public Actor
{
public:
	ECSTesting();
	virtual void Tick(float) override;
private:

};


