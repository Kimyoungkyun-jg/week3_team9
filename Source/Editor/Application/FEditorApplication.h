#pragma once
#include "Editor/Core/FEditor.h"
#include "Editor/UI/Imgui/FImguiManager.h"
#include "Editor/UI/Imgui/FImguiToolBar.h"
#include "Editor/UI/Imgui/FImguiPropertyWindow.h"
#include "Editor/UI/Imgui/FImguiEditorViewportWindow.h"
#include "Editor/UI/Imgui/FImguiControlPanelWindow.h"
#include "Editor/UI/Imgui/FImguiConsoleWindow.h"
#include "Runtime/Engine/FRenderView.h"
#include "Runtime/Input/FCameraInputController.h"

class FEditorApplication final {
	FEditor Editor;

	USceneManager* SceneManager = nullptr;

	FImguiManager ImguiManager;

	FImguiToolbar ToolBar;
	FImguiControlPanelWindow ControlPanelWindow;
	FImguiEditorViewportWindow EditorViewportWindow;
	FImguiPropertyWindow PropertyWindow;
	FImguiConsoleWindow ConsoleWindow;

	FRenderView* RenderView = nullptr;

public:
	
	static FEditorApplication& Get()
	{
		static FEditorApplication Instance;
		return Instance;
	}

	FEditorApplication(const FEditorApplication&) = delete;
	FEditorApplication& operator=(const FEditorApplication&) = delete;

	FEditorApplication(FEditorApplication&&) = delete;
	FEditorApplication& operator=(FEditorApplication&&) = delete;

	void Initialize_ImguiWin32DX11(HWND& Window, ID3D11Device* Device, ID3D11DeviceContext* Context);
	void Initialize_Runtime(FRenderResourceLibrary* RendererLibrary, USceneManager* SceneManager, FRenderView* RenderView); // TODO: RendererLibrary의존성 사라져야 함
	bool CheckSceneExistsAndInitializeIfNotExists(const FString& path = "");
	void Update(float DeltaTime);
	void Render();
	void OnWindowSize(UINT Width, UINT Height);

	void CollectGarbage();

private:
	FEditorApplication() = default;
	~FEditorApplication() = default;
	void BeginFrame();
	void Tick(float DeltaTime);
};
