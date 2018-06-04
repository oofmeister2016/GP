#include "sdk.h"
#include <winerror.h>
#pragma warning( disable : 4091)
#include <ShlObj.h>
#include <string>
#include <sstream>
#include "xor.h"
#include "Config.h"

#pragma region Disabling warnings

#pragma warning(disable: 4244)
#pragma warning(disable: 4800)
#pragma warning(disable: 4018)
#pragma warning(disable: 4715)

#pragma endregion

Config* ConSys = new Config();

inline bool Check(std::string File)
{
	struct stat buf;
	return (stat(File.c_str(), &buf) == 0);
}

bool Config::CheckConfigs()
{
	/*=====================================*/
	strcat(Path, "C:\\project2k");
	CreateDirectoryA(Path, nullptr);
	strcpy(Path2, Path);
	strcpy(Path3, Path);
	strcpy(Path4, Path);

	strcat(Path, "\\Rage.ini");
	strcat(Path2, "\\Rage2.ini");
	strcat(Path3, "\\Legit.ini");
	strcat(Path4, "\\Casual.ini");

	return true;
}

char* Config::Handle()
{
	if (Clientvariables->Misc.ConfigSelection == 0)
	{
		ConSys->Legit = false;
		ConSys->Rage = true;
		ConSys->RageMM = false;
		ConSys->Casual = false;
	}

	if (Clientvariables->Misc.ConfigSelection == 1)
	{
		ConSys->Legit = true;
		ConSys->Rage = false;
		ConSys->RageMM = false;
		ConSys->Casual = false;
	}

	if (Clientvariables->Misc.ConfigSelection == 2)
	{
		ConSys->Legit = false;
		ConSys->Rage = false;
		ConSys->RageMM = true;
		ConSys->Casual = false;
	}

	if (Clientvariables->Misc.ConfigSelection == 3)
	{
		ConSys->Legit = false;
		ConSys->Rage = false;
		ConSys->RageMM = false;
		ConSys->Casual = true;
	}
	if (Rage)
		return Path;
	if (Legit)
		return Path2;
	if (RageMM)
		return Path3;
	if (Casual)
		return Path4;
}

int Config::ReadInt(char* Cata, char* Name, int DefaultVal)
{
	int iResult;
	iResult = GetPrivateProfileIntA(Cata, Name, DefaultVal, this->Handle());
	return iResult;
}

float Config::ReadFloat(char* Cata, char* Name, float DefaultVal)
{
	char result[255];
	char cDefault[255];
	float fresult;
	sprintf(cDefault, "%f", DefaultVal);
	GetPrivateProfileStringA(Cata, Name, cDefault, result, 255, this->Handle());
	fresult = atof(result);
	return fresult;
}

char* Config::ReadString(char* Cata, char* Name, char* DefaultVal)
{
	auto result = new char[255];
	DWORD oProtection;
	VirtualProtect(reinterpret_cast<void*>(result), 4, PAGE_READWRITE, &oProtection);
	memset(result, 0x00, 255);
	VirtualProtect(reinterpret_cast<void*>(result), 4, oProtection, &oProtection);
	GetPrivateProfileStringA(Cata, Name, DefaultVal, result, 255, this->Handle());
	return result;
}

void Config::WriteFloat(char* Cata, char* Name, float SetVal)
{
	char buf[255];
	sprintf(buf, "%f", SetVal);
	WritePrivateProfileStringA(Cata, Name, buf, this->Handle());
}

void Config::WriteInt(char* Cata, char* Name, int SetVal)
{
	char buf[255];
	sprintf(buf, "%d", SetVal);
	WritePrivateProfileStringA(Cata, Name, buf, this->Handle());
}

void Config::WriteString(char* Cata, char* Name, char* SetVal)
{
	WritePrivateProfileStringA(Cata, Name, SetVal, this->Handle());
}

