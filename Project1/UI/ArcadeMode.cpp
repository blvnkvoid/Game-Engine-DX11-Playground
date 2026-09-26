#include "ArcadeMode.h"
#include "../Imgui/imgui.h"

bool ArcadeMode::Draw(const UIContext& ui)
{
	return ImGui::Button("Arcade Mode", ui.Size(300, 50));
}