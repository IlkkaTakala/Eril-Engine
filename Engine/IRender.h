#pragma once
#include "Core.h"
#include <functional>

#define OPENGL

class Material;
class Texture;
class RenderObject;
class VisibleObject;
class LoadedMesh;
struct LightData;

class Camera
{
public:
	virtual void SetRotation(const Vector& rotation) = 0;
	virtual void SetLocation(const Vector& location) = 0;

	virtual const Vector GetUpVector() const = 0;
	virtual const Vector GetForwardVector() const = 0;
	virtual const Vector GetRightVector() const = 0;
	virtual const Vector& GetRotation() const = 0;
	virtual const Vector& GetLocation() const = 0;

	virtual void SetLookAt(const Vector& to, const Vector& up = Vector(0.0f, 1.0f, 0.0f)) = 0;

	virtual void SetFov(float) = 0;
	virtual void SetPerspective(bool perspective) = 0;
};

class IRender
{
public: 
	virtual ~IRender() {}
	virtual int SetupWindow(int width, int height) = 0;
	virtual void CleanRenderer() = 0;

	virtual Camera* CreateCamera(VisibleObject* parent = nullptr) = 0;
	virtual void SetActiveCamera(Camera*) = 0;
	virtual void CreateLight(const LightData*) = 0;
	virtual void RemoveLight(const LightData*) = 0;

	virtual void LoadShaders() = 0;
	virtual Material* GetMaterialByName(String name) const = 0;
	virtual Material* LoadMaterialByName(String name) = 0;
	virtual Texture* LoadTextureByName(String name) = 0;

	virtual void Update() = 0;
	virtual void Render(float delta) = 0;
	virtual void GameStart() = 0;
	virtual void DestroyWindow() = 0;
};

class IInput
{
public:
	virtual void SetInputHandler(void(*Callback)(int, int, int, int) = 0) = 0;
	virtual void ProcessInputs(float delta) = 0;

	template <class UserClass>
	void RegisterKeyInput(int Key, void (UserClass::* Callback)(float, bool), UserClass* Caller)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		std::function<void(float, bool)> f = std::bind(Callback, Caller, _1, _2);
		KeyCallers.insert(std::pair<int, std::function<void(float, bool)>>(Key, f));
	}

	template <class UserClass>
	void RegisterMouseInput(int Key, void (UserClass::* Callback)(float, float), UserClass* Caller)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		std::function<void(float, float)> f = std::bind(Callback, Caller, _1, _2);
		MouseCallers.insert(std::pair<int, std::function<void(float, float)>>(Key, f));
	}

protected:
	std::multimap<int, std::function<void(float, bool)>> KeyCallers;
	std::multimap<int, std::function<void(float, float)>> MouseCallers;

	double MouseX;
	double MouseY;
};

class RenderMesh
{
public:
	virtual ~RenderMesh() {}
	virtual void ApplyTransform() = 0;
	virtual void SetMaterial(uint section, Material* nextMat) = 0;
	virtual Material* GetMaterial(uint section) const = 0;
	virtual void SetInstances(int count, Transformation* dispArray) = 0;
	float GetAABB() const { return extent; }
protected:
	float extent;
};

class IMesh
{
public:
	virtual ~IMesh() {}
	virtual RenderMesh* LoadData(VisibleObject* parent, String name) = 0;
	virtual void StartLoading() = 0;

protected:
	friend class GC;

	std::map<String, LoadedMesh*> LoadedMeshes;

};

extern IRender* RI;
extern IInput* II;
extern IMesh* MI;

int InterfaceInit();