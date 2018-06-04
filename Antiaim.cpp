#include "sdk.h"
#include "Antiaim.h"
#include "global.h"
#include "GameUtils.h"
#include "Math.h"
#include "Misc.h"
#include "Aimbot.h"

CAntiaim* g_Antiaim = new CAntiaim;

#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )


/*LBY Predict (credits: corruption.vip)*/
bool ShouldPredict() {
	/*------определения------*/
	static float next1 = 0;
	float tickbase = TICKS_TO_TIME(G::LocalPlayer->GetTickBase());
	float flServerTime = (G::LocalPlayer->GetTickBase()  * g_pGlobals->interval_per_tick);
	/*----------------------*/

	if (next1 - tickbase > 1.1) {
		next1 = 0;
		return false;
	}

	if (G::LocalPlayer->GetVelocity().Length2D() > 0.1f) {
		next1 = flServerTime;
		return false;
	}

	if ((next1 + 1.1 <= flServerTime) && (*G::LocalPlayer->GetFlags() & FL_ONGROUND) && (G::LocalPlayer->GetVelocity().Length2D() < 0.1f)) {
		next1 = flServerTime + g_pGlobals->interval_per_tick;
		return true;
	}
	return false;
}

bool GetAlive(CBaseEntity* pLocal)
{
	for (int i = 1; i < 64; ++i)
	{
		if (i == g_pEngine->GetLocalPlayer())
			continue;

		CBaseEntity* target = g_pEntitylist->GetClientEntity(i);
		player_info_t info;
		//Calls from left->right so we wont get an access violation error
		Vector pos;
		if (!target || target->GetHealth() < 1)
			continue;
		if (pLocal->GetTeamNum() != target->GetTeamNum())
		{
			return true;
		}
	}
	return false;
}

/*anti-aims*/


void CAntiaim::Run(QAngle org_view)
{
	QAngle oldAngle = G::UserCmd->viewangles;
	float oldForward = G::UserCmd->forwardmove;
	float oldSideMove = G::UserCmd->sidemove;
	if (Clientvariables->Antiaim.Enable)
	{

		static int iChokedPackets = -1;

		if ((g_Aimbot->fired_in_that_tick && iChokedPackets < 4 && GameUtils::IsAbleToShoot()) && !G::ForceRealAA)
		{
			G::SendPacket = false;
			iChokedPackets++;
		}
		else
		{
			iChokedPackets = 0;

			CGrenade* pCSGrenade = (CGrenade*)G::LocalPlayer->GetWeapon();

			if (G::UserCmd->buttons & IN_USE
				/*|| !GetAlive(G::LocalPlayer) && !Clientvariables->Misc.AntiUT*/
				|| G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER && G::LocalPlayer->GetVelocity().Length() > 0
				|| G::LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP
				|| *G::LocalPlayer->GetFlags() & FL_FROZEN
				|| pCSGrenade && pCSGrenade->GetThrowTime() > 0.f)

				return;

			choke = !choke;
			if (!Clientvariables->Misc.FakelagEnable || (*G::LocalPlayer->GetFlags() & FL_ONGROUND && !Clientvariables->Misc.FakelagOnground || *G::LocalPlayer->GetFlags() & FL_ONGROUND && G::LocalPlayer->GetVelocity().Length() < 3))
				G::SendPacket = choke;

			G::UserCmd->viewangles = org_view; //fixes aimbot angles

			if (Clientvariables->Antiaim.Pitch)
			{
				if (!(*G::LocalPlayer->GetFlags() & FL_ONGROUND)) {
					G::UserCmd->viewangles.x = Math::RandomFloat(50, 89);
				}
				else {
					G::UserCmd->viewangles.x = 89;
				}

			}

			if (*G::LocalPlayer->GetFlags() & FL_ONGROUND)
			{
				if (Clientvariables->Antiaim.Yaw == 0)
				{

				}
				else if (Clientvariables->Antiaim.Yaw == 1)
				{
					if (ShouldPredict() && !G::SendPacket)
						G::UserCmd->viewangles.y += Clientvariables->Antiaim.LBYDelta;

					G::UserCmd->viewangles.y += 179;
				}
				else if (Clientvariables->Antiaim.Yaw == 2)
				{
					if (ShouldPredict() && !G::SendPacket)
						G::UserCmd->viewangles.y += Clientvariables->Antiaim.LBYDelta;

					G::UserCmd->viewangles.y += 180 + Math::RandomFloat(-35, 35);
				}
				else if (Clientvariables->Antiaim.Yaw == 3)
				{
					if (ShouldPredict() && !G::SendPacket)
						G::UserCmd->viewangles.y += Clientvariables->Antiaim.LBYDelta;

					static bool flip;
					if (GetAsyncKeyState(Clientvariables->Antiaim.Left))
						flip = true;
					else if (GetAsyncKeyState(Clientvariables->Antiaim.Right))
						flip = false;

					if (flip)
					{
						if (G::SendPacket)
							G::UserCmd->viewangles.y += 90;
						else
						{
							G::UserCmd->viewangles.y -= 90;
						}
					}
					else
					{
						if (G::SendPacket)
							G::UserCmd->viewangles.y -= 90;
						else
						{
							G::UserCmd->viewangles.y += 90;
						}
					}
				}
			}
			else
			{
				if (Clientvariables->Antiaim.Yaw == 0)
				{

				}
				else
				{
					static int Ticks;
					if (G::SendPacket)
					{
						G::UserCmd->viewangles.y -= Ticks; // 180z using ticks
					}
					else
					{
						G::UserCmd->viewangles.y += Ticks; // 180z using ticks
					}
					Ticks += 2;

					if (Ticks > 240)
						Ticks = 115;
				}
			}
		}
	}
}