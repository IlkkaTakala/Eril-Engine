#include "Core.h"
#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Settings.h"
#include "Renderer.h"

GLCamera::GLCamera()
{
	Fov = 45.f;
	Perspective = true;
	Location = Vector(0.f, 0.f, 0.f);
	Rotation = Rotator(0.f, 0.f, 0.f, 0.f);
	Orientation = glm::mat4(1.0f);

	ApplyTransformation();

	SetPostProcess("PostProcessMaster");

	int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
	int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());

	Projection = glm::perspectiveFov(glm::radians(Fov), (float)x, (float)y, 0.1f, 1000.f);
}

GLCamera::~GLCamera()
{
	if (RI->GetActiveCamera() == this) RI->SetActiveCamera(nullptr);
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
	
	ApplyTransformation();
}

void GLCamera::SetLocation(const Vector& location)
{
	Location = location;

	ApplyTransformation();
}

const Vector GLCamera::GetUpVector() const
{
	const glm::mat4 inverted = glm::inverse(View);
	return Vector(inverted[0][1], inverted[2][1], inverted[1][1]);
}

const Vector GLCamera::GetForwardVector() const
{
	const glm::mat4 inverted = glm::inverse(View);
	return Vector(inverted[0][2], inverted[2][2], inverted[1][2]);
}

const Vector GLCamera::GetRightVector() const
{
	const glm::mat4 inverted = glm::inverse(View);
	return Vector(inverted[0][0], inverted[2][0], inverted[1][0]);
}

const Rotator& GLCamera::GetRotation() const
{
	return Rotation;
}

const Vector& GLCamera::GetLocation() const
{
	return Location;
}

void GLCamera::SetLookAt(const Vector& to, const Vector& up) {
	Orientation = glm::inverse(glm::lookAtRH(glm::vec3(Location.X, Location.Z, Location.Y), glm::vec3(to.X, to.Z, to.Y), glm::vec3(up.X, up.Z, up.Y)));
}

void GLCamera::SetPostProcess(const String& name)
{
	postProcess = dynamic_cast<Renderer*>(RI)->Shaders.at(name);
}

void GLCamera::ApplyTransformation()
{
	View = glm::translate(glm::mat4(1.0f), glm::vec3(Location.X, Location.Z, Location.Y))
		* glm::mat4(glm::quat(Rotation.W, Rotation.X, Rotation.Y, Rotation.Z));
	if (glm::all(glm::isnan(View[0]))) {
		View = glm::mat4(1.f);
	}
}
