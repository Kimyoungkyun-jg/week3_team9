#pragma once
#include "FImguiConsoleWindow.h"
#include "FImguiControlPanelWindow.h"
#include "FImguiEditorViewportWindow.h"
#include "FImguiPropertyWindow.h"

class FImguiToolbar final {
public:
	void Process(FEditor& Editor, FImguiConsoleWindow& ConsoleWindow,
		FImguiControlPanelWindow& ControlPanelWindow,
		FImguiPropertyWindow& PropertyWindow);
};