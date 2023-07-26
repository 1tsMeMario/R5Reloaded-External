#pragma once
#include "overlay.h"

class Cheat
{
public:
	bool CoolDown = false;
	int CoolDownTime = 0;

	void AimBot();
	void SpeedHack();
};