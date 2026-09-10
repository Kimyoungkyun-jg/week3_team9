#include "USceneManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "ThirdParty/Json/json.hpp"
#include "Runtime/CoreUObject/FGarbageCollector.h"


void USceneManager::SaveScene(const FString& path) const
{
	std::filesystem::path fsPath(path);
	std::filesystem::path directory = fsPath.parent_path();

	if (!directory.empty() && !std::filesystem::exists(directory))
		std::filesystem::create_directories(directory);

	json::JSON sceneData = CurrentScene->Serialize();

	std::ofstream file(path);

	if (!file)
	{
		// TODO: Log error: failed to open file
		return;
	}

	file << sceneData.dump();
}

void USceneManager::LoadScene(const FString& path)
{
	std::ifstream file(path);
	if(!file)
	{

		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	json::JSON sceneData = json::JSON::Load(buffer.str());
	SetScene(NewObject<UScene>(*ResourceLibrary));
	CurrentScene->CreateFromJson(sceneData);

}

void USceneManager::SetScene(UScene* scene)
{
	if (CurrentScene == scene) return;

	UScene* OldScene = CurrentScene;

	FGarbageCollector& GarbageCollector = FGarbageCollector::Get();

	if (scene != nullptr) GarbageCollector.AddRoot(scene);

	CurrentScene = scene;

	if (OldScene != nullptr) GarbageCollector.RemoveRoot(OldScene);
}
