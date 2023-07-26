#include "cheat.h"

extern Memory m;
extern Overlay* v;
float VisPlayer[16000] = {};

uint64_t FindBestTarget();
void NormalizeAngles(Vector3& angle);
Vector3 CalcAngle(const Vector3& src, const Vector3& dst);
Vector3 GetPredict(uint64_t Entity, float dis, float BulletSpeed);

extern uint64_t GetEntityById(uint64_t GameBaseAddr, int Ent);
extern bool WorldToScreen(Vector3 from, float* m_vMatrix, int targetWidth, int targetHeight, Vector2& to);
extern Vector3 GetEntityBonePosition(uintptr_t ent, int BoneId, Vector3 BasePosition);

void Cheat::AimBot()
{
	// replace AimKey
	if (IsKeyDown(VK_RBUTTON) or IsKeyDown(VK_LBUTTON))
	{
		uint64_t Entity = FindBestTarget();
		uint64_t LocalPlayer = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
		Vector3 LocalPos = m.Read<Vector3>(LocalPlayer + offset::m_localOrigin);

		// Pos
		Vector3 EntityPos = m.Read<Vector3>(Entity + offset::m_localOrigin);

		if (EntityPos == Vector3(0.f, 0.f, 0.f))
			return;
		
		// GetPos
		Vector3 LocalHead = m.Read<Vector3>(LocalPlayer + offset::camera_origin);
		Vector3 TargetHead = GetEntityBonePosition(Entity, g.AimBone, EntityPos); // Head / Neck

		// Prediction
		float pDist = ((EntityPos - LocalPos).Length() * 0.01905f);
		TargetHead += GetPredict(Entity, pDist, g.PredictVal);

		uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
		uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
		Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

		// W2S
		Vector2 ScreenPos = {};
		Vector3 EntityOrigin = m.Read<Vector3>(Entity + offset::m_localOrigin);
		WorldToScreen(EntityOrigin, &ViewMatrix._11, (float)v->GameSize.right, (float)v->GameSize.bottom, ScreenPos);

		if (ScreenPos.x != 0.f && ScreenPos.y != 0.f)
		{
			// Angle
			Vector3 AimAngle = CalcAngle(LocalHead, TargetHead);
			Vector3 TempViewAngles = m.Read<Vector3>(LocalPlayer + 0x2188);

			Vector3 Delta = { 0, 0, 0 };

			Vector3 SwayAngle = m.Read<Vector3>(LocalPlayer + 0x2178);

			if (g.NoSway)
			{
				Vector3 Breath = SwayAngle - TempViewAngles;

				if (Breath.x != 0.f || Breath.y != 0.f)
					Delta = (AimAngle - TempViewAngles) - Breath;
			}
			else
			{
				Delta = (AimAngle - TempViewAngles);
			}
			
			// Fov
			Vector2 ScreenMiddle = { (float)v->GameSize.right / 2, (float)v->GameSize.bottom / 2 };
			float fov = abs((ScreenMiddle - ScreenPos).Length());

			if (fov < g.AimFov)
			{
				NormalizeAngles(Delta);

				Vector3 SmoothedAngle = TempViewAngles + Delta / g.Smooth;
				NormalizeAngles(SmoothedAngle);

				if (SmoothedAngle.x != 0.f || SmoothedAngle.y != 0.f)
				{
					// Write Angle
					m.Write<Vector3>(LocalPlayer + 0x2188, SmoothedAngle);
				}
			}
		}
	}
}

