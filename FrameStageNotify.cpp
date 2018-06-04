#include "hooks.h"
#include "global.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "xor.h"
#include <intrin.h>
#include "SpoofedConvar.h"
#include "Math.h"
#include "Skinchanger.h"
#include "Items.h"
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

ConVar* mp_facefronttime;
ConVar* r_DrawSpecificStaticProp;



int get_model_index(int item_def_index)
{
	int ret = 0;
	switch (item_def_index)
	{
	case KNIFE:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		break;
	case KNIFE_T:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		break;
	case KNIFE_KARAMBIT:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
		break;
	case KNIFE_BAYONET:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
		break;
	case KNIFE_M9_BAYONET:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
		break;
	case KNIFE_TACTICAL:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
		break;
	case KNIFE_GUT:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
		break;
	case KNIFE_FALCHION:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
		break;
	case KNIFE_PUSH:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
		break;
	case KNIFE_BUTTERFLY:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
		break;
	case KNIFE_FLIP:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
		break;
	case KNIFE_BOWIE:
		ret = g_pModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
		break;
	default:
		break;
	}
	return ret;
}

struct ResolverData
{
	float simtime, flcycle[13], flprevcycle[13], flweight[13], flweightdatarate[13], fakewalkdetection[2], fakeanglesimtimedetection[2], fakewalkdetectionsimtime[2];
	float yaw, addyaw, lbycurtime;
	float shotsimtime, oldlby, lastmovinglby, balanceadjustsimtime, balanceadjustflcycle;
	int fakeanglesimtickdetectionaverage[4], amountgreaterthan2, amountequal1or2, amountequal0or1, amountequal1, amountequal0, resetmovetick, resetmovetick2;
	int tick, balanceadjusttick, missedshots, activity[13];
	bool bfakeangle, bfakewalk, playerhurtcalled, weaponfirecalled;
	Vector shotaimangles, hitboxPos, balanceadjustaimangles;
	uint32_t norder[13];
	char* resolvermode = "NONE", *fakewalk = "Not Moving";
} pResolverData[64];;

bool isPartOf(char *a, char *b) {
	if (std::strstr(b, a) != NULL) {    //Strstr says does b contain a
		return true;
	}
	return false;
}


std::vector<const char*> smoke_materials = {
	"particle/beam_smoke_01",
	"particle/particle_smokegrenade",
	"particle/particle_smokegrenade1",
	"particle/particle_smokegrenade2",
	"particle/particle_smokegrenade3",
	"particle/particle_smokegrenade_sc",
	"particle/smoke1/smoke1",
	"particle/smoke1/smoke1_ash",
	"particle/smoke1/smoke1_nearcull",
	"particle/smoke1/smoke1_nearcull2",
	"particle/smoke1/smoke1_snow",
	"particle/smokesprites_0001",
	"particle/smokestack",
	"particle/vistasmokev1/vistasmokev1",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_nearcull",
	"particle/vistasmokev1/vistasmokev1_nearcull_fog",
	"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev4_emods_nocull",
	"particle/vistasmokev1/vistasmokev4_nearcull",
	"particle/vistasmokev1/vistasmokev4_nocull"
};

