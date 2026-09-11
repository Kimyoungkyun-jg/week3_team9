#include "FImguiPropertyWindow.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/Actors/AActor.h"
#include "ThirdParty/Imgui/imgui.h"
#include "ThirdParty/Imgui/imgui_internal.h"
#include "ThirdParty/Imgui/imgui_impl_dx11.h"
#include "ThirdParty/Imgui/imgui_impl_win32.h"

void FImguiPropertyWindow::Process(FEditor& Editor)
{
	USceneComponent* SceneComponent = nullptr;
	AActor* SelectedActor = Editor.GetSelectedActor();
	if (SelectedActor)
	{
		SceneComponent = SelectedActor->GetRootComponent();
	}
	ImGui::Begin("Jungle Property Window");

	if (SelectedActor)
	{
		// 액터 정보 출력
		const char* ActorClassName = SelectedActor->GetClass() ? SelectedActor->GetClass()->GetDisplayName().c_str() : "None";
		ImGui::Text("Actor Class: %s", ActorClassName);
		ImGui::Text("Actor UUID: %u", SelectedActor->GetUUID());

		if (SceneComponent)
		{
			// 컴포넌트 정보 출력
			const char* CompClassName = SceneComponent->GetClass() ? SceneComponent->GetClass()->GetDisplayName().c_str() : "None";
			ImGui::Text("Root Component: %s", CompClassName);
			ImGui::Text("Component UUID: %u", SceneComponent->GetUUID());
		}

		ImGui::Separator();

		if (SceneComponent)
		{
			ImGui::DragFloat3("Translation", &Editor.SelectedTransform.Location.X, 0.01f);
			if (ImGui::DragFloat3("Rotation (deg)", &Editor.SelectedEulerDegDisplay.X, 0.5f))
			{
				Editor.SelectedTransform.Rotation = FQuaternion::FromEulerXYZDeg(Editor.SelectedEulerDegDisplay);
			}
			ImGui::DragFloat3("Scale", &Editor.SelectedTransform.Scale3D.X, 0.01f);
		}
	}
	else
	{
		ImGui::TextDisabled("No selection");
	}

	static const char* GizmoModes[4] = { "None", "Translation", "Rotation", "Scale" };
	int SelectedItem = static_cast<int>(Editor.GetGizmo().Mode);
	if (ImGui::Combo("Gizmo Mode", &SelectedItem, GizmoModes, 4))
	{
		Editor.GetGizmo().Mode = static_cast<EGizmoMode>(SelectedItem);
	}
	
	if (SelectedItem == 3) // Scale
	{
		static const char* GizmoSpaces[] = { "Local" };
		SelectedItem = static_cast<int>(Editor.GetGizmo().GetSpace()) - 1;
		if (ImGui::Combo("Gizmo Space", &SelectedItem, GizmoSpaces, 1))
		{
			Editor.GetGizmo().SetGizmoSpace(static_cast<EGizmoSpace>(SelectedItem - 1));
		}
	}
	else if (SelectedItem != 0) // Translation, Rotation
	{
		static const char* GizmoSpaces[] = { "World", "Local" };
		SelectedItem = static_cast<int>(Editor.GetGizmo().GetSpace());
		if (ImGui::Combo("Gizmo Space", &SelectedItem, GizmoSpaces, 2))
		{
			Editor.GetGizmo().SetGizmoSpace(static_cast<EGizmoSpace>(SelectedItem));
		}
	}

	ImGui::End();
}
