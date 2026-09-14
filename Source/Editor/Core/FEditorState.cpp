#include "FEditorState.h"
#include "Editor/Core/FConfigArchive.h"
#include "ThirdParty/mIni/ini.h"

void EditorUtil::WriteStateToFile(FEditorState& State, FStringView FilePath)
{
	FConfigArchive Archive;

	// Camera
	Archive.SetFloat("Camera", "Sensitivity", State.CameraSensitivity);
	Archive.SetFloat("Camera", "Speed", State.CameraSpeed);
	Archive.SetVector("Camera", "Position", State.CameraPosition);
	Archive.SetFloat("Camera", "Yaw", State.CameraYaw);
	Archive.SetFloat("Camera", "Pitch", State.CameraPitch);

	// Gizmo
	Archive.SetUInt32("Gizmo", "Mode", State.GizmoMode);
	Archive.SetUInt32("Gizmo", "Space", State.GizmoSpace);
	Archive.SetUInt32("Gizmo", "SelectedActor", State.SelectedActor);

	mINI::INIFile File{ FilePath };
	mINI::INIStructure Structure = Archive.GetConfig();

	File.write(Structure, true);
}

void EditorUtil::ReadStateFromFile(FEditorState& State, FStringView FilePath)
{
	mINI::INIFile File{ FilePath };
	mINI::INIStructure Structure;

	// 파일을 불러오는데 실패하면 기본값 유지
	if (!File.read(Structure))
	{
		UE_LOG("[EditorUtil::ReadStateFromFile] \"%s\" 파일을 불러오는데 실패했습니다.", FilePath);
		return;
	}

	const FConfigArchive Archive{ Structure };

	// Camera

	if (!Archive.IsEmpty("Camera", "Sensitivity"))
	{
		State.CameraSensitivity = Archive.GetFloat("Camera", "Sensitivity");
	}

	if (!Archive.IsEmpty("Camera", "Speed"))
	{
		State.CameraSpeed = Archive.GetFloat("Camera", "Speed");
	}

	if (!Archive.IsEmpty("Camera", "Position"))
	{
		State.CameraPosition = Archive.GetVector("Camera", "Position");
	}

	if (!Archive.IsEmpty("Camera", "Yaw"))
	{
		State.CameraYaw = Archive.GetFloat("Camera", "Yaw");
	}

	if (!Archive.IsEmpty("Camera", "Pitch"))
	{
		State.CameraPitch = Archive.GetFloat("Camera", "Pitch");
	}

	// Gizmo

	if (!Archive.IsEmpty("Gizmo", "Mode"))
	{
		State.GizmoMode = static_cast<uint8>(Archive.GetUInt32("Gizmo", "Mode"));
	}

	if (!Archive.IsEmpty("Gizmo", "Space"))
	{
		State.GizmoSpace = static_cast<uint8>(Archive.GetUInt32("Gizmo", "Space"));
	}

	if (!Archive.IsEmpty("Gizmo", "SelectedActor"))
	{
		State.SelectedActor = Archive.GetUInt32("Gizmo", "SelectedActor");
	}

}
