#include "sdk.h"
#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "Hitmarker.h"
#include "Draw.h"
#include "ESP.h"
#include "ImGUI\imconfig.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_internal.h"
#include "ImGUI\stb_rect_pack.h"
#include "ImGUI\stb_textedit.h"
#include "ImGUI\stb_truetype.h"
#include "ImGUI\DX9\imgui_impl_dx9.h"
#include "Items.h"
#include "Config.h"
#include "GameUtils.h"

typedef void(*CL_FullUpdate_t) (void);
CL_FullUpdate_t CL_FullUpdate = nullptr;

void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
{
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

	Color colColor(255, 255, 255, 255);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
	}
}

void KnifeApplyCallbk()
{
	static auto CL_FullUpdate = reinterpret_cast<CL_FullUpdate_t>(FindPattern("engine.dll", reinterpret_cast<PBYTE>("\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85"), "x????x????xxxxxxx"));
	CL_FullUpdate();
}

PresentFn oPresent;

ImFont* Impact;
ImFont* Default;
ImFont* Tabs;

tReset oResetScene;

void GUI_Init(IDirect3DDevice9* pDevice) // Setup for imgui
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX9_Init(G::Window, pDevice);

	ImGuiStyle& style = ImGui::GetStyle();

	ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF(
		Avalon_compressed_data, Avalon_compressed_size, 13.f);

	ImColor mainColor = ImColor(54, 54, 54, 255);
	ImColor bodyColor = ImColor(54, 54, 54, 255);
	ImColor fontColor = ImColor(255, 255, 255, 255);

	ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
	ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
	ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
	ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
	ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);


	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.18f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.14f, 0.18f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.27f, 0.30f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.27f, 0.30f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.30f, 0.45f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.12f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.12f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.12f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.13f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.43f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.79f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.79f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.27f, 0.31f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.09f, 0.12f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.24f, 0.79f, 0.60f, 0.84f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.27f, 0.31f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.39f, 0.43f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.39f, 0.43f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.39f, 0.43f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.13f, 0.91f, 0.71f, 0.84f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	style.Alpha = 1.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowRounding = 7.f;
	style.FramePadding = ImVec2(4, 1);
	style.ScrollbarSize = 10.f;
	style.ScrollbarRounding = 0.f;
	style.GrabMinSize = 5.f;


	G::Init = true;
}

enum class vKey : UINT
{
	KEY_MOUSE_BUTTON_LEFT = 1,
	KEY_MOUSE_BUTTON_RIGHT,
	KEY_CANCEL,
	KEY_MOUSE_BUTTON_MIDDLE,

	KEY_0 = 48,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_A = 65,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_NUMPAD_0 = 96,
	KEY_NUMPAD_1,
	KEY_NUMPAD_2,
	KEY_NUMPAD_3,
	KEY_NUMPAD_4,
	KEY_NUMPAD_5,
	KEY_NUMPAD_6,
	KEY_NUMPAD_7,
	KEY_NUMPAD_8,
	KEY_NUMPAD_9,

	KEY_F1 = 112,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_SHIFT_LEFT = 160,
	KEY_SHIFT_RIGHT,
	KEY_CTRL_LEFT,
	KEY_CTRL_RIGHT,
	KEY_ALT_LEFT,
	KEY_ALT_RIGHT,

	KEY_ESC = 27,
	KEY_ENTER = 257,
	KEY_TAB,
	KEY_BACKSPACE,

	KEY_SPACE = 0x20,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_END,
	KEY_HOME,
	KEY_ARROW_LEFT,
	KEY_ARROW_UP,
	KEY_ARROW_RIGHT,
	KEY_ARROW_DOWN,

	KEY_PAUSE = 0x13,
	KEY_CAPS_LOCK,

	KEY_NUM_LOCK = 0x90,
	KEY_SCROLL_LOCK,

	KEY_PRINT_SCREEN = 0x2C,
	KEY_INSERT = 20,
	KEY_DELETE = 0x2E
};

struct VIRTUALKEY
{
	vKey  uiKey;
	PCHAR  szKey;
};

