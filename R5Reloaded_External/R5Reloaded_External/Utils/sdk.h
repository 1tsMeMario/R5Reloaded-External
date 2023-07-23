#pragma once
#include "SimpleMath\SimpleMath.h"
#include "Memory\Memory.h"
using namespace DirectX::SimpleMath;

// SDK(èŒ)
// ç\ë¢ëÃ

// Ç∆ÇËÇ†Ç¶Ç∏égÇ¶ÇÍÇŒÇ¢Ç¢ÇÃÇ≈
struct CPlayer
{
	Vector3 m_vecAbsVelocity;
	Vector3 m_localOrigin;
	char z1[0x18];
	int m_shieldHealth;
	int m_shieldHealthMax;
	char z2[0x268];
	int m_iHealth;
	char z3[0xC];
	int m_iTeamNum;
	char z4[0x34];
	Vector3 m_localAngle;
	char z5[0xDC];
	int m_iMaxHealth;
};

// offsets
namespace offset
{
	// Main
	constexpr auto dwEntityList = 0x1F33F78;
	constexpr auto dwLocalPlayer = 0x22E93F8;
	constexpr auto ViewMatrix = 0x1A93D0;
	constexpr auto ViewRender= 0xD4138F0;
	constexpr auto g_GameSpeedHack = 0x1843340;

	// Player
	constexpr auto m_vecAbsVelocity		= 0x140;	// Vector3
	constexpr auto m_localOrigin		= 0x14C;	// Vector3
	constexpr auto m_shieldHealth		= 0x170;	// int
	constexpr auto m_shieldHealthMax	= 0x174;	// int
	constexpr auto m_iHealth			= 0x3E0;	// int
	constexpr auto m_iTeamNum			= 0x3F0;	// int
	constexpr auto m_localAngle			= 0x428;	// Vector3
	constexpr auto m_iMaxHealth			= 0x510;	// int
	constexpr auto m_lifeState			= 0x730;	// int

	constexpr auto m_nSkin = 0xe48;
	constexpr auto s_BoneMatrix = 0xEE0;
	constexpr auto camera_origin = 0x1b68;

	// Weapon
	constexpr auto m_latestPrimaryWeapons = 0x1704;
}

