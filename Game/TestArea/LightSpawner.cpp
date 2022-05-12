#include "LightSpawner.h"
#include <Interface/IECS.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>

LightSpawner::LightSpawner()
{

}

void LightSpawner::LoadWithParameters(const String& args)
{
	auto data = ParseOptions(args);

	auto lig = data.find("Lights");
	if (lig != data.end()) {
		auto it = split(lig->second, '/');
		SystemsManager* systemsManager = IECS::GetSystemsManager();
		IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (systemsManager->GetSystemByName("LightControllerSystem"));

		for (int i = 0; i < it.size(); i++) {
			LightComponent* DirLight = lightSystem->AddComponentToSystem();
			auto dat = split(it[i], ',');
			if (dat.size() != 7) continue;
			DirLight->Location.X = std::stof(dat[0]);
			DirLight->Location.Y = std::stof(dat[1]);
			DirLight->Location.Z = std::stof(dat[2]);
			DirLight->Color.X = std::stof(dat[3]);
			DirLight->Color.X = std::stof(dat[4]);
			DirLight->Color.X = std::stof(dat[5]);
			DirLight->Intensity = std::stof(dat[6]);

			DirLight->LightType = LIGHT_POINT;
			DirLight->Size = 5.f;
		}
	}
}