void Config::SaveConfig()
{
	/*-----------------------------------------------------------------------------------------*/
	WriteInt(("Ragebot"), ("AimbotSelection"), Clientvariables->Ragebot.AimbotSelection);
	WriteInt(("Ragebot"), ("AutomaticFire"), Clientvariables->Ragebot.AutomaticFire);
	WriteInt(("Ragebot"), ("AutomaticResolver"), Clientvariables->Ragebot.AutomaticResolver);
	WriteInt(("Ragebot"), ("Stepangle"), Clientvariables->Ragebot.ResolverStepAngle);
	WriteInt(("Ragebot"), ("AutomaticScope"), Clientvariables->Ragebot.AutomaticScope);
	WriteInt(("Ragebot"), ("Autowall"), Clientvariables->Ragebot.Autowall);
	WriteInt(("Ragebot"), ("AutowallHitscan"), Clientvariables->Ragebot.AutowallHitscan);
	WriteInt(("Ragebot"), ("EnableAimbot"), Clientvariables->Ragebot.EnableAimbot);
	WriteInt(("Ragebot"), ("FriendlyFire"), Clientvariables->Ragebot.FriendlyFire);
	WriteInt(("Ragebot"), ("Hitbox"), Clientvariables->Ragebot.Hitbox);
	WriteInt(("Ragebot"), ("Hitchance"), Clientvariables->Ragebot.Hitchance);


	//hitscan bones
	WriteInt(("Ragebot"), ("Head"), Clientvariables->Ragebot.Head);
	WriteInt(("Ragebot"), ("Neck"), Clientvariables->Ragebot.Neck);
	WriteInt(("Ragebot"), ("NeckFull"), Clientvariables->Ragebot.NeckFull);
	WriteInt(("Ragebot"), ("Body"), Clientvariables->Ragebot.Body);
	WriteInt(("Ragebot"), ("BodyFull"), Clientvariables->Ragebot.BodyFull);
	WriteInt(("Ragebot"), ("Arms"), Clientvariables->Ragebot.Arms);
	WriteInt(("Ragebot"), ("ArmsFull"), Clientvariables->Ragebot.ArmsFull);
	WriteInt(("Ragebot"), ("Legs"), Clientvariables->Ragebot.Legs);
	WriteInt(("Ragebot"), ("LegsFull"), Clientvariables->Ragebot.LegsFull);


	WriteInt(("Ragebot"), ("Mindamage"), Clientvariables->Ragebot.Mindamage);
	WriteInt(("Ragebot"), ("Minhitchance"), Clientvariables->Ragebot.Minhitchance);
	WriteInt(("Ragebot"), ("Multipoint"), Clientvariables->Ragebot.Multipoint);
	WriteInt(("Ragebot"), ("NoRecoil"), Clientvariables->Ragebot.NoRecoil);
	WriteInt(("Ragebot"), ("NoSpread"), Clientvariables->Ragebot.NoSpread);
	WriteFloat(("Ragebot"), ("Headscale"), Clientvariables->Ragebot.Headscale);
	WriteFloat(("Ragebot"), ("Bodyscale"), Clientvariables->Ragebot.Bodyscale);
	WriteInt(("Ragebot"), ("PositionAdjustment"), Clientvariables->Ragebot.PositionAdjustment);
	WriteInt(("Ragebot"), ("Preferbodyaim"), Clientvariables->Ragebot.Preferbodyaim);
	WriteInt(("Ragebot"), ("SilentAimbot"), Clientvariables->Ragebot.SilentAimbot);

	//Antiaims
	WriteInt(("AntiAim"), ("Enable"), Clientvariables->Antiaim.Enable);
	WriteInt(("AntiAim"), ("Pitch"), Clientvariables->Antiaim.Pitch);
	WriteInt(("AntiAim"), ("Yaw"), Clientvariables->Antiaim.Yaw);
	WriteInt(("AntiAim"), ("Left"), Clientvariables->Antiaim.Left);
	WriteInt(("AntiAim"), ("Right"), Clientvariables->Antiaim.Right);
	WriteFloat(("AntiAim"), ("Delta"), Clientvariables->Antiaim.LBYDelta);


	WriteInt(("AntiAim"), ("Fakelag"), Clientvariables->Antiaim.FakeLag.Type);
	WriteInt(("AntiAim"), ("FakeLag.Amount"), Clientvariables->Antiaim.FakeLag.Amount);
	WriteInt(("AntiAim"), ("FakeLagInAirOnly"), Clientvariables->Antiaim.FakeLag.FakeLagInAirOnly);

	WriteInt(("Visuals"), ("Ammo"), Clientvariables->Visuals.Ammo);
	WriteInt(("Visuals"), ("Armor"), Clientvariables->Visuals.Armor);
	WriteInt(("Visuals"), ("Bomb"), Clientvariables->Visuals.Bomb);
	WriteInt(("Visuals"), ("Bones"), Clientvariables->Visuals.Bones);
	WriteInt(("Visuals"), ("BoundingBox"), Clientvariables->Visuals.BoundingBox);
	WriteInt(("Visuals"), ("BulletTracers"), Clientvariables->Visuals.BulletTracers);
	WriteInt(("Visuals"), ("Weapon"), Clientvariables->Visuals.Weapon);
	WriteInt(("Visuals"), ("ChamsEnable"), Clientvariables->Visuals.ChamsEnable);
	WriteInt(("Visuals"), ("Chamsenemyonly"), Clientvariables->Visuals.Chamsenemyonly);
	WriteInt(("Visuals"), ("ChamsPlayer"), Clientvariables->Visuals.ChamsPlayer);
	WriteInt(("Visuals"), ("ChamsPlayerWall"), Clientvariables->Visuals.ChamsPlayerWall);
	WriteInt(("Visuals"), ("ChamsStyle"), Clientvariables->Visuals.ChamsStyle);
	WriteInt(("Visuals"), ("Crosshair"), Clientvariables->Visuals.Crosshair);
	WriteInt(("Visuals"), ("Dlight"), Clientvariables->Visuals.Dlight);
	WriteInt(("Visuals"), ("DroppedWeapons"), Clientvariables->Visuals.DroppedWeapons);
	WriteInt(("Visuals"), ("EnemyOnly"), Clientvariables->Visuals.EnemyOnly);
	WriteInt(("Visuals"), ("EspEnable"), Clientvariables->Visuals.EspEnable);
	WriteInt(("Visuals"), ("FakeChams"), Clientvariables->Visuals.FakeAngleGhost);
	WriteInt(("Visuals"), ("Glow"), Clientvariables->Visuals.Glow);
	WriteInt(("Visuals"), ("GrenadePrediction"), Clientvariables->Visuals.GrenadePrediction);
	WriteInt(("Visuals"), ("Health"), Clientvariables->Visuals.Health);
	WriteInt(("Visuals"), ("Hitmarker"), Clientvariables->Visuals.Hitmarker);
	WriteInt(("Visuals"), ("Hostage"), Clientvariables->Visuals.Hostage);
	WriteInt(("Visuals"), ("LineofSight"), Clientvariables->Visuals.LineofSight);
	WriteInt(("Visuals"), ("LocalPlayer"), Clientvariables->Visuals.LocalPlayer);
	WriteInt(("Visuals"), ("Name"), Clientvariables->Visuals.Name);
	WriteInt(("Visuals"), ("nightmode"), Clientvariables->Visuals.nightmode);
	WriteInt(("Visuals"), ("FlashAlpha"), Clientvariables->Visuals.FlashbangAlpha);
	WriteInt(("Visuals"), ("Noscope"), Clientvariables->Visuals.Noscope);
	WriteInt(("Visuals"), ("Nosmoke"), Clientvariables->Visuals.Nosmoke);
	WriteInt(("Visuals"), ("Novisrevoil"), Clientvariables->Visuals.Novisrevoil);
	WriteInt(("Visuals"), ("Rank"), Clientvariables->Visuals.Rank);
	WriteInt(("Visuals"), ("RecoilCrosshair"), Clientvariables->Visuals.RecoilCrosshair);
	WriteInt(("Visuals"), ("RemoveParticles"), Clientvariables->Visuals.RemoveParticles);
	WriteInt(("Visuals"), ("Skybox"), Clientvariables->Visuals.Skybox);
	WriteInt(("Visuals"), ("SnapLines"), Clientvariables->Visuals.SnapLines);
	WriteInt(("Visuals"), ("Spectators"), Clientvariables->Visuals.Spectators);
	WriteInt(("Visuals"), ("SpreadCrosshair"), Clientvariables->Visuals.SpreadCrosshair);
	WriteInt(("Visuals"), ("ThrownNades"), Clientvariables->Visuals.ThrownNades);
	WriteInt(("Visuals"), ("Wins"), Clientvariables->Visuals.Wins);
	WriteFloat(("Visuals"), ("gla"), Clientvariables->Colors.gla);
	WriteFloat(("Visuals"), ("TPRange"), Clientvariables->Visuals.TRange);

	WriteInt(("Miscellaneous"), ("AntiUT"), Clientvariables->Misc.AntiUT);
	WriteInt(("Miscellaneous"), ("AutoAccept"), Clientvariables->Misc.AutoAccept);
	WriteInt(("Miscellaneous"), ("AutoJump"), Clientvariables->Misc.AutoJump);
	WriteInt(("Miscellaneous"), ("AutoStrafe"), Clientvariables->Misc.AutoStrafe);
	WriteInt(("Miscellaneous"), ("Clantag"), Clientvariables->Misc.Clantag);
	WriteInt(("Miscellaneous"), ("ConfigSelection"), Clientvariables->Misc.ConfigSelection);
	WriteInt(("Miscellaneous"), ("MenuKey"), Clientvariables->Misc.MenuKey);
	WriteInt(("Miscellaneous"), ("PlayerFOV"), Clientvariables->Misc.PlayerFOV);
	WriteInt(("Miscellaneous"), ("PlayerViewmodel"), Clientvariables->Misc.PlayerViewmodel);
	WriteInt(("Miscellaneous"), ("Prespeed"), Clientvariables->Misc.Prespeed);
	WriteInt(("Miscellaneous"), ("PrespeedKey"), Clientvariables->Misc.PrespeedKey);
	WriteInt(("Miscellaneous"), ("Retrack"), Clientvariables->Misc.Retrack);
	WriteInt(("Miscellaneous"), ("TPangles"), Clientvariables->Misc.TPangles);
	WriteInt(("Miscellaneous"), ("TPKey"), Clientvariables->Misc.TPKey);
	WriteInt(("Miscellaneous"), ("Props"), Clientvariables->Misc.Props);
	WriteInt(("Miscellaneous"), ("Walls"), Clientvariables->Misc.Walls);
	//Menu
	WriteFloat(("Colors"), ("MenuR"), Clientvariables->Colors.MenuColor[0]);
	WriteFloat(("Colors"), ("MenuG"), Clientvariables->Colors.MenuColor[1]);
	WriteFloat(("Colors"), ("MenuB"), Clientvariables->Colors.MenuColor[2]);

	//Glow

	WriteFloat(("Colors"), ("GlowR"), Clientvariables->Colors.Glow[0]);
	WriteFloat(("Colors"), ("GlowG"), Clientvariables->Colors.Glow[1]);
	WriteFloat(("Colors"), ("GlowB"), Clientvariables->Colors.Glow[2]);

	//Bones
	WriteFloat(("Colors"), ("BonesR"), Clientvariables->Colors.Skeletons[0]);
	WriteFloat(("Colors"), ("BonesG"), Clientvariables->Colors.Skeletons[1]);
	WriteFloat(("Colors"), ("BonesB"), Clientvariables->Colors.Skeletons[2]);

	//Box 
	WriteFloat(("Colors"), ("BoxR"), Clientvariables->Colors.BoundingBox[0]);
	WriteFloat(("Colors"), ("BoxG"), Clientvariables->Colors.BoundingBox[1]);
	WriteFloat(("Colors"), ("BoxB"), Clientvariables->Colors.BoundingBox[2]);

	//Chams vis
	WriteFloat(("Colors"), ("ChamsPlayerR"), Clientvariables->Colors.PlayerChams[0]);
	WriteFloat(("Colors"), ("ChamsPlayerG"), Clientvariables->Colors.PlayerChams[1]);
	WriteFloat(("Colors"), ("ChamsPlayerB"), Clientvariables->Colors.PlayerChams[2]);

	//Chams Occluded
	WriteFloat(("Colors"), ("ChamsNONR"), Clientvariables->Colors.PlayerChamsWall[0]);
	WriteFloat(("Colors"), ("ChamsNONG"), Clientvariables->Colors.PlayerChamsWall[1]);
	WriteFloat(("Colors"), ("ChamsNONB"), Clientvariables->Colors.PlayerChamsWall[2]);

	//Tracers
	WriteFloat(("Colors"), ("TracersR"), Clientvariables->Colors.Bulletracer[0]);
	WriteFloat(("Colors"), ("TracersG"), Clientvariables->Colors.Bulletracer[1]);
	WriteFloat(("Colors"), ("TracersB"), Clientvariables->Colors.Bulletracer[2]);
}

