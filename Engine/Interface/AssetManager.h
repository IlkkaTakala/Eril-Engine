#pragma once
#include <Core.h>

class Animation;

enum class AssetType : uint8
{
	None,
	MeshStatic,
	MeshSkeletal,
	Animation,
	Texture
};

typedef std::function<void(bool)> AssetLoadCallback;

namespace AssetManager
{
	void StartLoader();

	void LoadAssetAsync(const String& name);
	void LoadAssetAsyncWithCallback(const String& name, const AssetLoadCallback& callback);
	void LoadMeshAsync(const String& name, RenderMesh* empty);
	void LoadSkeletalMeshAsync(const String& name, RenderMesh* empty);
	void LoadTextureAsync(const String& name, Texture* empty);
	void LoadAnimationAsync(const String& name, Animation* empty);
	AssetType GetAssetType(const String& name);
	RenderMesh* LoadSkeletalMesh(const String& name);

	RenderMesh* LoadMeshAsyncWithPromise(const String& name);
	RenderMesh* LoadSkeletalMeshAsyncWithPromise(const String& name);
	Texture* LoadTextureAsyncWithPromise(const String& name);
	Animation* LoadAnimationAsyncWithPromise(const String& name, RenderMesh* skeletalMesh);
};

