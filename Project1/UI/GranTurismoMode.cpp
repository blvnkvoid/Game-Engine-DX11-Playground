#include "GranTurismoMode.h"
#include "../Imgui/imgui.h"

bool GranTurismoMode::Draw(const UIContext& ui)
{
	return  ImGui::Button("Gran Turismo Mode", ui.Size(300, 50));
}