std::vector<VIRTUALKEY> vtable =
{
	{ vKey::KEY_MOUSE_BUTTON_LEFT,  "LeftClick" },
{ vKey::KEY_MOUSE_BUTTON_RIGHT,  "RighClick" },
{ vKey::KEY_MOUSE_BUTTON_MIDDLE, "MiddleMouse" },
{ vKey::KEY_0, "0" },
{ vKey::KEY_1, "1" },
{ vKey::KEY_2, "2" },
{ vKey::KEY_3, "3" },
{ vKey::KEY_4, "4" },
{ vKey::KEY_5, "5" },
{ vKey::KEY_6, "6" },
{ vKey::KEY_7, "7" },
{ vKey::KEY_8, "8" },
{ vKey::KEY_9, "9" },
{ vKey::KEY_A, "A" },
{ vKey::KEY_B, "B" },
{ vKey::KEY_C, "C" },
{ vKey::KEY_D, "D" },
{ vKey::KEY_E, "E" },
{ vKey::KEY_F, "F" },
{ vKey::KEY_G, "G" },
{ vKey::KEY_H, "H" },
{ vKey::KEY_I, "I" },
{ vKey::KEY_J, "J" },
{ vKey::KEY_K, "K" },
{ vKey::KEY_L, "L" },
{ vKey::KEY_M, "M" },
{ vKey::KEY_N, "N" },
{ vKey::KEY_O, "O" },
{ vKey::KEY_P, "P" },
{ vKey::KEY_Q, "Q" },
{ vKey::KEY_R, "R" },
{ vKey::KEY_S, "S" },
{ vKey::KEY_T, "T" },
{ vKey::KEY_U, "U" },
{ vKey::KEY_V, "V" },
{ vKey::KEY_W, "W" },
{ vKey::KEY_X, "X" },
{ vKey::KEY_Y, "Y" },
{ vKey::KEY_Z, "Z" },
{ vKey::KEY_NUMPAD_0, "NUM 0" },
{ vKey::KEY_NUMPAD_1, "NUM 1" },
{ vKey::KEY_NUMPAD_2, "NUM 2" },
{ vKey::KEY_NUMPAD_3, "NUM 3" },
{ vKey::KEY_NUMPAD_4, "NUM 4" },
{ vKey::KEY_NUMPAD_5, "NUM 5" },
{ vKey::KEY_NUMPAD_6, "NUM 6" },
{ vKey::KEY_NUMPAD_7, "NUM 7" },
{ vKey::KEY_NUMPAD_8, "NUM 8" },
{ vKey::KEY_NUMPAD_9, "NUM 9" },
{ vKey::KEY_F1, "F1" },
{ vKey::KEY_F2, "F2" },
{ vKey::KEY_F3, "F3" },
{ vKey::KEY_F4, "F4" },
{ vKey::KEY_F5, "F5" },
{ vKey::KEY_F6, "F6" },
{ vKey::KEY_F7, "F7" },
{ vKey::KEY_F8, "F8" },
{ vKey::KEY_F9, "F9" },
{ vKey::KEY_F10, "F10" },
{ vKey::KEY_F11, "F11" },
{ vKey::KEY_F12, "F12" },
{ vKey::KEY_SHIFT_LEFT, "SHIFT L" },
{ vKey::KEY_CTRL_LEFT, "CTRL L" },
{ vKey::KEY_ALT_LEFT, "ALT L" },
{ vKey::KEY_SHIFT_RIGHT, "SHIFT R" },
{ vKey::KEY_CTRL_RIGHT, "CTRL R" },
{ vKey::KEY_ALT_RIGHT, "ALT R" },
{ vKey::KEY_SPACE, "Space" },
{ vKey::KEY_ESC, "Esc" },
{ vKey::KEY_ENTER, "Enter" },
{ vKey::KEY_TAB, "Tab" },
{ vKey::KEY_BACKSPACE, "BackSpace" },
{ vKey::KEY_INSERT, "Insert" },
{ vKey::KEY_DELETE, "Delete" },
{ vKey::KEY_ARROW_RIGHT, "Right" },
{ vKey::KEY_ARROW_LEFT, "Left" },
{ vKey::KEY_ARROW_DOWN, "Down" },
{ vKey::KEY_ARROW_UP, "Up" },
{ vKey::KEY_PAGE_UP, "Page Up" },
{ vKey::KEY_PAGE_DOWN, "Page Down" },
{ vKey::KEY_HOME, "Home" },
{ vKey::KEY_END, "End" },
{ vKey::KEY_CAPS_LOCK, "CapsLock" },
{ vKey::KEY_SCROLL_LOCK, "ScrollLock" },
{ vKey::KEY_NUM_LOCK, "NumLock" },
{ vKey::KEY_PRINT_SCREEN, "PrintScreen" },
{ vKey::KEY_PAUSE, "Pause" },
};


