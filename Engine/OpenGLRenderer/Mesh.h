#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Material.h"

class RenderObject
{
public:
	void SetMaterial(uint section, Material* nextMat) { if (section < Sections.size()) Sections[section]->Instance = nextMat; }
	Material* GetMaterial(uint section) const { if (section < Sections.size()) return Sections[section]->Instance; }

private:
	std::vector<Section*> Sections;

};