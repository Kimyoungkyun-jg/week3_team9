#pragma once

#include "Runtime/CoreUObject/UClass.h"

#include "Runtime/Actors/AActor.h"
#include "Runtime/Actors/ACubeActor.h"
#include "Runtime/Actors/ASphereActor.h"
#include "Runtime/Actors/ACylinderActor.h"
#include "Runtime/Actors/ABillboardActor.h"
#include "Runtime/Actors/ASpotlightActor.h"
#include "Runtime/Actors/ATextRenderActor.h"

namespace EditorConstant
{

	/// <summary>
	/// 에디터에서 스폰 가능한 액터들을 정의합니다.
	/// </summary>
	inline UClass* const SpawnableActors[]
	{
	   ACubeActor::StaticClass(),
	   ASphereActor::StaticClass(),
	   ACylinderActor::StaticClass(),
	   ABillboardActor::StaticClass(),
	   ASpotlightActor::StaticClass(),
	   ATextRenderActor::StaticClass(),
	};

}