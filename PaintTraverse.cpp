#include "hooks.h"
#include "GameUtils.h"
#include "Draw.h"
#include "Menu.h"
#include "ESP.h"
#include "Misc.h"
#include "global.h"
#include "SpoofedConvar.h"
#include "Hitmarker.h"
#include "GrenadePrediction.h"
#include "global.h"

bool first_frame_passed = false;
std::string sPanel = XorStr("FocusOverlayPanel");

void __fastcall Hooks::PaintTraverse(void* ecx/*thisptr*/, void* edx, unsigned int vgui_panel, bool force_repaint, bool allow_force) // cl
{
	if (Clientvariables->Visuals.EspEnable && Clientvariables->Visuals.Noscope && strcmp("HudZoom", g_pPanel->GetName(vgui_panel)) == 0)
		return;

	panelVMT->GetOriginalMethod<PaintTraverseFn>(41)(ecx, vgui_panel, force_repaint, allow_force);

	static bool bSpoofed = false;

	if (Clientvariables->Misc.TPKey > 0 && !bSpoofed)
	{
		ConVar* svcheats = g_pCvar->FindVar("sv_cheats");
		SpoofedConvar* svcheatsspoof = new SpoofedConvar(svcheats);
		svcheatsspoof->SetInt(1);
		bSpoofed = true;
	}

	const char* pszPanelName = g_pPanel->GetName(vgui_panel);

	if (!strstr(pszPanelName, sPanel.data()))
		return;

	int cur_height, cur_width; g_pEngine->GetScreenSize(cur_width, cur_height);

	if (!first_frame_passed || cur_width != G::Screen.width || cur_height != G::Screen.height)
	{
		first_frame_passed = true;
		g_Draw.Init();

		g_pEngine->GetScreenSize(cur_width, cur_height);
		G::Screen.height = cur_height;
		G::Screen.width = cur_width;
	}

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected() && G::LocalPlayer)
	{

		if (Clientvariables->Antiaim.Yaw != 0 && G::LocalPlayer->isAlive())
		{
			if (G::RealAngle.y == G::LocalPlayer->LowerBodyYaw())
				g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 90, 188, 34, 0, 255, "%s", "LBY");
			else
				g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 90, 100, 255, 0,  255, "%s", "LBY");

			if (Clientvariables->Antiaim.FakeLag.Type > 0 && !(*G::LocalPlayer->GetFlags() & FL_ONGROUND))
			{
				if (!G::SendPacket)
					g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 125, 208, 255, 0, 255, "%s", "LC");
				else
					g_Draw.StringA(g_Draw.font_build, false, 10, G::Screen.height - 125, 188, 34, 0, 255, "%s", "LC");
			}
		}


		if (Clientvariables->Visuals.DamageIndicators)
		{
			G::CurrTime = g_pGlobals->interval_per_tick * (G::LocalPlayer->GetTickBase() + 1);

			if (!G::DamageHit.empty())
			{
				for (auto it = G::DamageHit.begin(); it != G::DamageHit.end();) {
					if (G::CurrTime > it->ExpireTime) {
						it = G::DamageHit.erase(it);
						continue;
					}
					it->Draw();
					++it;
				}
			}
		}

		if (Clientvariables->Visuals.RemoveParticles)
		{
			auto postprocessing = g_pCvar->FindVar("mat_postprocess_enable");
			auto postprocessingspoof = new SpoofedConvar(postprocessing);
			postprocessingspoof->SetInt(0);
		}

		if (Clientvariables->Visuals.Noscope)
		{
			g_ESP->DrawScope(G::LocalPlayer);
			auto postprocessing = g_pCvar->FindVar("mat_postprocess_enable");
			auto postprocessingspoof = new SpoofedConvar(postprocessing);
			postprocessingspoof->SetInt(0);
		}
		g_ESP->Loop();
		grenade_prediction::instance().Paint();
	}

}