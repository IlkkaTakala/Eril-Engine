#pragma once
#include "Interface/IRender.h"
#include "glm/glm.hpp"

class RenderMeshStaticGL;
class Material;

class GLCamera : public Camera
{
public:
	GLCamera();
	virtual ~GLCamera();

	virtual void SetFov(float) override;
	virtual void SetPerspective(bool perspective) override;

	virtual void SetRotation(const Rotator& rotation) override;
	virtual void SetLocation(const Vector& location) override;
	virtual const Vector GetUpVector() const override;
	virtual const Vector GetForwardVector() const override;
	virtual const Vector GetRightVector() const override;
	virtual const Rotator& GetRotation() const override;
	virtual const Vector& GetLocation() const override;

	virtual void SetLookAt(const Vector& to, const Vector& up = Vector(0.0f, 0.0f, 1.0f)) override;

	const glm::mat4& GetProjectionMatrix() const { return Projection; }
	const glm::mat4& GetViewMatrix() const { return View; }

	virtual void SetPostProcess(const String& name) override;
	Material* GetPostProcess() const { return postProcess; }

	void ApplyTransformation();
private:
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Orientation;
	Vector Location;
	Rotator Rotation;
	Material* postProcess;


	float Fov;
	bool Perspective;
};