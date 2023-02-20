#include "Core.h"
#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Settings.h"
#include "Renderer.h"
#include <Objects/SceneComponent.h>

GLCamera::GLCamera()
{
	Fov = 45.f;
	Perspective = true;
	Location = Vector(0.f, 0.f, 0.f);
	Rotation = Rotator(Vector{ 0.f, 0.f, 0.f });
	Orientation = glm::mat4(1.0f);
	postProcess = nullptr;
	Parent = nullptr;

	ApplyTransformation();

	SetPostProcess("Assets/Materials/PostProcess");

	int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
	int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());

	Projection = glm::perspectiveFov(glm::radians(Fov), (float)x, (float)y, 0.1f, 1000.f);
}

GLCamera::~GLCamera()
{
	if (IRender::GetActiveCamera() == this) IRender::SetActiveCamera(nullptr);
}

void GLCamera::SetFov(float fov)
{
	int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
	int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());
	Fov = fov;
	if (Perspective) Projection = glm::perspectiveFov(glm::radians(Fov), (float)x, (float)y, 0.1f, 100.f);
}

void GLCamera::SetPerspective(bool perspective)
{
	Perspective = perspective;
	int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
	int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());
	if (perspective) {
		Projection = glm::perspectiveFov(glm::radians(Fov), (float)x, (float)y, 0.1f, 100.f);
	}
	else
	{
		Projection = glm::ortho(0.f, (float)x, 0.f, (float)y, 0.1f, 100.f);
	}
}

void GLCamera::SetRotation(const Rotator& rotation)
{
	Rotation = rotation;
}

void GLCamera::SetLocation(const Vector& location)
{
	Location = location;
}

const Vector GLCamera::GetUpVector() const
{
	const glm::mat4 inverted = /*glm::inverse*/(View);
	return Vector(inverted[1][0], inverted[1][1], inverted[1][2]);
}

const Vector GLCamera::GetForwardVector() const
{
	const glm::mat4 inverted = /*glm::inverse*/(View);
	return -Vector(inverted[2][0], inverted[2][1], inverted[2][2]);
}

const Vector GLCamera::GetRightVector() const
{
	const glm::mat4 inverted = /*glm::inverse*/(View);
	return Vector(inverted[0][0], inverted[0][1], inverted[0][2]);
}

Rotator GLCamera::GetRotation() const
{
	return Parent->GetWorldRotation() * Rotation;
}

Vector GLCamera::GetLocation() const
{
	return Parent->GetWorldLocation() + Parent->GetWorldRotation() * Location;
}

void GLCamera::SetLookAt(const Vector& to, const Vector& up) {
	Orientation = glm::inverse(glm::lookAtRH(glm::vec3(Location.X, Location.Y, Location.Z), glm::vec3(to.X, to.Z, -to.Y), glm::vec3(up.X, up.Z, -up.Y)));
}

void GLCamera::SetPostProcess(const String& name)
{
	postProcess = IRender::LoadMaterialByName(name);
}

void GLCamera::ApplyTransformation()
{
	glm::mat4 finalT(1.f);
	SceneComponent* parent = Parent;
	while (parent)
	{
		Vector loc = parent->GetLocation();
		Rotator rot = parent->GetRotation();

		finalT = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
			* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
			* finalT;

		parent = parent->GetParent();
	}

	View = finalT * glm::translate(glm::mat4(1.0f), glm::vec3(Location.X, Location.Y, Location.Z))
		* glm::toMat4(glm::quat(Rotation.W, Rotation.X, Rotation.Y, Rotation.Z));
	View *= glm::mat4(glm::quat({ glm::half_pi<float>(), 0 , 0 }));
	if (glm::all(glm::isnan(View[0]))) {
		View = glm::mat4(1.f);
	}
}
