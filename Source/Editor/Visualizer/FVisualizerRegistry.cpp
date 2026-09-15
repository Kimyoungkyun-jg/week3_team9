#include "FVisualizerRegistry.h"

#include "Editor/Visualizer/FPrimitiveVisualizer.h"
#include "Editor/Visualizer/FSpotlightVisualizer.h"
#include "Editor/Visualizer/FBillboardVisualizer.h"
#include "Editor/Visualizer/FTextVisualizer.h"

#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/CoreUObject/USpotLightComponent.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"
#include "Runtime/CoreUObject/UTextInstanceComponent.h"

FVisualizerRegistry::FVisualizerRegistry()
{
	Visualizers.push_back(MakeUnique<FPrimitiveVisualizer>());
	Map[UPrimitiveComponent::StaticClass()] = Visualizers.back().get();
	
	Visualizers.push_back(MakeUnique<FSpotlightVisualizer>());
	Map[USpotLightComponent::StaticClass()] = Visualizers.back().get();

	Visualizers.push_back(MakeUnique<FBillboardVisualizer>());
	Map[UBillBoardComp::StaticClass()] = Visualizers.back().get();

	Visualizers.push_back(MakeUnique<FTextVisualizer>());
	Map[UTextInstanceComponent::StaticClass()] = Visualizers.back().get();
}

IVisualizer* FVisualizerRegistry::FindVisualizer(UClass* ClassType)
{
	auto Item = Map.find(ClassType);

	if (Item == Map.end()) { return Visualizers[0].get(); }

	return Item->second;
}