void Config::LoadConfig()
{
	/*-----------------------------------------------------------------------------------------*/
	Clientvariables->Ragebot.AimbotSelection = ReadInt(("Ragebot"), ("AimbotSelection"), 0);
	Clientvariables->Ragebot.AutomaticFire = ReadInt(("Ragebot"), ("AutomaticFire"), 0);
	Clientvariables->Ragebot.AutomaticResolver = ReadInt(("Ragebot"), ("AutomaticResolver"), 0);
	Clientvariables->Ragebot.ResolverStepAngle = ReadInt(("Ragebot"), ("Stepangle"), 0);
	Clientvariables->Ragebot.AutomaticScope = ReadInt(("Ragebot"), ("AutomaticScope"), 0);
	Clientvariables->Ragebot.Autowall = ReadInt(("Ragebot"), ("Autowall"), 0);
	Clientvariables->Ragebot.AutowallHitscan = ReadInt(("Ragebot"), ("AutowallHitscan"), 0);
	Clientvariables->Ragebot.EnableAimbot = ReadInt(("Ragebot"), ("EnableAimbot"), 0);
	Clientvariables->Ragebot.FriendlyFire = ReadInt(("Ragebot"), ("FriendlyFire"), 0);
	Clientvariables->Ragebot.Hitbox = ReadInt(("Ragebot"), ("Hitbox"), 0);
	Clientvariables->Ragebot.Hitchance = ReadInt(("Ragebot"), ("Hitchance"), 0);

	Clientvariables->Ragebot.Head = ReadInt(("Ragebot"), ("Head"), 0);
	Clientvariables->Ragebot.Neck = ReadInt(("Ragebot"), ("Neck"), 0);
	Clientvariables->Ragebot.NeckFull = ReadInt(("Ragebot"), ("NeckFull"), 0);
	Clientvariables->Ragebot.Body = ReadInt(("Ragebot"), ("Body"), 0);
	Clientvariables->Ragebot.BodyFull = ReadInt(("Ragebot"), ("BodyFull"), 0);
	Clientvariables->Ragebot.Arms = ReadInt(("Ragebot"), ("Arms"), 0);
	Clientvariables->Ragebot.ArmsFull = ReadInt(("Ragebot"), ("ArmsFull"), 0);
	Clientvariables->Ragebot.Legs = ReadInt(("Ragebot"), ("Legs"), 0);
	Clientvariables->Ragebot.LegsFull = ReadInt(("Ragebot"), ("LegsFull"), 0);


	Clientvariables->Ragebot.Mindamage = ReadInt(("Ragebot"), ("Mindamage"), 0);
	Clientvariables->Ragebot.Minhitchance = ReadInt(("Ragebot"), ("Minhitchance"), 0);
	Clientvariables->Ragebot.Multipoint = ReadInt(("Ragebot"), ("Multipoint"), 0);
	Clientvariables->Ragebot.NoRecoil = ReadInt(("Ragebot"), ("NoRecoil"), 0);
	Clientvariables->Ragebot.NoSpread = ReadInt(("Ragebot"), ("NoSpread"), 0);
	Clientvariables->Ragebot.Headscale = ReadFloat(("Ragebot"), ("Headscale"), 0);
	Clientvariables->Ragebot.Bodyscale = ReadFloat(("Ragebot"), ("Bodyscale"), 0);
	Clientvariables->Ragebot.PositionAdjustment = ReadInt(("Ragebot"), ("PositionAdjustment"), 0);
	Clientvariables->Ragebot.Preferbodyaim = ReadInt(("Ragebot"), ("Preferbodyaim"), 0);
	Clientvariables->Ragebot.SilentAimbot = ReadInt(("Ragebot"), ("SilentAimbot"), 0);

	//Antiaims load
	Clientvariables->Antiaim.Enable = ReadInt(("AntiAim"), ("Enable"), 0);
	Clientvariables->Antiaim.Pitch = ReadInt(("AntiAim"), ("Pitch"), 0);
	Clientvariables->Antiaim.Yaw = ReadInt(("AntiAim"), ("Yaw"), 0);
	Clientvariables->Antiaim.Left = ReadFloat(("AntiAim"), ("Left"), 0);
	Clientvariables->Antiaim.Right = ReadInt(("AntiAim"), ("Right"), 0);
	Clientvariables->Antiaim.LBYDelta = ReadFloat(("AntiAim"), ("Delta"), 0);

	Clientvariables->Antiaim.FakeLag.Type = ReadInt(("AntiAim"), ("Fakelag"), 0);
	Clientvariables->Antiaim.FakeLag.Amount = ReadInt(("AntiAim"), ("FakeLag.Amount"), 0);
	Clientvariables->Antiaim.FakeLag.FakeLagInAirOnly = ReadInt(("AntiAim"), ("FakeLagInAirOnly"), 0);

	Clientvariables->Visuals.TRange = ReadFloat(("Visuals"), ("TPRange"), 0);

	Clientvariables->Visuals.Ammo = ReadInt(("Visuals"), ("Ammo"), 0);
	Clientvariables->Visuals.Armor = ReadInt(("Visuals"), ("Armor"), 0);
	Clientvariables->Visuals.Bomb = ReadInt(("Visuals"), ("Bomb"), 0);
	Clientvariables->Visuals.Bones = ReadInt(("Visuals"), ("Bones"), 0);
	Clientvariables->Visuals.BoundingBox = ReadInt(("Visuals"), ("BoundingBox"), 0);
	Clientvariables->Visuals.BulletTracers = ReadInt(("Visuals"), ("BulletTracers"), 0);
	Clientvariables->Visuals.Weapon = ReadInt(("Visuals"), ("Weapon"), 0);
	Clientvariables->Visuals.ChamsEnable = ReadInt(("Visuals"), ("ChamsEnable"), 0);
	Clientvariables->Visuals.Chamsenemyonly = ReadInt(("Visuals"), ("Chamsenemyonly"), 0);
	Clientvariables->Visuals.ChamsPlayer = ReadInt(("Visuals"), ("ChamsPlayer"), 0);
	Clientvariables->Visuals.ChamsPlayerWall = ReadInt(("Visuals"), ("ChamsPlayerWall"), 0);
	Clientvariables->Visuals.ChamsStyle = ReadInt(("Visuals"), ("ChamsStyle"), 0);
	Clientvariables->Visuals.Crosshair = ReadInt(("Visuals"), ("Crosshair"), 0);
	Clientvariables->Visuals.Dlight = ReadInt(("Visuals"), ("Dlight"), 0);
	Clientvariables->Visuals.DroppedWeapons = ReadInt(("Visuals"), ("DroppedWeapons"), 0);
	Clientvariables->Visuals.EnemyOnly = ReadInt(("Visuals"), ("EnemyOnly"), 0);
	Clientvariables->Visuals.EspEnable = ReadInt(("Visuals"), ("EspEnable"), 0);
	Clientvariables->Visuals.FakeAngleGhost = ReadInt(("Visuals"), ("FakeChams"), 0);
	Clientvariables->Visuals.Glow = ReadInt(("Visuals"), ("Glow"), 0);
	Clientvariables->Visuals.GrenadePrediction = ReadInt(("Visuals"), ("GrenadePrediction"), 0);
	Clientvariables->Visuals.Health = ReadInt(("Visuals"), ("Health"), 0);
	Clientvariables->Visuals.Hitmarker = ReadInt(("Visuals"), ("Hitmarker"), 0);
	Clientvariables->Visuals.Hostage = ReadInt(("Visuals"), ("Hostage"), 0);
	Clientvariables->Visuals.LineofSight = ReadInt(("Visuals"), ("LineofSight"), 0);
	Clientvariables->Visuals.LocalPlayer = ReadInt(("Visuals"), ("LocalPlayer"), 0);
	Clientvariables->Visuals.Name = ReadInt(("Visuals"), ("Name"), 0);
	Clientvariables->Visuals.nightmode = ReadInt(("Visuals"), ("nightmode"), 0);
	Clientvariables->Visuals.FlashbangAlpha = ReadInt(("Visuals"), ("FlashAlpha"), 255);
	Clientvariables->Visuals.Noscope = ReadInt(("Visuals"), ("Noscope"), 0);
	Clientvariables->Visuals.Nosmoke = ReadInt(("Visuals"), ("Nosmoke"), 0);
	Clientvariables->Visuals.Novisrevoil = ReadInt(("Visuals"), ("Novisrevoil"), 0);
	Clientvariables->Visuals.Rank = ReadInt(("Visuals"), ("Rank"), 0);
	Clientvariables->Visuals.RecoilCrosshair = ReadInt(("Visuals"), ("RecoilCrosshair"), 0);
	Clientvariables->Visuals.RemoveParticles = ReadInt(("Visuals"), ("RemoveParticles"), 0);
	Clientvariables->Visuals.Skybox = ReadInt(("Visuals"), ("Skybox"), 0);
	Clientvariables->Visuals.SnapLines = ReadInt(("Visuals"), ("SnapLines"), 0);
	Clientvariables->Visuals.Spectators = ReadInt(("Visuals"), ("Spectators"), 0);
	Clientvariables->Visuals.SpreadCrosshair = ReadInt(("Visuals"), ("SpreadCrosshair"), 0);
	Clientvariables->Visuals.ThrownNades = ReadInt(("Visuals"), ("ThrownNades"), 0);
	Clientvariables->Visuals.Wins = ReadInt(("Visuals"), ("Wins"), 0);

	Clientvariables->Colors.gla = ReadInt(("Visuals"), ("gla"), 0);

	Clientvariables->Misc.AntiUT = ReadInt(("Miscellaneous"), ("AntiUT"), 0);
	Clientvariables->Misc.AutoAccept = ReadInt(("Miscellaneous"), ("AutoAccept"), 0);
	Clientvariables->Misc.AutoJump = ReadInt(("Miscellaneous"), ("AutoJump"), 0);
	Clientvariables->Misc.AutoStrafe = ReadInt(("Miscellaneous"), ("AutoStrafe"), 0);
	Clientvariables->Misc.Clantag = ReadInt(("Miscellaneous"), ("Clantag"), 0);
	Clientvariables->Misc.ConfigSelection = ReadInt(("Miscellaneous"), ("ConfigSelection"), 0);
	Clientvariables->Misc.MenuKey = ReadInt(("Miscellaneous"), ("MenuKey"), 0);
	Clientvariables->Misc.PlayerFOV = ReadInt(("Miscellaneous"), ("PlayerFOV"), 0);
	Clientvariables->Misc.PlayerViewmodel = ReadInt(("Miscellaneous"), ("PlayerViewmodel"), 0);
	Clientvariables->Misc.Prespeed = ReadInt(("Miscellaneous"), ("Prespeed"), 0);
	Clientvariables->Misc.PrespeedKey = ReadInt(("Miscellaneous"), ("PrespeedKey"), 0);
	Clientvariables->Misc.Retrack = ReadInt(("Miscellaneous"), ("Retrack"), 0);
	Clientvariables->Misc.TPangles = ReadInt(("Miscellaneous"), ("TPangles"), 0);
	Clientvariables->Misc.TPKey = ReadInt(("Miscellaneous"), ("TPKey"), 0);
	Clientvariables->Misc.Props = ReadInt(("Miscellaneous"), ("Props"), 0);
	Clientvariables->Misc.Walls = ReadInt(("Miscellaneous"), ("Walls"), 0);

	Clientvariables->Colors.MenuColor[0] = ReadFloat(("Colors"), ("MenuR"), 0.f);
	Clientvariables->Colors.MenuColor[1] = ReadFloat(("Colors"), ("MenuG"), 0.f);
	Clientvariables->Colors.MenuColor[2] = ReadFloat(("Colors"), ("MenuB"), 0.f);

	Clientvariables->Colors.Glow[0] = ReadFloat(("Colors"), ("GlowR"), 0.f);
	Clientvariables->Colors.Glow[1] = ReadFloat(("Colors"), ("GlowG"), 0.f);
	Clientvariables->Colors.Glow[2] = ReadFloat(("Colors"), ("GlowB"), 0.f);

	Clientvariables->Colors.Skeletons[0] = ReadFloat(("Colors"), ("BonesR"), 0.f);
	Clientvariables->Colors.Skeletons[1] = ReadFloat(("Colors"), ("BonesG"), 0.f);
	Clientvariables->Colors.Skeletons[2] = ReadFloat(("Colors"), ("BonesB"), 0.f);

	Clientvariables->Colors.BoundingBox[0] = ReadFloat(("Colors"), ("BoxR"), 0.f);
	Clientvariables->Colors.BoundingBox[1] = ReadFloat(("Colors"), ("BoxG"), 0.f);
	Clientvariables->Colors.BoundingBox[2] = ReadFloat(("Colors"), ("BoxB"), 0.f);

	Clientvariables->Colors.PlayerChams[0] = ReadFloat(("Colors"), ("ChamsPlayerR"), 0.f);
	Clientvariables->Colors.PlayerChams[1] = ReadFloat(("Colors"), ("ChamsPlayerG"), 0.f);
	Clientvariables->Colors.PlayerChams[2] = ReadFloat(("Colors"), ("ChamsPlayerB"), 0.f);

	Clientvariables->Colors.PlayerChamsWall[0] = ReadFloat(("Colors"), ("ChamsNONR"), 0.f);
	Clientvariables->Colors.PlayerChamsWall[1] = ReadFloat(("Colors"), ("ChamsNONG"), 0.f);
	Clientvariables->Colors.PlayerChamsWall[2] = ReadFloat(("Colors"), ("ChamsNONB"), 0.f);

	Clientvariables->Colors.Bulletracer[0] = ReadFloat(("Colors"), ("TracersR"), 0.f);
	Clientvariables->Colors.Bulletracer[1] = ReadFloat(("Colors"), ("TracersG"), 0.f);
	Clientvariables->Colors.Bulletracer[2] = ReadFloat(("Colors"), ("TracersB"), 0.f);
}