void Cheat::SpeedHack()
{
	CoolDownTime = 0;

	if (IsKeyDown(VK_XBUTTON1))
	{
		g.DownTime = 0;

		if (IsKeyDown(VK_XBUTTON1))
		{
			DWORD LimitTime = 0;

			if (g.Speed > 1.f && g.Speed < 3.f)
				LimitTime = 2150;
			else if (g.Speed > 3.f && g.Speed < 4.f)
				LimitTime = 1500;
			else if (g.Speed > 4.f && g.Speed < 5.f)
				LimitTime = 1000;
			else if (g.Speed > 5.f)
				LimitTime = 700;

			bool Limit = false;
			DWORD StartTime = GetTickCount64();
			DWORD elapsed_time = 0;

			while (!Limit)
			{
				if (IsKeyDown(VK_XBUTTON1))
				{
					m.Write<float>(m.BaseAddress + offset::g_GameSpeedHack, g.Speed);

					elapsed_time = GetTickCount64() - StartTime;
					if (elapsed_time > LimitTime)
					{
						Limit = true;
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}
				else
				{
					Limit = true;
				}
			}

			m.Write<float>(m.BaseAddress + offset::g_GameSpeedHack, 1.f);

			g.DownTime = (elapsed_time * g.Speed) / 100;
			g.DownTime += (g.DownTime * 0.275);

			while (g.DownTime > 0)
			{
				g.DownTime--;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
	}
}

uint64_t FindBestTarget()
{
	float fov = 0;
	uint64_t Target = 0;

	// Get LocalPlayer
	uint64_t LocalPlayer = m.Read<uint64_t>(m.BaseAddress + offset::dwLocalPlayer);
	CPlayer lp, * pLocal = &lp;
	lp = m.Read<CPlayer>(LocalPlayer + 0x140);

	uint64_t viewRenderer = m.Read<uint64_t>(m.BaseAddress + offset::ViewRender);
	uint64_t tmpvmx = m.Read<uint64_t>(viewRenderer + offset::ViewMatrix);
	Matrix ViewMatrix = m.Read<Matrix>(tmpvmx);

	// Loop
	for (int i = 0; i < g.MaxCount; i++)
	{
		// Get Entity
		uint64_t ent = GetEntityById(m.BaseAddress, i);

		if (ent == NULL || ent == LocalPlayer)
			continue;

		CPlayer pp, * pEntity = &pp;
		pp = m.Read<CPlayer>(ent + 0x140);

		// Some checks
		if (pEntity->m_iHealth <= 0)
			continue;
		else if (pEntity->m_iTeamNum == 0)
			continue;
		else if (pEntity->m_localOrigin == Vector3(0.f, 0.f, 0.f))
			continue;

		if (g.VisCheck)
		{
			// Vis check
			bool visible = false;
			float LastVisibleTime = m.Read<float>(ent + 0x1754);
			visible = LastVisibleTime > 0.f && LastVisibleTime > VisPlayer[i];
			VisPlayer[i] = LastVisibleTime;

			if (!visible)
				continue;
		}

		// Some Data
		Vector3 LocalHead = m.Read<Vector3>(LocalPlayer + offset::camera_origin);
		Vector3 EntityHead = m.Read<Vector3>(ent + offset::camera_origin);

		// W2S
		Vector2 ScreenPos = {};
		WorldToScreen(pEntity->m_localOrigin, &ViewMatrix._11, (float)v->GameSize.right, (float)v->GameSize.bottom, ScreenPos);

		Vector2 ScreenMiddle = { (float)v->GameSize.right / 2.f, (float)v->GameSize.bottom / 2.f };
		fov = abs((ScreenMiddle - ScreenPos).Length());

		float pDist = ((pEntity->m_localOrigin - pLocal->m_localOrigin).Length() * 0.01905f);

		if (fov < g.AimFov && pDist < g.AimDist)
			Target = ent;
		else
			continue;
	}

	return Target;
}

void NormalizeAngles(Vector3& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

Vector3 CalcAngle(const Vector3& src, const Vector3& dst)
{
	Vector3 angle = Vector3();
	Vector3 delta = Vector3((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / 3.1415926535);
	angle.y = atan(delta.y / delta.x) * (180.0f / 3.1415926535);
	angle.z = 0;
	if (delta.x >= 0.0) angle.y += 180.0f;

	return angle;
}

Vector3 GetPredict(uint64_t Entity, float dis, float BulletSpeed)
{
	Vector3 predict;

	float bulletTime = dis / BulletSpeed;
	Vector3 pd = m.Read<Vector3>(Entity + offset::m_vecAbsVelocity);
	predict.x = pd.x * bulletTime;
	predict.y = pd.y * bulletTime;
	predict.z = (142.5f * 0.5f * (bulletTime * bulletTime));

	return predict;
}