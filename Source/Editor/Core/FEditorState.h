#pragma once

#include "Runtime/Math/FVector.h"
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/FString.h"

class FConfigArchive;

/// <summary>
/// 에디터에서 필요한 상태들을 담는 구조체입니다.
/// 코드를 작성하는 과정에서 Editor에 뭔가 포인터가 아닌 값에 대한 멤버 변수가 필요하다면,
/// 그건 아마 여기에 들어가야 할겁니다.
/// </summary>
struct FEditorState
{
	// Camera
	float CameraSensitivity = 0.5f;
	float CameraSpeed = 1.0f;
	FVector CameraPosition = { 0.0f, 0.0f, 0.0f };
	float CameraYaw = 0.0f;
	float CameraPitch = 0.0f;

	// Gizmo
	uint8 GizmoMode = 0;
	uint8 GizmoSpace = 0;
	uint32 SelectedActor = -1; // Note: uint32이므로 -1은 언더플로우됨
};

namespace EditorUtil
{
	void WriteStateToFile(FEditorState& State, FStringView FilePath);
	void ReadStateFromFile(FEditorState& State, FStringView FilePath);
}