namespace ImGui
{
	bool KeyBinder(int* keyptr, ImVec2 size = ImVec2(100, 15))
	{
		int& key_status = *keyptr;

		string text;

		if (!key_status)
		{
			text = "( None )";
		}
		else if (key_status == -1)
		{
			text = "( Press the key )";

			for (auto i = vtable.begin(); i != vtable.end(); ++i)
			{
				if (!GetAsyncKeyState(UINT(i->uiKey)))
					continue;

				if (i->uiKey == vKey::KEY_MOUSE_BUTTON_LEFT)
				{
					const ImGuiIO io = ImGui::GetIO();
					const ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

					if (io.MousePos.x < cursor_pos.x ||
						io.MousePos.x > cursor_pos.x + size.x ||
						io.MousePos.y < cursor_pos.y ||
						io.MousePos.y > cursor_pos.y + size.y)
					{
						key_status = 0;
						break;
					}
					else continue;
				}

				if (i->uiKey == vKey::KEY_ESC )
				{
					key_status = 0;
					break;
				}

				key_status = (int)i->uiKey;
				keybd_event((byte)key_status, 0, KEYEVENTF_KEYUP, 0);//поднимаю кнопку

				break;
			}
		}

		if (key_status > 0)
			for (auto a = vtable.begin(); a != vtable.end(); a++)
				if ((int)a->uiKey == key_status)
				{
					text = "";
					text.append("( ");
					text.append(a->szKey);
					text.append(" )");
				}

		const bool button = ImGui::Button(text.c_str(), size);

		if (button)
		{
			if (!key_status)
				key_status = -1;
			else if (key_status > 0)
				key_status = -1;
		}

		return button;
	}
}

