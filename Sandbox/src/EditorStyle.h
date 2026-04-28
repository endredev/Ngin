#pragma once
#include <imgui.h>

struct EditorStyle
{
	// Background shades
	ImVec4 WindowBg      = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	ImVec4 ChildBg       = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	ImVec4 PopupBg       = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	ImVec4 MenuBarBg     = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	ImVec4 TitleBg       = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	ImVec4 TitleBgActive = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);

	// Borders & frames
	ImVec4 Border        = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	ImVec4 FrameBg       = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	// Text
	ImVec4 Text          = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	ImVec4 TextDisabled  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	// Accent (UE orange)
	ImVec4 Accent        = ImVec4(1.00f, 0.42f, 0.00f, 1.00f);
	ImVec4 AccentHovered = ImVec4(1.00f, 0.55f, 0.10f, 1.00f);
	ImVec4 AccentActive  = ImVec4(0.80f, 0.32f, 0.00f, 1.00f);

	// Tabs
	ImVec4 Tab               = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	ImVec4 TabHovered        = ImVec4(1.00f, 0.42f, 0.00f, 0.70f);
	ImVec4 TabSelected       = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	// Rounding & spacing
	float WindowRounding = 2.0f;
	float FrameRounding  = 2.0f;
	float TabRounding    = 2.0f;
	float ItemSpacingX   = 8.0f;
	float ItemSpacingY   = 5.0f;

	static void Apply(const EditorStyle& s = {})
	{
		ImGuiStyle& st = ImGui::GetStyle();

		st.WindowRounding  = s.WindowRounding;
		st.FrameRounding   = s.FrameRounding;
		st.TabRounding     = s.TabRounding;
		st.ItemSpacing     = ImVec2(s.ItemSpacingX, s.ItemSpacingY);
		st.WindowBorderSize = 1.0f;
		st.FrameBorderSize  = 0.0f;

		auto& c = st.Colors;
		c[ImGuiCol_Text]                 = s.Text;
		c[ImGuiCol_TextDisabled]         = s.TextDisabled;
		c[ImGuiCol_WindowBg]             = s.WindowBg;
		c[ImGuiCol_ChildBg]              = s.ChildBg;
		c[ImGuiCol_PopupBg]              = s.PopupBg;
		c[ImGuiCol_Border]               = s.Border;
		c[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);
		c[ImGuiCol_FrameBg]              = s.FrameBg;
		c[ImGuiCol_FrameBgHovered]       = s.AccentHovered;
		c[ImGuiCol_FrameBgActive]        = s.AccentActive;
		c[ImGuiCol_TitleBg]              = s.TitleBg;
		c[ImGuiCol_TitleBgActive]        = s.TitleBgActive;
		c[ImGuiCol_TitleBgCollapsed]     = s.TitleBg;
		c[ImGuiCol_MenuBarBg]            = s.MenuBarBg;
		c[ImGuiCol_ScrollbarBg]          = s.ChildBg;
		c[ImGuiCol_ScrollbarGrab]        = s.FrameBg;
		c[ImGuiCol_ScrollbarGrabHovered] = s.AccentHovered;
		c[ImGuiCol_ScrollbarGrabActive]  = s.AccentActive;
		c[ImGuiCol_CheckMark]            = s.Accent;
		c[ImGuiCol_SliderGrab]           = s.Accent;
		c[ImGuiCol_SliderGrabActive]     = s.AccentActive;
		c[ImGuiCol_Button]               = s.FrameBg;
		c[ImGuiCol_ButtonHovered]        = s.AccentHovered;
		c[ImGuiCol_ButtonActive]         = s.AccentActive;
		c[ImGuiCol_Header]               = s.FrameBg;
		c[ImGuiCol_HeaderHovered]        = s.AccentHovered;
		c[ImGuiCol_HeaderActive]         = s.AccentActive;
		c[ImGuiCol_Separator]            = s.Border;
		c[ImGuiCol_SeparatorHovered]     = s.AccentHovered;
		c[ImGuiCol_SeparatorActive]      = s.AccentActive;
		c[ImGuiCol_ResizeGrip]           = s.FrameBg;
		c[ImGuiCol_ResizeGripHovered]    = s.AccentHovered;
		c[ImGuiCol_ResizeGripActive]     = s.AccentActive;
		c[ImGuiCol_Tab]                  = s.Tab;
		c[ImGuiCol_TabHovered]           = s.TabHovered;
		c[ImGuiCol_TabSelected]          = s.TabSelected;
		c[ImGuiCol_TabDimmed]            = s.Tab;
		c[ImGuiCol_TabDimmedSelected]    = s.FrameBg;
		c[ImGuiCol_DockingPreview]       = s.Accent;
		c[ImGuiCol_DockingEmptyBg]       = s.ChildBg;
	}
};