void DrawBeam(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 1.5f;
	beamInfo.m_flWidth = 3.f;
	beamInfo.m_flEndWidth = 3.f;
	beamInfo.m_flFadeLength = 1.0f;
	beamInfo.m_flAmplitude = 0.0f;
	beamInfo.m_flBrightness = color.a();
	beamInfo.m_flSpeed = 1.00f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 6;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = 0;

	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;

	Beam_t* myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);

	if (myBeam)
		g_pViewRenderBeams->DrawBeam(myBeam);
}

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t curStage)
{
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;

	if (!G::LocalPlayer || !g_pEngine->IsConnected() || !g_pEngine->IsInGame())
	{
		clientVMT->GetOriginalMethod<FrameStageNotifyFn>(36)(curStage);
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}



	if (OldNightmode != Clientvariables->Visuals.nightmode)
	{

		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_pCvar->FindVar("r_DrawSpecificStaticProp");

		r_DrawSpecificStaticProp->SetValue(0);

		for (MaterialHandle_t i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_pMaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (Clientvariables->Visuals.nightmode)
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
						pMaterial->ColorModulate(0.3f, 0.3f, 0.3f);
					else
						pMaterial->ColorModulate(0.05f, 0.05f, 0.05f);
				else
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}
			
		}
		OldNightmode = Clientvariables->Visuals.nightmode;
	}


	if (OldSky != Clientvariables->Visuals.Skybox)
	{
		auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(FindPatternIDA("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
		if (old_Skyname == "")
		{
			//old_Skyname = g_pCvar->FindVar("sv_skyname")->GetName();
		}

		int type = Clientvariables->Visuals.Skybox;

		/*if(type == 0)
			LoadNamedSky(old_Skyname.c_str());
		else */if (type == 1)
	LoadNamedSky("cs_baggage_skybox_");
		else if (type == 2)
			LoadNamedSky("cs_tibet");
		else if (type == 3)
			LoadNamedSky("italy");
		else if (type == 4)
			LoadNamedSky("jungle");
		else if (type == 5)
			LoadNamedSky("office");
		else if (type == 6)
			LoadNamedSky("sky_cs15_daylight02_hdr");
		else if (type == 7)
			LoadNamedSky("sky_csgo_night02");
		else if (type == 8)
			LoadNamedSky("vertigo");

		OldSky = Clientvariables->Visuals.Skybox;
	}



	static Vector oldViewPunch;
	static Vector oldAimPunch;


	Vector* view_punch = G::LocalPlayer->GetViewPunchPtr();
	Vector* aim_punch = G::LocalPlayer->GetPunchAnglePtr();

	if (curStage == FRAME_RENDER_START && G::LocalPlayer->GetHealth() > 0)
	{
		static bool enabledtp = false, check = false;

		if (GetAsyncKeyState(Clientvariables->Misc.TPKey))
		{
			if (!check)
				enabledtp = !enabledtp;
			check = true;
		}
		else
			check = false;

		if (enabledtp)
		{
			*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(G::LocalPlayer + 0x31C0 + 0x8)) = G::AAAngle; //to visualize real/faked aa angles
		}

		if (view_punch && aim_punch && Clientvariables->Visuals.Novisrevoil)
		{
			oldViewPunch = *view_punch;
			oldAimPunch = *aim_punch;

			view_punch->Init();
			aim_punch->Init();
		}

		if (enabledtp && G::LocalPlayer->isAlive()) {
			*(bool*)((DWORD)g_pInput + 0xA5) = true;
			*(float*)((DWORD)g_pInput + 0xA8 + 0x8) = Clientvariables->Visuals.TRange; // Distance
		}
		else {
			*(bool*)((DWORD)g_pInput + 0xA5) = false;
			*(float*)((DWORD)g_pInput + 0xA8 + 0x8);
		}
	}

	if (curStage == FRAME_NET_UPDATE_START)
	{
		if (Clientvariables->Visuals.BulletTracers)
		{
			float Red, Green, Blue;

			Red = Clientvariables->Colors.Bulletracer[0] * 255;
			Green = Clientvariables->Colors.Bulletracer[1] * 255;
			Blue = Clientvariables->Colors.Bulletracer[2] * 255;

			for (unsigned int i = 0; i < trace_logs.size(); i++) {

				auto *shooter = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(trace_logs[i].userid));

				if (!shooter) return;

				Color color;
				if (shooter->GetTeamNum() == 3)
					color = Color(Red, Green, Blue, 210);
				else
					color = Color(Red, Green, Blue, 210);

				DrawBeam(trace_logs[i].start, trace_logs[i].position, color);

				trace_logs.erase(trace_logs.begin() + i);
			}
		}


		for (auto material_name : smoke_materials) {
			IMaterial* mat = g_pMaterialSystem->FindMaterial(material_name, TEXTURE_GROUP_OTHER);
			mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Clientvariables->Visuals.Nosmoke ? true : false);
		}

		if (Clientvariables->Visuals.Nosmoke) {
			static int* smokecount = *(int**)(FindPatternIDA("client.dll", "A3 ? ? ? ? 57 8B CB") + 0x1);
			*smokecount = 0;
		}

		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{

			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0)
				{
					if (i != g_pEngine->GetLocalPlayer())
					{
						VarMapping_t* map = pEntity->GetVarMap();
						if (map)
						{
							if (Clientvariables->Ragebot.PositionAdjustment)
							{
								map->m_nInterpolatedEntries = 0;
							}
							else
							{
								if (map->m_nInterpolatedEntries == 0)
									map->m_nInterpolatedEntries = 6;
							}
						}

					}
				}
			}
		}
	}

	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			if (i == g_pEngine->GetLocalPlayer())
				continue;
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0 && !pEntity->IsDormant())
				{
					*(int*)((uintptr_t)pEntity + 0xA30) = g_pGlobals->framecount; //we'll skip occlusion checks now
					*(int*)((uintptr_t)pEntity + 0xA28) = 0;//clear occlusion flags
				}
			}
		}
	}

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			if (i == g_pEngine->GetLocalPlayer())
				continue;
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);

			if (pEntity && pEntity->GetHealth() > 0)
			{
				if (pEntity->IsDormant())
					continue;
				int index = pEntity->Index();

				if (Clientvariables->Ragebot.AutomaticResolver && G::LocalPlayer->isAlive())
				{
					float c_m_flWeight = g_BacktrackHelper->PlayerRecord[i].currentLayer.m_flWeight;
					float c_m_flCycle = g_BacktrackHelper->PlayerRecord[i].currentLayer.m_flCycle;

					float p_m_flWeight = g_BacktrackHelper->PlayerRecord[i].previous_layer.m_flWeight;
					float p_m_flCycle = g_BacktrackHelper->PlayerRecord[i].previous_layer.m_flCycle;

					if (G::weaponfirecalled)
					{
						if (!G::playerhurtcalled)
						{
							pResolverData[i].addyaw += 65.f;
							Math::NormalizeYaw(pResolverData[i].addyaw);
						}
						else
							G::playerhurtcalled = false;
						G::weaponfirecalled = false;
					}

					const int activity = pEntity->GetSequenceActivity(g_BacktrackHelper->PlayerRecord[i].currentLayer.m_nSequence);
					const int activity2 = pEntity->GetSequenceActivity(g_BacktrackHelper->PlayerRecord[i].previous_layer.m_nSequence);

					Vector* pAngles = pEntity->GetEyeAnglesPtr();

					if (pEntity->GetVelocity().Length2D() > 100)
					{
						pResolverData[pEntity->GetIndex()].lastmovinglby = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget;
						pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget;
					}
					else if (pEntity->GetVelocity().Length2D() > 15)
					{

						pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget;
					}
					else
					{
						pResolverData[pEntity->GetIndex()].oldlby = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget;
						if (activity == ACT_CSGO_IDLE_TURN_BALANCEADJUST) {
							if (activity2 == ACT_CSGO_IDLE_TURN_BALANCEADJUST) {
								if ((p_m_flCycle != c_m_flCycle) || c_m_flWeight == 1.f)
								{
									float
										flAnimTime = c_m_flCycle,
										flSimTime = pEntity->GetSimulationTime();

									if (flAnimTime < 0.01f && p_m_flCycle > 0.01f && Clientvariables->Ragebot.PositionAdjustment) // lby flicked here
									{
										G::FakeDetection[i] = 1;
										pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby;
										pResolverData[pEntity->GetIndex()].addyaw > 0.f ? pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget - pResolverData[pEntity->GetIndex()].addyaw : pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget - 17.5;
									}

									G::FakeDetection[i] = 1;
									if (pResolverData[pEntity->GetIndex()].oldlby != pResolverData[pEntity->GetIndex()].lastmovinglby)
										pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby;
									else
										pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget - 180.f;
								}
								else if (c_m_flWeight == 0.f && (p_m_flCycle > 0.92f && c_m_flCycle > 0.92f)) // breaking lby with delta < 120; can fakewalk here aswell
								{
									G::FakeDetection[i] = 1;
									if (pResolverData[pEntity->GetIndex()].oldlby != pResolverData[pEntity->GetIndex()].lastmovinglby)
										pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby;
									else
										pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget - 180.f;
								}
							}
						}
						/*другая ресольва  ака брут :thinking:*/
						else {

							G::FakeDetection[i] = 2;
							pAngles->y = pResolverData[pEntity->GetIndex()].lastmovinglby;
							pResolverData[pEntity->GetIndex()].addyaw > 0.f ? pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget - pResolverData[pEntity->GetIndex()].addyaw : pAngles->y = g_BacktrackHelper->PlayerRecord[i].LowerBodyYawTarget - 17.5;
						}
					}
				}
				else
				{
					g_BacktrackHelper->PlayerRecord[i].records.clear();
				}
			}
		}

		if (Clientvariables->Skinchanger.Enabled)
		{

			CBaseCombatWeapon* pWeapon = G::LocalPlayer->GetWeapon();

			if (pWeapon)
			{
				if (pWeapon->GetItemDefinitionIndex() == WEAPON_GLOCK)
				{
					int GlockLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < GlockLoop; i++)
					{
						if (Clientvariables->Skinchanger.GlockSkin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_P250)
				{
					int P250Loop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < P250Loop; i++)
					{
						if (Clientvariables->Skinchanger.P250Skin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_DEAGLE)
				{
					int DeagleLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < DeagleLoop; i++)
					{
						if (Clientvariables->Skinchanger.DeagleSkin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_TEC9)
				{
					int Tec9Loop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < Tec9Loop; i++)
					{
						if (Clientvariables->Skinchanger.tec9Skin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_CZ75)
				{
					int CZAutoLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < CZAutoLoop; i++)
					{
						if (Clientvariables->Skinchanger.CZ75Skin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
				{
					int RevolverLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < RevolverLoop; i++)
					{
						if (Clientvariables->Skinchanger.RevolverSkin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_P2000)
				{
					int P2000Loop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < P2000Loop; i++)
					{
						if (Clientvariables->Skinchanger.P2000Skin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_USPS)
				{
					int USPLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < USPLoop; i++)
					{
						if (Clientvariables->Skinchanger.USPSkin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_DUALS)
				{
					int DualsLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < DualsLoop; i++)
					{
						if (Clientvariables->Skinchanger.DualSkins == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_FIVE7)
				{
					int FiveSevenLoop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < FiveSevenLoop; i++)
					{
						if (Clientvariables->Skinchanger.FiveSevenSkin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
				else if (pWeapon->GetItemDefinitionIndex() == WEAPON_AK47)
				{
					int AK47Loop = SIZEOF(Enumerated_Skins_Values);

					for (int i = 0; i < AK47Loop; i++)
					{
						if (Clientvariables->Skinchanger.AK47Skin == i) {
							pWeapon->SetPattern(Enumerated_Skins_Values[i], 1, 1, 1337, Enumerated_Skin_Names[i]);
						}
					}
				}
			}
		}
	}

	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0 && !pEntity->IsDormant())
				{

					g_BacktrackHelper->UpdateBacktrackRecords(pEntity);
					g_BacktrackHelper->UpdateExtrapolationRecords(pEntity);

				}
			}
		}
	}

	clientVMT->GetOriginalMethod<FrameStageNotifyFn>(36)(curStage);

	if (curStage == FRAME_RENDER_START && G::LocalPlayer && G::LocalPlayer->GetHealth() > 0)
	{
		if (Clientvariables->Visuals.Novisrevoil)
		{
			*aim_punch = oldAimPunch;
			*view_punch = oldViewPunch;
		}
	}
}


