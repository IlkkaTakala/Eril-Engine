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

typedef std::function<void(const String&, bool)> AssetLoadCallback;

namespace AssetManager
{
	void StartLoader();

	void LoadAssetAsync(const String& name, const AssetLoadCallback& callback = nullptr);
	bool LoadAsset(const String& name);
	void LoadMeshAsync(const String& name, RenderMesh* empty, const AssetLoadCallback& callback = nullptr);
	void LoadSkeletalMeshAsync(const String& name, RenderMesh* empty, const AssetLoadCallback& callback = nullptr);
	void LoadTextureAsync(const String& name, Texture* empty, const AssetLoadCallback& callback = nullptr);
	void LoadAnimationAsync(const String& name, Animation* empty, const AssetLoadCallback& callback = nullptr);
	AssetType GetAssetType(const String& name);
	RenderMesh* LoadSkeletalMesh(const String& name);

	RenderMesh* LoadMeshAsyncWithPromise(const String& name);
	RenderMesh* LoadSkeletalMeshAsyncWithPromise(const String& name);
	Texture* LoadTextureAsyncWithPromise(const String& name);
	Animation* LoadAnimationAsyncWithPromise(const String& name, RenderMesh* skeletalMesh);
};