HRESULT __stdcall Hooks::D3D9_EndScene(IDirect3DDevice9* pDevice)
{
	HRESULT result = d3d9VMT->GetOriginalMethod<EndSceneFn>(42)(pDevice);

	if (!G::Init)
	{
		GUI_Init(pDevice);
	}
	else
	{
		if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
			ImGui::GetIO().MouseDrawCursor = G::Opened;
		else
			ImGui::GetIO().MouseDrawCursor = true;
		ImGui_ImplDX9_NewFrame();

		auto& style = ImGui::GetStyle();

		if (G::Opened && style.Alpha < 1.f)
		{
			G::ShowMenu = true;
			if (style.Alpha > 1.f)
				style.Alpha = 1.f;
			else if (style.Alpha != 1.f)
				style.Alpha += 0.03f;
		}
		else if (style.Alpha > 0.f)
		{
			if (style.Alpha < 0.f)
				style.Alpha = 0.f;
			else if (style.Alpha != 0.f)
				style.Alpha -= 0.03f;
			if (style.Alpha == 0.f)
				G::ShowMenu = false;
		}

		static int tab;

		if (G::ShowMenu)
		{
			ImVec2 mainWindowPos; 
		
			ImGui::Begin("project2k csgo", &G::ShowMenu, ImVec2(649, 539), 0.98f, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/ /*| ImGuiWindowFlags_NoTitleBar*/| ImGuiWindowFlags_NoScrollbar);
			{
				mainWindowPos = ImGui::GetWindowPos();

				ImGui::Separator();
				static int page = 0;
				const char* tabs[] = {
					"Ragebot",
					"Visuals",
					"Other"
				};

				for (int i = 0; i < ARRAYSIZE(tabs); i++)
				{

					int distance = i == page ? 0 : i > page ? i - page : page - i;

					if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / ARRAYSIZE(tabs) - 9, 20)))
						page = i;

					if (i < ARRAYSIZE(tabs) - 1)
						ImGui::SameLine();

				}
				ImGui::Separator();

				switch (page)
				{
				case 0:
				{
					ImGui::Checkbox("Enabled", &Clientvariables->Ragebot.EnableAimbot);
					if (Clientvariables->Ragebot.EnableAimbot)
					{
						ImGui::Separator();
						static int page = 0;
						const char* tabs[] = {
							"Aimbot",
							"Hitscan",
							"AntiAims"
						};

						for (int i = 0; i < ARRAYSIZE(tabs); i++)
						{

							int distance = i == page ? 0 : i > page ? i - page : page - i;

							if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / ARRAYSIZE(tabs) - 9, 20)))
								page = i;

							if (i < ARRAYSIZE(tabs) - 1)
								ImGui::SameLine();

						}
						ImGui::Separator();
						switch (page)
						{
						case 0:
						{
							ImGui::PushItemWidth(120);
							ImGui::Columns(2, nullptr, false);
							ImGui::SetColumnOffset(1, 250);
							{
								ImGui::Checkbox("Silent aim", &Clientvariables->Ragebot.SilentAimbot);
								ImGui::Checkbox("Auto Shoot", &Clientvariables->Ragebot.AutomaticFire);
								ImGui::Checkbox("Auto Scope", &Clientvariables->Ragebot.AutomaticScope);

								ImGui::Combo("Selection", &Clientvariables->Ragebot.AimbotSelection, SelectionMode, ARRAYSIZE(SelectionMode));

								ImGui::Checkbox("AutoWall", &Clientvariables->Ragebot.Autowall);
								ImGui::Checkbox("AutoWall on HitScan", &Clientvariables->Ragebot.AutowallHitscan);
			
								ImGui::SliderInt("Aimstep", &Clientvariables->Ragebot.ResolverStepAngle, 25, 90);
							}
							ImGui::NextColumn();
							{
								ImGui::PushItemWidth(120);
								if (Clientvariables->Misc.AntiUT)
									Clientvariables->Ragebot.NoSpread = false;

								ImGui::Checkbox("Backtrack", &Clientvariables->Ragebot.PositionAdjustment);
								ImGui::Checkbox("Resolver", &Clientvariables->Ragebot.AutomaticResolver);

								style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
								ImGui::Checkbox("No recoil", &Clientvariables->Ragebot.NoRecoil);
								style.Colors[ImGuiCol_Text] = Clientvariables->Misc.AntiUT ? ImColor(180, 180, 100, 255) : ImColor(255, 255, 255, 255);
								ImGui::Checkbox("No spread", &Clientvariables->Ragebot.NoSpread);
								style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
								ImGui::SliderInt("Hitchancee", &Clientvariables->Ragebot.Minhitchance, 0, 100, "%.0f%%");

								ImGui::SliderInt("Minimum damage", &Clientvariables->Ragebot.Mindamage, 1, 101);
							}
							ImGui::Columns(1);
						}
						break;
						case 1:
						{
							ImGui::PushItemWidth(120);
							ImGui::Columns(2, nullptr, false);
							ImGui::SetColumnOffset(1, 250);
							{
								ImGui::Combo("Hitbox", &Clientvariables->Ragebot.Hitbox, HitboxMode, ARRAYSIZE(HitboxMode));

								ImGui::SliderFloat("PointScale", &Clientvariables->Ragebot.Headscale, 0.f, 1.f, "%.2f%%");
							}
							ImGui::NextColumn();
							{
								ImGui::PushItemWidth(120);
								ImGui::Combo("Body Aim", &Clientvariables->Ragebot.Preferbodyaim, PreferBodyaim, ARRAYSIZE(PreferBodyaim));

								ImGui::Checkbox("Head", &Clientvariables->Ragebot.Head);
								ImGui::Checkbox("Neck", &Clientvariables->Ragebot.Neck);
								ImGui::SameLine();
								ImGui::Checkbox("Edges", &Clientvariables->Ragebot.NeckFull);
								ImGui::Checkbox("Body", &Clientvariables->Ragebot.Body);
								ImGui::SameLine();
								ImGui::Checkbox("Edges ", &Clientvariables->Ragebot.BodyFull);
								ImGui::Checkbox("Arms", &Clientvariables->Ragebot.Arms);
								ImGui::SameLine();
								ImGui::Checkbox("Edges ", &Clientvariables->Ragebot.ArmsFull);
								ImGui::Checkbox("Legs ", &Clientvariables->Ragebot.Legs);
								ImGui::SameLine();
								ImGui::Checkbox("Edges   ", &Clientvariables->Ragebot.LegsFull);
							}
						}
						break;
						case 2: 
						{
							ImGui::PushItemWidth(120);
							ImGui::Columns(2, nullptr, false);
							ImGui::SetColumnOffset(1, 250);
							{
								ImGui::Checkbox("Enable Anti-Aim", &Clientvariables->Antiaim.Enable);
								ImGui::Checkbox("Enable Pitch", &Clientvariables->Antiaim.Pitch);
								ImGui::Combo("Yaw", &Clientvariables->Antiaim.Yaw, AntiaimbotYaw, ARRAYSIZE(AntiaimbotYaw));

								if (Clientvariables->Antiaim.Yaw == 3)
								{
									ImGui::Text("Left"); ImGui::SameLine();
									ImGui::KeyBinder(&Clientvariables->Antiaim.Right);

									ImGui::Text("Right"); ImGui::SameLine();
									ImGui::KeyBinder(&Clientvariables->Antiaim.Left);
								}

								ImGui::SliderFloat("LBYDelta", &Clientvariables->Antiaim.LBYDelta, -180, 180, "%.0f");

								ImGui::Spacing();
								static const char* fakelag[] = {
									"Off",
									"Factor",
									"Switch",
									"Adaptive",
									"Fluctuate"
								};
								ImGui::Spacing();
								ImGui::Combo("Fake-Lag", &Clientvariables->Antiaim.FakeLag.Type, fakelag, ARRAYSIZE(fakelag));
								ImGui::SliderInt("Factor", &Clientvariables->Antiaim.FakeLag.Amount, 1, 15);
								ImGui::Checkbox("In air", &Clientvariables->Antiaim.FakeLag.FakeLagInAirOnly);
							}
							ImGui::NextColumn();
							{
								ImGui::PushItemWidth(120);
								ImGui::Text("Force third person");
								ImGui::KeyBinder(&Clientvariables->Misc.TPKey, ImVec2(164, 20));
								ImGui::Combo("Type", &Clientvariables->Misc.TPangles, ThirdpersonAngles, ARRAYSIZE(ThirdpersonAngles));
								ImGui::SliderFloat("Range", &Clientvariables->Visuals.TRange, 0, 300, "%.0f");

							
							}
							ImGui::Columns(1);

						}
						break;
						}
					}
				}
				break;
				case 1:
				{
					ImGui::Checkbox("Enabled", &Clientvariables->Visuals.EspEnable);
					if (Clientvariables->Visuals.EspEnable)
					{
						ImGui::Separator();
						static int page = 0;
						const char* tabs[] = {
							"Main",
							"Chams",
							"Misc",
						};

						for (int i = 0; i < ARRAYSIZE(tabs); i++)
						{

							int distance = i == page ? 0 : i > page ? i - page : page - i;

							if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / ARRAYSIZE(tabs) - 9, 20)))
								page = i;

							if (i < ARRAYSIZE(tabs) - 1)
								ImGui::SameLine();

						}
						ImGui::Separator();

						switch (page)
						{
						case 0:
							ImGui::PushItemWidth(120);
							ImGui::Columns(2, nullptr, false);
							ImGui::SetColumnOffset(1, 250);
							{

								ImGui::Checkbox("Enemy only", &Clientvariables->Visuals.EnemyOnly);
								ImGui::Checkbox("2D Box", &Clientvariables->Visuals.BoundingBox);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##box", Clientvariables->Colors.BoundingBox, 1 << 7);
								ImGui::Checkbox("Skeleton", &Clientvariables->Visuals.Bones);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##bones", Clientvariables->Colors.Skeletons, 1 << 7);
								ImGui::Checkbox("Name", &Clientvariables->Visuals.Name);
								ImGui::Checkbox("Health", &Clientvariables->Visuals.Health);
								ImGui::Checkbox("Armor", &Clientvariables->Visuals.Armor);
								ImGui::Checkbox("Weapon", &Clientvariables->Visuals.Weapon);
								ImGui::Checkbox("Ammo", &Clientvariables->Visuals.Ammo);
								ImGui::Checkbox("Snap lines", &Clientvariables->Visuals.SnapLines);
								ImGui::Checkbox("Resolver State", &Clientvariables->Visuals.Fake);
								ImGui::Checkbox("Hitmarker", &Clientvariables->Visuals.Hitmarker);
							}
							ImGui::NextColumn();
							{
								ImGui::PushItemWidth(120);
								ImGui::Checkbox("Glow", &Clientvariables->Visuals.Glow);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##glow", Clientvariables->Colors.Glow, 1 << 7);
								ImGui::SliderFloat("Alpha", &Clientvariables->Colors.gla, 0, 255, "%.0f");

								ImGui::Checkbox("Grenade trajectory", &Clientvariables->Visuals.GrenadePrediction);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##nadetrajectory", Clientvariables->Colors.GrenadePrediction, 1 << 7);
								
								ImGui::Checkbox("Bullet Tracers", &Clientvariables->Visuals.BulletTracers);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##tracers", Clientvariables->Colors.Bulletracer, 1 << 7);
	
								ImGui::Checkbox("Show Hit Damage", &Clientvariables->Visuals.DamageIndicators);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##damageindicator", Clientvariables->Colors.DamageIndicator, 1 << 7);

								ImGui::Combo("Dropped weapons", &Clientvariables->Visuals.DroppedWeapons, DroppedWeapons, ARRAYSIZE(DroppedWeapons));
								ImGui::SameLine();
								ImGui::MyColorEdit3("##weaponcolor", Clientvariables->Colors.DroppedWeapon, 1 << 7);

								ImGui::Checkbox("Spread crosshair", &Clientvariables->Visuals.SpreadCrosshair);
								ImGui::SameLine();
								ImGui::MyColorEdit3("##spreadcolor", Clientvariables->Colors.SpreadCrosshair, 1 << 7);

								ImGui::Checkbox("Flags", &Clientvariables->Visuals.Flags);
								ImGui::Checkbox("Bomb", &Clientvariables->Visuals.Bomb);
								ImGui::Checkbox("Grenades", &Clientvariables->Visuals.ThrownNades);
				
							}
							ImGui::Columns(1);
							break;
						case 1:
						{
							ImGui::PushItemWidth(120);
							ImGui::Checkbox("Enabled##chams", &Clientvariables->Visuals.ChamsEnable);

							ImGui::Combo("Type", &Clientvariables->Visuals.ChamsStyle, ModelsMode, ARRAYSIZE(ModelsMode));

							ImGui::Checkbox("Enemy", &Clientvariables->Visuals.ChamsPlayer);
							ImGui::SameLine();
							ImGui::MyColorEdit3("##chams", Clientvariables->Colors.PlayerChams, 1 << 7);

							ImGui::Checkbox("XQZ", &Clientvariables->Visuals.ChamsPlayerWall);
							ImGui::SameLine();
							ImGui::MyColorEdit3("##chamswall", Clientvariables->Colors.PlayerChamsWall, 1 << 7);

							ImGui::Checkbox("On Hands", &Clientvariables->Visuals.ChamsHands);
							ImGui::SameLine();
							ImGui::MyColorEdit3("##handchams", Clientvariables->Colors.ChamsHand, 1 << 7);

							ImGui::Checkbox("On Hands Wireframe", &Clientvariables->Visuals.ChamsHandsWireframe);
							ImGui::SameLine();
							ImGui::MyColorEdit3("##handwireframe", Clientvariables->Colors.WireframeHand, 1 << 7);

							ImGui::Checkbox("On Weapon", &Clientvariables->Visuals.WeaponChams);
							ImGui::SameLine();
							ImGui::MyColorEdit3("##weaponchams", Clientvariables->Colors.ChamsWeapon, 1 << 7);

							ImGui::Checkbox("On Weapon Wireframe", &Clientvariables->Visuals.WeaponWireframe);
							ImGui::SameLine();
							ImGui::MyColorEdit3("##weaponwireframe", Clientvariables->Colors.WireframeWeapon, 1 << 7);


							ImGui::Combo("Fake Chams", &Clientvariables->Visuals.FakeAngleGhost, Fakeghost, ARRAYSIZE(Fakeghost));
							ImGui::SameLine();
							ImGui::MyColorEdit3("##ghostcolor", Clientvariables->Colors.FakeAngleGhost, 1 << 7);


						}
						break;

						case 2:
						{
							ImGui::Columns(2, nullptr, false);
							ImGui::SetColumnOffset(1, 250);
							{
								ImGui::PushItemWidth(120);
								ImGui::SliderInt("Flash alpha", &Clientvariables->Visuals.FlashbangAlpha, 0, 255, "%.0f%%");
								ImGui::Checkbox("Remove smoke", &Clientvariables->Visuals.Nosmoke);
								ImGui::Checkbox("Remove visual recoil", &Clientvariables->Visuals.Novisrevoil);
								ImGui::Checkbox("Remove scope", &Clientvariables->Visuals.Noscope);
								ImGui::Checkbox("Disable post processing", &Clientvariables->Visuals.RemoveParticles);
							}
							ImGui::NextColumn();
							{
								ImGui::PushItemWidth(120);
								ImGui::Combo("Skybox", &Clientvariables->Visuals.Skybox, Skyboxmode, ARRAYSIZE(Skyboxmode));
								ImGui::Checkbox("Night mode", &Clientvariables->Visuals.nightmode);
								ImGui::SliderInt("Fov", &Clientvariables->Misc.PlayerFOV, -50, 50);
								ImGui::SliderInt("Viewmodel", &Clientvariables->Misc.PlayerViewmodel, 0, 80);
							}
							ImGui::Columns();
						}
						break;
						}

					}
				}
				break;
				case 2:
				{
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnOffset(1, 250);
					{
						ImGui::Checkbox("Anti-untrusted", &Clientvariables->Misc.AntiUT);

						ImGui::Checkbox("Bunny hop", &Clientvariables->Misc.AutoJump);
						ImGui::Checkbox("Auto Strafe", &Clientvariables->Misc.AutoStrafe);
						ImGui::Checkbox("Clantag", &Clientvariables->Misc.Clantag);
			
					}
					ImGui::NextColumn();
					{
						ImGui::PushItemWidth(120);
						ImGui::Combo("Cfg", &Clientvariables->Misc.ConfigSelection, Configs, ARRAYSIZE(Configs));

						if (ImGui::Button("Save CFG", ImVec2(120, 20)))
							ConSys->SaveConfig();

						if (ImGui::Button("Load CFG", ImVec2(120, 20)))
							ConSys->LoadConfig();

						if (ImGui::Button("Reset CFG", ImVec2(120, 20)))
							ConSys->Reset();


						if (ImGui::Button("Reset Font", ImVec2(120, 20)))
							g_Draw.Init();
					}
					ImGui::Columns(1);
				}
				break;
				}
		
			} ImGui::End();

		}
		ImGui::Render();
	}
	return result;
}

HRESULT __stdcall Hooks::hkdReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresParam)
{
	if (!G::Init)
		return oResetScene(pDevice, pPresParam);

	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto newr = oResetScene(pDevice, pPresParam);
	ImGui_ImplDX9_CreateDeviceObjects();

	return newr;
}
