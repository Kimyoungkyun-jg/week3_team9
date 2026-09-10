#pragma once
#include "UScene.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
class USceneManager final
{

public:
	void SaveScene(const FString& path) const;
	void LoadScene(const FString& path);
	void SetScene(UScene* scene);


	UScene* CurrentScene = nullptr;
	FRenderResourceLibrary* ResourceLibrary = nullptr;
};