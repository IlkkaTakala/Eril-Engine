#pragma once
#include "Core.h"
#include <functional>

#define OPENGL

class Material;
class Texture;
class RenderObject;
class VisibleObject;
class LoadedMesh;
//struct LightData; //Lights have been moved to be handled by the ECS-system.
struct Vertex;
class UISpace;

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
	virtual void SetPostProcess(const String& name) = 0;

	virtual void SetFov(float) = 0;
	virtual void SetPerspective(bool perspective) = 0;
	void SetParent(VisibleObject* p) { Parent = p; }
protected:
	VisibleObject* Parent;
};

class IRender
{
public: 
	virtual ~IRender() {}
	virtual int SetupWindow(int width, int height) = 0;
	virtual void CleanRenderer() = 0;

	virtual Camera* CreateCamera(VisibleObject* parent = nullptr) = 0;
	virtual void SetActiveCamera(Camera*) = 0;
	
	//virtual void CreateLight(const LightData*) = 0; //Lights have been moved to be handled by the ECS-system.
	//virtual void RemoveLight(const LightData*) = 0; //Lights have been moved to be handled by the ECS-system.
	

	virtual void LoadShaders() = 0;
	virtual Material* GetMaterialByName(String name) const = 0;
	virtual Material* LoadMaterialByName(String name) = 0;
	virtual Texture* LoadTextureByName(String name) = 0;

	virtual void Update() = 0;
	virtual void Render(float delta) = 0;
	virtual void GameStart() = 0;
	virtual void DestroyWindow() = 0;
	virtual UISpace* GetUIManager(int screen = 0) const = 0;

};

class IInput
{
public:
	virtual void SetInputHandler(void(*Callback)(int, int, int, int) = 0) = 0;
	virtual void ProcessInputs(float delta) = 0;
	void SetTextMode(bool mode) { isText = mode; }
	void RegisterInputComponent(InputComponent* com) { ICs.push_back(com); }
	void UnregisterInputComponent(InputComponent* com) { ICs.remove(com); }
	void ClearInputs() { ICs.clear(); }

protected:
	std::list<InputComponent*> ICs;
	bool isText;
	float MouseX;
	float MouseY;
};


struct AABB
{
	AABB() {}
	AABB(float f) { mins = -f; maxs = f; }
	AABB(Vector min, Vector max) { mins = min; maxs = max; }
	Vector mins;
	Vector maxs;
};

class RenderMesh
{
public:
	virtual ~RenderMesh() {}
	virtual void ApplyTransform() = 0;
	virtual void SetMaterial(uint section, Material* nextMat) = 0;
	virtual Material* GetMaterial(uint section) const = 0;
	virtual void SetInstances(int count, Transformation* dispArray) = 0;
	AABB GetAABB() const { return bounds; }
	void SetAABB(AABB bounds) { this->bounds = bounds; }
protected:
	AABB bounds; // low level " collision " --> tänne collision
};

class IMesh
{
public:
	virtual ~IMesh() {}
	virtual RenderMesh* LoadData(VisibleObject* parent, String name) = 0;
	virtual RenderMesh* CreateProcedural(VisibleObject* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices) = 0;
	virtual void StartLoading() = 0;

protected:
	friend class GC;

	std::map<String, LoadedMesh*> LoadedMeshes;

};

extern IRender* RI;
extern IInput* II;
extern IMesh* MI;

int InterfaceInit();