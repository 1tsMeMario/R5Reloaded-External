#pragma once

struct globals
{
	// System
	bool Active = false;
	int MaxCount = 75;

	// AimBot
	bool AimBot = true;
	bool AimAtTeam = false;
	bool VisCheck = true;
	bool NoSway = true;
	int AimBone = 3;
	int PredictVal = 225;

	float AimFov = 150.f;
	float Smooth = 1.f;
	float AimDist = 150.f;

	// Misc
	bool SpeedHx = false;
	float Speed = 2.f;
	int DownTime = 0;
};

extern globals g;