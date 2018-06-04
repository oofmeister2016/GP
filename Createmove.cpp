#include "hooks.h"
#include "global.h"
#include "Misc.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "Math.h"
#include "GameUtils.h"
#include "Aimbot.h"
#include "PredictionSystem.h"
#include "Antiaim.h"
#include "FixMove.h"
#include "GrenadePrediction.h"

static CPredictionSystem* Prediction = new CPredictionSystem();
CFixMove FixMove;

QAngle Normalize2(QAngle& angs)
{
	while (angs.y < -180.0f)
		angs.y += 360.0f;
	while (angs.y > 180.0f)
		angs.y -= 360.0f;
	if (angs.x > 89.0f)
		angs.x = 89.0f;
	if (angs.x < -89.0f)
		angs.x = -89.0f;
	angs.z = 0;
	return angs;
}

static int Ticks = 0;
static int LastReserve = 0;

bool __fastcall Hooks::CreateMove(void* thisptr, void*, float flInputSampleTime, CUserCmd* cmd)
{
	if (!cmd->command_number || !cmd)
		return true;

	PVOID pebp;
	__asm mov pebp, ebp;
	bool* pbSendPacket = (bool*)(*(PDWORD)pebp - 0x1C);
	bool& bSendPacket = *pbSendPacket;

	g_pEngine->SetViewAngles(cmd->viewangles);
	QAngle org_view = cmd->viewangles;

	CBaseEntity* LocalPlayer = g_pEntitylist->GetClientEntity(g_pEngine->GetLocalPlayer());



	if (LocalPlayer)
	{
		G::LocalPlayer = LocalPlayer;
		G::UserCmd = cmd;
		G::UserCmdForBacktracking = cmd;
		G::vecUnpredictedVel = G::LocalPlayer->GetVelocity();

		if (LocalPlayer->isAlive())
		{
			if (Clientvariables->Misc.Clantag)
				g_Misc->HandleClantag();

			Prediction->EnginePrediction(cmd);

			FixMove.Start(G::UserCmd);

			CBaseCombatWeapon* pWeapon = LocalPlayer->GetWeapon();
			bool IsLadder = LocalPlayer->GetMoveType() == MOVETYPE_LADDER;
			G::StrafeAngle = G::UserCmd->viewangles;
			if (pWeapon)
			{
				G::MainWeapon = pWeapon;
				G::WeaponData = pWeapon->GetCSWpnData();

				g_Aimbot->DropTarget();

				if ((G::UserCmd->buttons & IN_ATTACK ||
					(G::UserCmd->buttons & IN_ATTACK2 && (G::MainWeapon->WeaponID() == REVOLVER || G::MainWeapon->IsKnife()))) && GameUtils::IsAbleToShoot())
					g_Aimbot->fired_in_that_tick = true;

				g_Aimbot->Run();
				g_Aimbot->CompensateInaccuracies();

				if ((G::UserCmd->buttons & IN_ATTACK || G::UserCmd->buttons & IN_ATTACK2 && G::MainWeapon->WeaponID() == REVOLVER) && (G::MainWeapon->IsPistol() || G::MainWeapon->WeaponID() == AWP || G::MainWeapon->WeaponID() == SSG08))
				{
					static bool bFlip = false;
					if (bFlip)
					{
						if (G::MainWeapon->WeaponID() == REVOLVER)
						{
						}
						else
							G::UserCmd->buttons &= ~IN_ATTACK;
					}
					bFlip = !bFlip;
				}

				if (G::MainWeapon->WeaponID() == REVOLVER && Clientvariables->Ragebot.EnableAimbot)
				{
					if (G::MainWeapon->GetPostponeFireReadyTime() - (float)G::LocalPlayer->GetTickBase() * g_pGlobals->interval_per_tick > 0.045 && Clientvariables->Ragebot.EnableAimbot)
						G::UserCmd->buttons |= IN_ATTACK;
				}
			}

			G::ForceRealAA = false;
			if (G::ChokedPackets >= 14)
			{
				G::SendPacket = true;
				G::ChokedPackets = 0;
				G::ForceRealAA = true;
			}
			if (!IsLadder)
				g_Antiaim->Run(org_view);

			g_Misc->FakeLag();

#define CheckIfNonValidNumber1(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
			
			QAngle OrigAng = G::UserCmd->viewangles;
			static int OldMouseX = OrigAng.y;
			int mousedx = OldMouseX - OrigAng.y;
			OldMouseX = OrigAng.y;
			static Vector LastOrigAng = Normalize2(OrigAng);
			if (Clientvariables->Misc.AutoJump)
			{
				static auto bJumped = false;
				static auto bFake = false;
				if (!bJumped && bFake)
				{
					bFake = false;
					G::UserCmd->buttons |= IN_JUMP;
				}
				else if (G::UserCmd->buttons & IN_JUMP)
				{
					if (*G::LocalPlayer->GetFlags() & FL_ONGROUND)
					{
						bJumped = true;
						bFake = true;
					}
					else
					{
						G::UserCmd->buttons &= ~IN_JUMP;
						bJumped = false;
					}
				}
				else
				{
					bJumped = false;
					bFake = false;
				}
			}
			if (Clientvariables->Misc.AutoStrafe)
			{
				g_Misc->AutoStrafe();
			}
			LastOrigAng.y = Math::NormalizeYaw(LastOrigAng.y);
			LastOrigAng = Normalize2(OrigAng);

			if (G::SendPacket)
			{
				if (Clientvariables->Misc.TPangles == 0)
					G::AAAngle = G::UserCmd->viewangles;

				G::ChokedPackets = 0;
				G::FakeAngle = G::UserCmd->viewangles;
			}
			else
			{
				if (Clientvariables->Misc.TPangles == 1 && !g_Aimbot->aimbotted_in_current_tick)
					G::AAAngle = G::UserCmd->viewangles;

				G::ChokedPackets++;
				G::RealAngle = G::UserCmd->viewangles;
			}

			if (Clientvariables->Misc.TPangles == 2)
				G::AAAngle = QAngle(G::UserCmd->viewangles.x, G::LocalPlayer->LowerBodyYaw());


			g_Misc->FixCmd();

			cmd = G::UserCmd;
			bSendPacket = G::SendPacket;

			grenade_prediction::instance().Tick(G::UserCmd->buttons);

			FixMove.End(G::UserCmd);
		}
	}
	return false;
}