void Config::Reset()
{
	/*-----------------------------------------------------------------------------------------*/
	Clientvariables->Ragebot.AimbotSelection =  0;
	Clientvariables->Ragebot.AutomaticFire = 0;
	Clientvariables->Ragebot.AutomaticResolver = 0;
	Clientvariables->Ragebot.ResolverStepAngle = 0;
	Clientvariables->Ragebot.AutomaticScope = 0;
	Clientvariables->Ragebot.Autowall = 0;
	Clientvariables->Ragebot.AutowallHitscan = 0;
	Clientvariables->Ragebot.EnableAimbot = 0;
	Clientvariables->Ragebot.FriendlyFire = 0;
	Clientvariables->Ragebot.Hitbox = 0;
	Clientvariables->Ragebot.Hitchance = 0;
	Clientvariables->Ragebot.Hitscan = 0;
	Clientvariables->Ragebot.Mindamage = 1;
	Clientvariables->Ragebot.Minhitchance = 0;
	Clientvariables->Ragebot.Multipoint = 0;
	Clientvariables->Ragebot.NoRecoil = 0;
	Clientvariables->Ragebot.NoSpread = 0;
	Clientvariables->Ragebot.Headscale = 0;
	Clientvariables->Ragebot.Bodyscale = 0;
	Clientvariables->Ragebot.PositionAdjustment = 0;
	Clientvariables->Ragebot.Preferbodyaim = 0;
	Clientvariables->Ragebot.SilentAimbot = 0;

	Clientvariables->Ragebot.Head = 0;
	Clientvariables->Ragebot.Neck = 0;
	Clientvariables->Ragebot.NeckFull = 0;
	Clientvariables->Ragebot.Body = 0;
	Clientvariables->Ragebot.BodyFull = 0;
	Clientvariables->Ragebot.Arms = 0;
	Clientvariables->Ragebot.ArmsFull = 0;
	Clientvariables->Ragebot.Legs = 0;
	Clientvariables->Ragebot.LegsFull = 0;
	//Antiaims load
	Clientvariables->Antiaim.Enable = 0;
    Clientvariables->Antiaim.Pitch = 0;
	Clientvariables->Antiaim.Yaw = 0;
	Clientvariables->Antiaim.Left = 0;
	Clientvariables->Antiaim.Right = 0;
	Clientvariables->Antiaim.LBYDelta = 0;

	Clientvariables->Antiaim.FakeLag.Type = 0;
	Clientvariables->Antiaim.FakeLag.Amount = 0;
	Clientvariables->Antiaim.FakeLag.FakeLagInAirOnly = 0;

	Clientvariables->Visuals.Ammo = 0;
	Clientvariables->Visuals.Armor = 0;
	Clientvariables->Visuals.Bomb = 0;
	Clientvariables->Visuals.Bones = 0;
	Clientvariables->Visuals.BoundingBox = 0;
	Clientvariables->Visuals.BulletTracers = 0;
	Clientvariables->Visuals.ChamsEnable = 0;
	Clientvariables->Visuals.Chamsenemyonly = 0;
	Clientvariables->Visuals.ChamsPlayer = 0;
	Clientvariables->Visuals.ChamsPlayerWall = 0;
	Clientvariables->Visuals.ChamsStyle = 0;
	Clientvariables->Visuals.Crosshair = 0;
	Clientvariables->Visuals.Dlight = 0;
	Clientvariables->Visuals.DroppedWeapons = 0;
	Clientvariables->Visuals.EnemyOnly = 0;
	Clientvariables->Visuals.EspEnable = 0;
	Clientvariables->Visuals.FakeAngleGhost = 0;
	Clientvariables->Visuals.Glow = 0;
	Clientvariables->Visuals.GrenadePrediction = 0;
	Clientvariables->Visuals.Health = 0;
	Clientvariables->Visuals.Hitmarker = 0;
	Clientvariables->Visuals.Hostage = 0;
	Clientvariables->Visuals.LineofSight = 0;
	Clientvariables->Visuals.LocalPlayer = 0;
	Clientvariables->Visuals.Name = 0;
	Clientvariables->Visuals.nightmode = 0;
	Clientvariables->Visuals.FlashbangAlpha = 255;
	Clientvariables->Visuals.Noscope = 0;
	Clientvariables->Visuals.Nosmoke = 0;
	Clientvariables->Visuals.Novisrevoil = 0;
	Clientvariables->Visuals.Rank = 0;
	Clientvariables->Visuals.RecoilCrosshair = 0;
	Clientvariables->Visuals.RemoveParticles = 0;
	Clientvariables->Visuals.Skybox = 0;
	Clientvariables->Visuals.SnapLines = 0;
	Clientvariables->Visuals.TRange = 100;
	Clientvariables->Visuals.Spectators = 0;
	Clientvariables->Visuals.SpreadCrosshair = 0;
	Clientvariables->Visuals.ThrownNades = 0;
	Clientvariables->Visuals.Wins = 0;
	Clientvariables->Colors.gla = 0;

	Clientvariables->Misc.AntiUT = 0;
	Clientvariables->Misc.AutoAccept = 0;
	Clientvariables->Misc.AutoJump = 0;
	Clientvariables->Misc.AutoStrafe = 0;
	Clientvariables->Misc.Clantag = 0;
	Clientvariables->Misc.ConfigSelection = 0;
	Clientvariables->Misc.MenuKey = VK_INSERT;
	Clientvariables->Misc.PlayerFOV = 0;
	Clientvariables->Misc.PlayerViewmodel = 0;
	Clientvariables->Misc.Prespeed = 0;
	Clientvariables->Misc.PrespeedKey = 0;
	Clientvariables->Misc.Retrack = 0;
	Clientvariables->Misc.TPangles = 0;
	Clientvariables->Misc.TPKey = 0;

	Clientvariables->Misc.Props = 255;
	Clientvariables->Misc.Walls = 255;

	Clientvariables->Colors.MenuColor[0] = ReadFloat(("Colors"), ("MenuR"), 0.f);
	Clientvariables->Colors.MenuColor[1] = ReadFloat(("Colors"), ("MenuG"), 0.f);
	Clientvariables->Colors.MenuColor[2] = ReadFloat(("Colors"), ("MenuB"), 0.f);

	Clientvariables->Colors.Glow[0] = ReadFloat(("Colors"), ("GlowR"), 0.f);
	Clientvariables->Colors.Glow[1] = ReadFloat(("Colors"), ("GlowG"), 0.f);
	Clientvariables->Colors.Glow[2] = ReadFloat(("Colors"), ("GlowB"), 0.f);

	Clientvariables->Colors.gla = ReadFloat(("Colors"), ("GlowA"), 0.f);

	Clientvariables->Colors.Skeletons[0] = ReadFloat(("Colors"), ("BonesR"), 0.f);
	Clientvariables->Colors.Skeletons[1] = ReadFloat(("Colors"), ("BonesG"), 0.f);
	Clientvariables->Colors.Skeletons[2] = ReadFloat(("Colors"), ("BonesB"), 0.f);

	Clientvariables->Colors.BoundingBox[0] = ReadFloat(("Colors"), ("BoxR"), 0.f);
	Clientvariables->Colors.BoundingBox[1] = ReadFloat(("Colors"), ("BoxG"), 0.f);
	Clientvariables->Colors.BoundingBox[2] = ReadFloat(("Colors"), ("BoxB"), 0.f);

	Clientvariables->Colors.PlayerChams[0] = ReadFloat(("Colors"), ("ChamsPlayerR"), 0.f);
	Clientvariables->Colors.PlayerChams[1] = ReadFloat(("Colors"), ("ChamsPlayerG"), 0.f);
	Clientvariables->Colors.PlayerChams[2] = ReadFloat(("Colors"), ("ChamsPlayerB"), 0.f);

	Clientvariables->Colors.PlayerChamsWall[0] = ReadFloat(("Colors"), ("ChamsNONR"), 0.f);
	Clientvariables->Colors.PlayerChamsWall[1] = ReadFloat(("Colors"), ("ChamsNONG"), 0.f);
	Clientvariables->Colors.PlayerChamsWall[2] = ReadFloat(("Colors"), ("ChamsNONB"), 0.f);

	Clientvariables->Colors.Bulletracer[0] = ReadFloat(("Colors"), ("TracersR"), 0.f);
	Clientvariables->Colors.Bulletracer[1] = ReadFloat(("Colors"), ("TracersG"), 0.f);
	Clientvariables->Colors.Bulletracer[2] = ReadFloat(("Colors"), ("TracersB"), 0.f);
}
