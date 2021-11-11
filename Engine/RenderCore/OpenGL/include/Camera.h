#pragma once
#include "Interface/IRender.h"
#include "glm/glm.hpp"

class RenderObject;

class GLCamera : public Camera
{
public:
	GLCamera(RenderObject*);
	virtual ~GLCamera() {};

	virtual void SetFov(float) override;
	virtual void SetPerspective(bool perspective) override;

	virtual void SetRotation(const Vector& rotation) override;
	virtual void SetLocation(const Vector& location) override;
	virtual const Vector GetUpVector() const override;
	virtual const Vector GetForwardVector() const override;
	virtual const Vector GetRightVector() const override;
	virtual const Vector& GetRotation() const override;
	virtual const Vector& GetLocation() const override;

	virtual void SetLookAt(const Vector& to, const Vector& up = Vector(0.0f, 0.0f, 1.0f)) override;

	const glm::mat4& GetProjectionMatrix() const { return Projection; }
	const glm::mat4& GetViewMatrix() const { return View; }

private:
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Orientation;
	Vector Location;
	Vector Rotation;

	inline void ApplyTransformation();

	float Fov;
	bool Perspective;

	RenderObject* Parent;
};