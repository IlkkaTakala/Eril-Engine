#pragma once
#include <Core.h>

enum class AssetType : uint8
{
	None,
	MeshStatic,
	MeshSkeletal,
	Animation,
	Texture
};

namespace AssetManager
{
	void StartLoader();

	void LoadAssetAsync(const String& name);
	void LoadMeshAsync(const String& name, RenderMesh* empty);
	void LoadSkeletalMeshAsync(const String& name, RenderMesh* empty);
	void LoadTextureAsync(const String& name, Texture* empty);
	AssetType GetAssetType(const String& name);
	
	RenderMesh* LoadMeshAsyncWithPromise(const String& name);
	RenderMesh* LoadSkeletalMeshAsyncWithPromise(const String& name);
	Texture* LoadTextureAsyncWithPromise(const String& name);
};

