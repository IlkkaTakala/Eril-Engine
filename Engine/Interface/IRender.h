#pragma once
#include "Core.h"
#include <functional>
#include <Basic/Transformation.h>

#define OPENGL

class Material;
class Texture;
class RenderMeshStaticGL;
class SceneComponent;
class LoadedMesh;
//struct LightData; //Lights have been moved to be handled by the ECS-system.
struct Vertex;
class UISpace;
struct RenderCommand;
struct MeshCommand;
class RenderHandler;

class Camera
{
public:
	virtual ~Camera() {}
	virtual void SetRotation(const Rotator& rotation) = 0;
	virtual void SetLocation(const Vector& location) = 0;

	virtual const Vector GetUpVector() const = 0;
	virtual const Vector GetForwardVector() const = 0;
	virtual const Vector GetRightVector() const = 0;
	virtual const Rotator& GetRotation() const = 0;
	virtual const Vector& GetLocation() const = 0;

	virtual void SetLookAt(const Vector& to, const Vector& up = Vector(0.0f, 1.0f, 0.0f)) = 0;
	virtual void SetPostProcess(const String& name) = 0;

	virtual void SetFov(float) = 0;
	virtual void SetPerspective(bool perspective) = 0;
	void SetParent(SceneComponent* p) { Parent = p; }
protected:
	SceneComponent* Parent;
};

namespace IRender
{
	void SendCommand(RenderCommand c);

	void CleanRenderer();

	Camera* CreateCamera(SceneComponent* parent = nullptr);
	void SetActiveCamera(Camera*);
	Camera* GetActiveCamera();

	int SetupWindow(int width, int height);

	void LoadShaders();
	Material* GetMaterialByName(const String& name);
	Material* LoadMaterialByName(const String& name);
	Texture* LoadTextureByName(const String& name);

	void Update();
	void Render(float delta);
	void GameStart();
	void DestroyWindow();
	UISpace* GetUIManager(int screen = 0);

	void SetShowCursor(bool show, uint window = 0);
	bool GetShowCursor(uint window = 0);
};

class IInput
{
public:
	virtual void Update() = 0;
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

class RenderMeshStatic
{
public:
	virtual ~RenderMeshStatic() {}
	virtual void ApplyTransform() = 0;
	virtual void SetMaterial(uint section, Material* nextMat) = 0;
	virtual Material* GetMaterial(uint section) const = 0;
	virtual void SetInstances(int count, Transformation* dispArray) = 0;
	virtual void SetInstanceCount(int count) = 0;
	virtual SceneComponent* GetParent() const = 0;
	virtual void SetParent(SceneComponent* p) = 0;
	virtual void SetSectionRenderDistance(uint section, float distance) = 0;

	AABB GetAABB() const { return bounds; }
	virtual void SetAABB(AABB bounds) { this->bounds = bounds; }

	virtual void SetBinds(std::function<void(void)> bind) = 0;
	virtual std::function<void(void)>& GetBinds() = 0;
protected:
	AABB bounds; // low level " collision " --> tänne collision
};

class RenderMeshSkeletal
{
public:
	virtual ~RenderMeshSkeletal() {}
	virtual void ApplyTransform() = 0;
	virtual void SetMaterial(uint section, Material* nextMat) = 0;
	virtual Material* GetMaterial(uint section) const = 0;
	virtual SceneComponent* GetParent() const = 0;
	virtual void SetParent(SceneComponent* p) = 0;
	virtual void SetSectionRenderDistance(uint section, float distance) = 0;

	AABB GetAABB() const { return bounds; }
	virtual void SetAABB(AABB bounds) { this->bounds = bounds; }

	virtual void SetBinds(std::function<void(void)> bind) = 0;
	virtual std::function<void(void)>& GetBinds() = 0;
protected:
	AABB bounds; // low level " collision " --> tänne collision
};

class IMesh
{
public:
	virtual ~IMesh() {}
	virtual RenderMeshStatic* GetStatic(SceneComponent* parent, const String& name) = 0;
	virtual RenderMeshStatic* CreateProcedural(SceneComponent* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices) = 0;
	virtual RenderMeshStatic* MakeEmptyStatic() = 0;
};

extern IInput* II;
extern IMesh* MI;

int InterfaceInit();