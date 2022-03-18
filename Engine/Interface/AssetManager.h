#pragma once
#include <Core.h>

enum class AssetType : uint8
{
	MeshStatic,
	Texture
};

namespace AssetManager
{
	void StartLoader();

	void LoadAssetAsync(const String& name);
	void LoadMeshAsync(const String& name, RenderMeshStatic* empty);
	void LoadTextureAsync(const String& name, Texture* empty);
	AssetType GetAssetType(const String& name);
	
	RenderMeshStatic* LoadMeshAsyncWithPromise(const String& name);
	Texture* LoadTextureAsyncWithPromise(const String& name);
};

