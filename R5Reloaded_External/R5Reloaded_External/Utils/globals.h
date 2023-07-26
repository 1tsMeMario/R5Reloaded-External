#pragma once
#include <string>

struct globals
{
	//Auth System
	std::string Application = "";
	std::string Version = "1.0.0";


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

	float AimFov = 500.f;
	float Smooth = 1.f;
	float AimDist = 150.f;

	// Misc
	bool SpeedHx = false;
	float Speed = 2.f;
	int DownTime = 0;
};

extern globals g;