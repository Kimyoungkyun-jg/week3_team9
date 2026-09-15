#pragma once
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FMatrix.h"

struct FSceneView
{
	const FCamera& Camera;
	FMatrix  ViewProj;

};