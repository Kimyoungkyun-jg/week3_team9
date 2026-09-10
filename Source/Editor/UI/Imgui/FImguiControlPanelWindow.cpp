#include "FImguiControlPanelWindow.h"
#include "ThirdParty/Imgui/imgui.h"
#include "ThirdParty/Imgui/imgui_internal.h"
#include "ThirdParty/Imgui/imgui_impl_dx11.h"
#include "ThirdParty/Imgui/imgui_impl_win32.h"
#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/Core/FString.h"
#include <Windows.h>
#include <ShlObj.h>
#include <filesystem>

namespace
{
    // <디렉토리>/<이름>.Scene 형식으로 씬 경로 생성
    FString MakeScenePath(const char* SceneName)
    {
        PWSTR UserPath = nullptr;

        if (FAILED(SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &UserPath)))
            return "";

        std::filesystem::path Path = UserPath;
        CoTaskMemFree(UserPath);

        Path /= "GameTechLabWeek2";
        Path /= "SceneData";
        Path /= FString(SceneName) + ".Scene";

        return Path.string();
    }
}

void FImguiControlPanelWindow::Process(FEditor& Editor)
{
    const uint64 Count = UObject::GetTotalAllocationCount();
    const uint64 Bytes = UObject::GetTotalAllocationBytes();
    ImGui::Begin("Jungle Control Panel");

    ImGui::Text("Hello Jungle World!");
    ImGui::Text("FPS %.0f (%.0f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Live UObjects : %llu, UObject Memory: %llu bytes (%.2f KiB)", static_cast<unsigned long long>(Count), static_cast<unsigned long long>(Bytes), static_cast<double>(Bytes) / 1024.0);
    ImGui::Separator();

    // ---------------- 프리미티브 스폰 ----------------
    static int primitive = 0;
    const char* primitives[] = { "Cube", "Cylinder", "Sphere" };
    ImGui::SetNextItemWidth(180.0f);
    ImGui::Combo("##Primitive", &primitive, primitives, IM_ARRAYSIZE(primitives));
    ImGui::SameLine();
    ImGui::Text("Primitive");

    static int spawnCount = 1;

    if (ImGui::Button("Spawn"))
    {
        const int Count = (spawnCount < 1) ? 1 : spawnCount;
        for (int i = 0; i < Count; ++i)
        {
            Editor.SpawnPrimitive(static_cast<EEditorPrimitiveType>(primitive));
        }
    }


    ImGui::SameLine();
    ImGui::SetNextItemWidth(180.0f);
    ImGui::InputInt("##SpawnCount", &spawnCount);
    ImGui::SameLine();
    ImGui::Text("Number of spawn");

    //씬 저장, 로드
    static char sceneName[128] = "Default";
    ImGui::SetNextItemWidth(180.0f);
    ImGui::InputText("##SceneName", sceneName, IM_ARRAYSIZE(sceneName));
    ImGui::SameLine();
    ImGui::Text("Scene Name");

    if (ImGui::Button("New scene"))
    {
        Editor.NewScene();
    }
    if (ImGui::Button("Save scene"))
    {
        Editor.SaveScene(MakeScenePath(sceneName));
    }
    if (ImGui::Button("Load scene"))
    {
        Editor.LoadScene(MakeScenePath(sceneName));
    }

    ImGui::Separator();

    
    if (FEditorViewport* Viewport = Editor.GetActiveViewport())
    {
        FCamera& Camera = Viewport->ViewportCamera;

        bool bOrthographic =
            (Camera.Projection.ProjectionType == EProjectionType::Orthographic);
        if (ImGui::Checkbox("Orthogonal", &bOrthographic))
        {
            Camera.Projection.ProjectionType =
                bOrthographic ? EProjectionType::Orthographic : EProjectionType::Perspective;
        }


        ImGui::SetNextItemWidth(180.0f);
        ImGui::DragFloat("##FOV", &Camera.Projection.FOV, 0.1f, 1.0f, 179.0f, "%.1f");
        ImGui::SameLine();
        ImGui::Text("FOV");


        ImGui::SetNextItemWidth(180.0f);
        ImGui::DragFloat3("##CameraLocation", &Camera.Position.X, 0.05f, 0.0f, 0.0f, "%.3f");
        ImGui::SameLine();
        ImGui::Text("Camera Location");

    
        float Rotation[3] = { 0.0f, Camera.Pitch, Camera.Yaw };
        ImGui::SetNextItemWidth(180.0f);
        if (ImGui::DragFloat3("##CameraRotation", Rotation, 0.5f, 0.0f, 0.0f, "%.2f"))
        {

            Camera.Pitch = Rotation[1];
            Camera.Yaw = Rotation[2];
        }
        ImGui::SameLine();
        ImGui::Text("Camera Rotation");
    }

    ImGui::End();
}
