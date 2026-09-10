#include "FImguiToolBar.h"
#include "ThirdParty/Imgui/imgui.h"

void FImguiToolbar::Process(FEditor& Editor, FImguiConsoleWindow& ConsoleWindow, FImguiControlPanelWindow& ControlPanelWindow, FImguiPropertyWindow& PropertyWindow)
{
	if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Console")) { ConsoleWindow.bIsOpened = true; }
            ImGui::EndMenu();
        }

        auto& Gizmo = Editor.GetGizmo();
        
		static const char* GizmoModes[4] = { "None", "Translation", "Rotation", "Scale" };
        const int SelectedItem = static_cast<int>(Editor.GetGizmo().Mode);
		if (ImGui::Button(GizmoModes[SelectedItem], { 150.0f, 0.0f }))
		{
			Gizmo.Mode = static_cast<EGizmoMode>((SelectedItem + 1) % 4);
		}

        ImGui::EndMainMenuBar();
	}
}
