/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

// debug macro
//#define RANDOM_SEED_CALIBRATION 1
//#define CHECKING_NEXT_PRIM_ATTACK_SYNC	1
#define CLIENT_PREDICT_PRIM_ATK	1
#define CLIENT_PREDICT_AIM	1

// util macro
#define PRECACHE_NECESSARY_FILES(x)	PRECACHE_MODEL(x##_VIEW_MODEL);	\
									PRECACHE_MODEL(x##_WORLD_MODEL);	\
									PRECACHE_SOUND(x##_FIRE_SFX)

#ifndef CLIENT_DLL
#define DECLARE_STANDARD_RESET_MODEL_FUNC(x)	void C##x::ResetModel(void)	\
												{	\
													m_pPlayer->pev->viewmodel = MAKE_STRING(x##_VIEW_MODEL);	\
													m_pPlayer->pev->weaponmodel = MAKE_STRING(x##_WORLD_MODEL);	\
												}
#else
#define DECLARE_STANDARD_RESET_MODEL_FUNC(x)	void C##x::ResetModel(void)	\
												{	\
													g_pViewEnt->model = gEngfuncs.CL_LoadModel(x##_VIEW_MODEL, &m_pPlayer->pev->viewmodel);	\
												}
#endif

class CBaseEntity;
class CBasePlayer;
class CWeaponBox;

const float MAX_NORMAL_BATTERY    = 100.0f;
const float MAX_DIST_RELOAD_SOUND = 512.0f;

#define ITEM_FLAG_NONE				0
#define ITEM_FLAG_SELECTONEMPTY     1
#define ITEM_FLAG_NOAUTORELOAD      2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD      8
#define ITEM_FLAG_EXHAUSTIBLE       16 // A player can totally exhaust their ammo supply and lose this weapon

#define WEAPON_IS_ONTARGET          0x40

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP               -1

#define LOUD_GUN_VOLUME             1000
#define NORMAL_GUN_VOLUME           600
#define QUIET_GUN_VOLUME            200

#define BRIGHT_GUN_FLASH            512
#define NORMAL_GUN_FLASH            256
#define DIM_GUN_FLASH               128

#define BIG_EXPLOSION_VOLUME        2048
#define NORMAL_EXPLOSION_VOLUME     1024
#define SMALL_EXPLOSION_VOLUME      512

#define WEAPON_ACTIVITY_VOLUME      64

// spawn flags
#define SF_DETONATE                 BIT(0) // Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges

#define ARMOR_NONE		0.0f	// No armor
#define ARMOR_KEVLAR	1.0f	// Body vest only
#define ARMOR_VESTHELM	2.0f	// Vest and helmet

#ifndef CLIENT_DLL	// client.dll doesn't need these.
struct MULTIDAMAGE
{
	CBaseEntity *pEntity;
	float amount;
	int type;
};
#endif

#include "ammo.h"
#include "weapontype.h"
#include "player_classes.h"
#include <list>

// something can place on your hand.
class CBaseWeapon
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}

protected:	// you can remove weapons only via its manager function.
	CBaseWeapon() noexcept {}
	CBaseWeapon(const CBaseWeapon& s) = default;
	CBaseWeapon(CBaseWeapon&& s) = default;
	CBaseWeapon& operator=(const CBaseWeapon& s) = default;
	CBaseWeapon& operator=(CBaseWeapon&& s) = default;
	virtual ~CBaseWeapon() {}

public:
	static	void			TheWeaponsThink	(void);
	static	CBaseWeapon*	Give(WeaponIdType iId, CBasePlayer* pPlayer = nullptr, int iClip = 0, unsigned bitsFlags = 0);

public:
	static std::list<CBaseWeapon*>	m_lstWeapons;

public:
	WeaponIdType	m_iId					{ WEAPON_NONE };
	const WeaponInfo* m_pItemInfo			{ &g_rgWpnInfo[WEAPON_NONE] };
	const AmmoInfo* m_pAmmoInfo				{ &g_rgAmmoInfo[AMMO_NONE] };
	float			m_flNextPrimaryAttack	{ 0.0f };
	float			m_flNextSecondaryAttack	{ 0.0f };
	float			m_flTimeWeaponIdle		{ 0.0f };
	unsigned		m_bitsFlags				{ 0 };
	int				m_iClip					{ 0 };
	bool			m_bInReload				{ false };
	int				m_iShotsFired			{ 0 };
	float			m_flDecreaseShotsFired	{ 0.0f };
	bool			m_bDirection			{ false };
	float			m_flAccuracy			{ 0.0f };	// TODO: this should be remove later.
	float			m_flLastFire			{ 0.0f };
	AmmoIdType		m_iPrimaryAmmoType		{ AMMO_NONE };			// "primary" ammo index into players m_rgAmmo[]
	AmmoIdType		m_iSecondaryAmmoType	{ AMMO_NONE };		// "secondary" ammo index into players m_rgAmmo[]
	bool			m_bInZoom				{ false };
	RoleTypes		m_iVariation			{ Role_UNASSIGNED };	// weapons suppose to variegate accroading to their owner.
	bool			m_bDelayRecovery		{ false };

	struct	// this structure is for anim push and pop. it save & restore weapon state.
	{
		// on player.
		int		m_iSequence;
		float	m_flNextAttack;
		float	m_flEjectBrass;
		int		m_iShellModelIndex;

		// on weapon.
		float	m_flNextPrimaryAttack;
		float	m_flNextSecondaryAttack;
		float	m_flTimeWeaponIdle;

#ifdef CLIENT_DLL
		// for model. these will be applied on g_pViewEnt.
		float	m_flTimeAnimStarted;
		float	m_flFramerate;
		float	m_flFrame;
#endif
	}
	m_Stack
	{ 0, 0.0f, 0.0f, 0,
		0.0f, 0.0f, 0.0f
#ifndef CLIENT_DLL
	};
#else
		, 0.0f, 0.0f, 0.0f };
#endif

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	EntityHandle<CBasePlayer>	m_pPlayer;		// one of these two must be valid. or this weapon will be removed.
	EntityHandle<CWeaponBox>	m_pWeaponBox;
	int		m_iClientClip		{ 0 };
	int		m_iClientWeaponState{ 0 };
#else
public:	// CL exclusive variables.
	CBasePlayer*	m_pPlayer		{ nullptr };	// local pseudo-player
	float			m_flBlockCheck	{ 0.0f };
	Vector			m_vecBlockOffset{ g_vecZero };
#endif

public:	// basic logic funcs
	virtual void	Think			(void);		// called by PlayerPreThink.
	virtual bool	AddToPlayer		(CBasePlayer* pPlayer);	// should only be called by CBasePlayer::AddPlayerItem();
	virtual bool	Deploy			(void) { return false; }	// called when attempting to take it out.
	virtual void	PostFrame		(void);		// called by PlayerPostThink.
	virtual void	PrimaryAttack	(void) {}	// IN_MOUSE1
	virtual void	SecondaryAttack	(void) {}	// IN_MOUSE2
	virtual void	WeaponIdle		(void) {}	// constantly called when nothing else to do.
	virtual bool	Reload			(void) { return false; }	// you know what it is, right?
	virtual bool	Melee			(void);		// quick knife.
	virtual bool	QuickThrowStart	(EquipmentIdType iId);	// quick grenade (maybe something else in the future?).
	virtual bool	QuickThrowRelease(void);	// triggered when +qtg button released.
	virtual bool	AlterAct		(void) { return false; }	// special use. for instance, XM8 "morph".
	virtual bool	HolsterStart	(void);		// play holster anim, initialize holstering.
	virtual void	Holstered		(void);		// majorlly reset the weapon data. no visual stuff.
	virtual	void	DashStart		(void) { m_bitsFlags |= WPNSTATE_DASHING; }		// called when system thinks it's time to dash.
	virtual void	DashEnd			(void) { m_bitsFlags &= ~WPNSTATE_DASHING; }		// called when system thinks it's time to stop dash.
	virtual bool	Drop			(void **ppWeaponBoxReturned = nullptr);		// called when attempting to drop it on ground. ppWeaponBoxReturned is the CWeaponBox to be returned. (NOT avaliable on client side.)
	virtual bool	Kill			(void);		// called when attempting to remove it from your inventory.

#ifndef CLIENT_DLL
public:	// SV exclusive functions.
	virtual void	UpdateClientData(void);
	virtual void	Precache		(void) {}
#else
public:	// CL xclusive functions.
	virtual	bool	UsingInvertedVMDL(void)		{ return true; }	// by default, original CS/CZ vmdls are inverted displaying.
	virtual int		CalcBodyParam(void)			{ return 0; }		// allow user to varient weapon body.
	virtual	void	UpdateBobParameters(void);						// in which you may set some bob parameters.
#endif

public:	// basic API and behaviour for weapons.
	bool	DefaultDeploy	(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt, float flDeployTime = 0.75f);
	void	DefaultIdle		(int iDashingAnim, int iIdleAnim = 0, float flDashLoop = 20.0f);
	bool	DefaultReload	(int iClipSize, int iAnim, float flTotalDelay, float flSoftDelay = 0.5f);
	bool	DefaultHolster	(int iHolsterAnim, float flHolsterDelay);
	void	DefaultSteelSight(const Vector& vecOfs, int iFOV, float flDriftingSpeed = 10.0f, float flNextSecondaryAttack = 0.3f);
	void	DefaultScopeSight(const Vector& vecOfs, int iFOV, float flEnterScopeDelay = 0.25f, float flFadeFromBlack = 5.0f, float flDriftingSpeed = 10.0f, float flNextSecondaryAttack = 0.3f);
	void	DefaultDashStart(int iEnterAnim, float flEnterTime);
	void	DefaultDashEnd	(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime);
	bool	DefaultSetLHand	(bool bAppear, int iLHandUpAnim, float flLHandUpTime, int iLHandDownAnim, float flLHandDownTime);
	void	DefaultBlock	(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime);
	float	DefaultSpread	(float flBaseline, float flAimingMul, float flDuckingMul, float flWalkingMul, float flJumpingMul);

public:	// util funcs
	inline	bool	IsDead			(void) { return !!(m_bitsFlags & WPNSTATE_DEAD); }
	virtual	float	GetMaxSpeed		(void) { return 260.0f; }
	virtual	bool	AddPrimaryAmmo	(int iCount);	// fill in clip first, then bpammo.
	inline	bool	IsPistol		(void) { return m_pItemInfo->m_iSlot == PISTOL_SLOT; }
	virtual	void	SendWeaponAnim	(int iAnim, bool bSkipLocal = true);
	virtual	void	PlayEmptySound	(void);
	virtual	void	ReloadSound		(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual	bool	CanHolster		(void);	// smells, looks and tastes like a duck...
	virtual	bool	CanDrop			(void) { return true; }
	virtual void	KickBack		(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change);	// recoil
	virtual void	ResetModel		(void) { }	// used after Melee() and QuickThrowRelease().
	virtual bool	SetVariation	(RoleTypes iType) { m_iVariation = iType; return true; }
	virtual bool	SetLeftHand		(bool bAppear) { return false; }
	virtual void	PlayBlockAnim	(void) { }
	virtual float	GetSpread		(void) { return 0.0f; }
};



#define USP_VIEW_MODEL		"models/weapons/v_usp.mdl"
#define USP_WORLD_MODEL		"models/weapons/w_usp.mdl"
#define USP_FIRE_SFX		"weapons/usp/usp_fire.wav"

constexpr float USP_MAX_SPEED       = 250.0f;
constexpr float USP_DAMAGE          = 32;
constexpr float USP_RANGE_MODIFER   = 1.187260896;	// 80% damage @650 inches.
constexpr float USP_DEPLOY_TIME		= 0.34f;
constexpr float USP_RELOAD_TIME		= 1.87f;
constexpr float USP_FIRE_INTERVAL	= 0.15f;
constexpr float	USP_EFFECTIVE_RANGE = 4096.0f;
constexpr int	USP_PENETRATION		= 1;	// 1 means it can't penetrate anything.
constexpr float	USP_SPREAD_BASELINE	= 1.2f;

enum usp_e
{
	USP_IDLE,
	USP_SHOOT1,
	USP_SHOOT2,
	USP_SHOOT3,
	USP_SHOOT_EMPTY,
	USP_RELOAD,
	USP_DRAW,
};

class CUSP : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return USPFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return USP_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void USPFire(float flSpread, float flCycleTime = USP_FIRE_INTERVAL);
};

#define MP5N_VIEW_MODEL		"models/weapons/v_mp5.mdl"
#define MP5N_WORLD_MODEL	"models/weapons/w_mp5.mdl"
#define MP5N_FIRE_SFX		"weapons/mp5/mp5_fire.wav"

constexpr float MP5N_MAX_SPEED     = 250.0f;
constexpr float MP5N_DAMAGE        = 26.0f;
constexpr float MP5N_RANGE_MODIFER = 1.124618621;	// 80% damage @950 inches.
constexpr float MP5N_RELOAD_TIME   = 2.63f;

enum mp5n_e
{
	MP5N_IDLE1,
	MP5N_RELOAD,
	MP5N_DRAW,
	MP5N_SHOOT1,
	MP5N_SHOOT2,
	MP5N_SHOOT3,
};

#define SCARH_VIEW_MODEL	"models/weapons/v_scarh.mdl"
#define SCARH_WORLD_MODEL	"models/weapons/w_scarl.mdl"
#define SCARH_FIRE_SFX		"weapons/SCARH/mk17_shoot.wav"

constexpr float SCARH_MAX_SPEED			= 235.0f;
constexpr float SCARH_DAMAGE			= 54;
constexpr float SCARH_RANGE_MODIFER		= 1.057371263;	// 80% damage @2000 inches.
constexpr float SCARH_DEPLOY_TIME		= 0.97F;
constexpr float SCARH_DRAW_FIRST_TIME	= 2.61F;
constexpr float SCARH_RELOAD_TIME		= 2.6f;
constexpr float SCARH_RELOAD_EMPTY_TIME	= 2.9f;
constexpr float SCARH_CHECK_MAGAZINE_TIME = 3.06F;
constexpr float SCARH_HOLSTER_TIME		= 0.74F;
constexpr float SCARH_DASH_ENTER_TIME	= 0.485F;
constexpr float SCARH_DASH_EXIT_TIME	= 0.485F;
constexpr float SCARH_BLOCK_UP_TIME		= 0.3333f;
constexpr float SCARH_BLOCK_DOWN_TIME	= 0.3939f;
constexpr float SCARH_LHAND_UP_TIME		= 0.5526f;
constexpr float SCARH_LHAND_DOWN_TIME	= 0.4210f;
constexpr float SCARH_RPM				= 550.0f;
constexpr int	SCARH_PENETRATION		= 2;
constexpr float	SCARH_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	SCARH_GUN_VOLUME		= NORMAL_GUN_VOLUME;
constexpr float	SCARH_SPREAD_BASELINE	= 0.15f;

enum scarh_e
{
	SCARH_IDLE,
	SCARH_SHOOT1,
	SCARH_SHOOT2,
	SCARH_SHOOT3,
	SCARH_SHOOT_LAST,
	SCARH_SHOOT_ATTACHMENTS,
	SCARH_M870MCS_PUMP,
	SCARH_RELOAD,
	SCARH_RELOAD_EMPTY,
	SCARH_EGLM_RELOAD,
	SCARH_XM26_RELOAD,
	SCARH_XM26_RELOAD_EMPTY,
	SCARH_M870MCS_RELOAD_START,
	SCARH_M870MCS_RELOAD_FIRST_INSERT,
	SCARH_M870MCS_RELOAD_INSERT,
	SCARH_M870MCS_RELOAD_END,
	SCARH_M870MCS_RELOAD_END_EMPTY,
	SCARH_DRAW_FIRST,
	SCARH_DEPLOY,
	SCARH_JUMP,
	SCARH_CHECK_MAGAZINE,
	SCARH_SWITCH_SELECTOR,
	SCARH_HOLSTER,
	SCARH_BLOCK_UP,
	SCARH_BLOCK_DOWN,
	SCARH_LHAND_UP,
	SCARH_LHAND_DOWN,
	SCARH_DASH_ENTER,
	SCARH_DASHING,
	SCARH_DASH_EXIT,
};

class CSCARH : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return SCARHFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void) { return DefaultIdle(SCARH_DASHING); }
	virtual bool	Reload			(void);
	virtual bool	HolsterStart	(void) { return DefaultHolster(SCARH_HOLSTER, SCARH_HOLSTER_TIME); }
	virtual	void	DashStart		(void) { return DefaultDashStart(SCARH_DASH_ENTER, SCARH_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void) { return DefaultDashEnd(SCARH_DASH_ENTER, SCARH_DASH_ENTER_TIME, SCARH_DASH_EXIT, SCARH_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return SCARH_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear) { return DefaultSetLHand(bAppear, SCARH_LHAND_UP, SCARH_LHAND_UP_TIME, SCARH_LHAND_DOWN, SCARH_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void) { return DefaultBlock(SCARH_BLOCK_UP, SCARH_BLOCK_UP_TIME, SCARH_BLOCK_DOWN, SCARH_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void SCARHFire(float flSpread, float flCycleTime = (60.0f / SCARH_RPM));
};

#define AK47_VIEW_MODEL		"models/weapons/v_ak47.mdl"
#define AK47_WORLD_MODEL	"models/weapons/w_ak47.mdl"
#define AK47_FIRE_SFX		"weapons/ak47/ak47_fire.wav"

constexpr float AK47_MAX_SPEED			= 221.0f;
constexpr float AK47_DAMAGE				= 48;
constexpr float AK47_RANGE_MODIFER		= 1.077217345;	// 80% damage @1500 inches.
constexpr float AK47_RELOAD_TIME		= 2.44f;
constexpr float AK47_RPM				= 600.0f;
constexpr float	AK47_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	AK47_PENETRATION		= 2;
constexpr float	AK47_SPREAD_BASELINE	= 0.4f;

enum ak47_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

class CAK47 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return AK47Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return AK47_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void AK47Fire(float flSpread, float flCycleTime = (60.0f / AK47_RPM));
};

#define XM8_VIEW_MODEL	"models/weapons/v_xm8.mdl"
#define XM8_WORLD_MODEL	"models/weapons/w_xm8.mdl"
#define XM8_FIRE_SFX	"weapons/xm8/xm8_shoot.wav"

constexpr float XM8_MAX_SPEED				= 240.0f;
constexpr float XM8_DAMAGE					= 32.0f;
constexpr float XM8_RANGE_MODIFER			= 1.057371263;	// 80% damage @2000 inches
constexpr float XM8_RELOAD_TIME				= 2.33f;
constexpr float XM8_RELOAD_EMPTY_TIME		= 3.03f;
constexpr float XM8_DRAW_FIRST_TIME			= 1.3F;
constexpr float XM8_DRAW_TIME				= 0.7F;
constexpr float XM8_HOLSTER_TIME			= 0.7F;
constexpr float XM8_CHECKMAG_TIME			= 2.2667F;
constexpr float XM8_DASH_ENTER_TIME			= 0.8F;
constexpr float XM8_DASH_EXIT_TIME			= 0.533F;
constexpr float XM8_TO_SHARPSHOOTER_TIME	= 8.8F;
constexpr float XM8_TO_CARBIN_TIME			= 8.8F;
constexpr float XM8_BLOCK_UP_TIME			= 0.5333f;
constexpr float XM8_BLOCK_DOWN_TIME			= 0.5333f;
constexpr float XM8_LHAND_UP_TIME			= 0.5333f;
constexpr float XM8_LHAND_DOWN_TIME			= 0.5333f;
constexpr float XM8_RPM						= 800.0f;
constexpr int	XM8_PENETRATION				= 2;
constexpr float	XM8_EFFECTIVE_RANGE			= 8192.0f;
constexpr int	XM8_GUN_VOLUME				= NORMAL_GUN_VOLUME;
constexpr float	XM8_SPREAD_BASELINE			= 0.1f;

enum xm8_e
{
	XM8_IDLE = 0,
	XM8_FIRE,
	XM8_FIRE_LAST,
	XM8_FIRE_AIM,
	XM8_FIRE_AIM_LAST,
	XM8_RELOAD,
	XM8_RELOAD_EMPTY,
	XM8_DRAW_FIRST,
	XM8_DRAW,
	XM8_HOLSTER,
	XM8_CHECK_MAGAZINE,
	XM8_SELECTOR_SEMI,
	XM8_SELECTOR_AUTO,
	XM8_BLOCK_UP,
	XM8_BLOCK_DOWN,
	XM8_LHAND_DOWN,
	XM8_LHAND_UP,
	XM8_DASH_ENTER,
	XM8_DASHING,
	XM8_DASH_EXIT,
	XM8_SWITCH_TO_SHARPSHOOTER,
	XM8_SWITCH_TO_CARBINE
};

class CXM8 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(XM8_DASHING); }
	virtual bool	Reload			(void);
	virtual bool	AlterAct		(void);
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(XM8_HOLSTER, XM8_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(XM8_DASH_ENTER, XM8_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(XM8_DASH_ENTER, XM8_DASH_ENTER_TIME, XM8_DASH_EXIT, XM8_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return XM8_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear) { return DefaultSetLHand(bAppear, XM8_LHAND_UP, XM8_LHAND_UP_TIME, XM8_LHAND_DOWN, XM8_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(XM8_BLOCK_UP, XM8_BLOCK_UP_TIME, XM8_BLOCK_DOWN, XM8_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void XM8Fire(float flSpread, float flCycleTime = (60.0f / XM8_RPM));
};

#define AWP_VIEW_MODEL	"models/weapons/v_awp.mdl"
#define AWP_WORLD_MODEL	"models/weapons/w_awp.mdl"
#define AWP_FIRE_SFX	"weapons/l115a3/l115a3_fire.wav"

constexpr float AWP_MAX_SPEED			= 210.0f;
constexpr float AWP_MAX_SPEED_ZOOM		= 150.0f;
constexpr float AWP_DAMAGE				= 125.0f;
constexpr float AWP_RANGE_MODIFER		= 1.047585759;	// 80% damage @2400 inches.
constexpr float AWP_FIRE_INTERVAL		= 1.5f;
constexpr float AWP_FIRE_LAST_INV		= 0.5666f;
constexpr float AWP_TIME_SHELL_EJ		= 0.666F;
constexpr float AWP_TIME_RECHAMBER		= 1.2F;
constexpr float AWP_TIME_REC_SHELL_EJ	= 0.3667F;
constexpr float	AWP_RELOAD_TIME			= 3.566F;
constexpr float	AWP_RELOAD_EMPTY_TIME	= 4.5F;
constexpr float	AWP_RELOAD_EMPTY_SHELL	= 0.4333f;
constexpr float	AWP_DEPLOY_TIME			= 0.733F;
constexpr float	AWP_DRAW_FIRST_TIME		= 1.533F;
constexpr float	AWP_HOLSTER_TIME		= 0.7333F;
constexpr float	AWP_CHECKMAG_TIME		= 2.0333F;
constexpr float	AWP_BLOCK_UP_TIME		= 0.5333F;
constexpr float	AWP_BLOCK_DOWN_TIME		= 0.5333F;
constexpr float	AWP_LHAND_UP_TIME		= 0.7F;
constexpr float	AWP_LHAND_DOWN_TIME		= 0.7F;
constexpr float	AWP_DASH_ENTER_TIME		= 0.4667F;
constexpr float	AWP_DASH_EXIT_TIME		= 0.4667F;
constexpr int	AWP_PENETRATION			= 3;
constexpr float	AWP_EFFECTIVE_RANGE		= 8192.0f;
constexpr int	AWP_GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
constexpr float AWP_SPREAD_BASELINE		= 0.001f;

enum awp_e
{
	AWP_IDLE = 0,
	AWP_SHOOT_REC,
	AWP_RECHAMBER,
	AWP_SHOOT_LAST,
	AWP_RELOAD,
	AWP_RELOAD_EMPTY,
	AWP_DRAW,
	AWP_DRAW_FIRST,
	AWP_HOLSTER,
	AWP_CHECK_MAGAZINE,
	AWP_BLOCK_UP,
	AWP_BLOCK_DOWN,
	AWP_LHAND_UP,
	AWP_LHAND_DOWN,
	AWP_DASH_ENTER,
	AWP_DASHING,
	AWP_DASH_EXIT
};

class CAWP : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_PIN_UNINVOLVED_ANIM = (1 << AWP_IDLE) |
													(1 << AWP_DRAW) |
													(1 << AWP_RELOAD) |	// normal reload could not bypass this rechamber flag.
													(1 << AWP_HOLSTER) |
													(1 << AWP_CHECK_MAGAZINE) |
													(1 << AWP_LHAND_DOWN) | (1 << AWP_LHAND_UP) |
													(1 << AWP_BLOCK_DOWN) | (1 << AWP_BLOCK_UP) |
													(1 << AWP_DASH_ENTER) | (1 << AWP_DASHING) | (1 << AWP_DASH_EXIT);

	// Rechamber acceptable anims.
	static constexpr int BITS_RECHAMBER_ANIM = (1 << AWP_RECHAMBER) | (1 << AWP_SHOOT_REC) | (1 << AWP_RELOAD_EMPTY);

public: // shared new vars.
	float m_flTimeChamberCleared;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void)	{ return DefaultDeploy(AWP_VIEW_MODEL, AWP_WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? AWP_DRAW_FIRST : AWP_DRAW, "rifle", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? AWP_DRAW_FIRST_TIME : AWP_DEPLOY_TIME); }
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void)	{ return DefaultScopeSight(Vector(-6.2f, -2, 1.1f), 25); }
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(AWP_DASHING); }
	virtual bool	Reload			(void);
	virtual bool	HolsterStart	(void);
	virtual	void	DashStart		(void)	{ return DefaultDashStart(AWP_DASH_ENTER, AWP_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(AWP_DASH_ENTER, AWP_DASH_ENTER_TIME, AWP_DASH_EXIT, AWP_DASH_EXIT_TIME); }

public:	// util funcs
	virtual float	GetMaxSpeed		(void);
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, AWP_LHAND_UP, AWP_LHAND_UP_TIME, AWP_LHAND_DOWN, AWP_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(AWP_BLOCK_UP, AWP_BLOCK_UP_TIME, AWP_BLOCK_DOWN, AWP_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new funcs
	void AWPFire(float flSpread, float flCycleTime = AWP_FIRE_INTERVAL);
};

#define DEagle_VIEW_MODEL	"models/weapons/v_deagle.mdl"
#define DEagle_WORLD_MODEL	"models/weapons/w_deagle.mdl"
#define DEagle_FIRE_SFX		"weapons/deagle/deagle_fire.wav"

constexpr float DEAGLE_MAX_SPEED			= 245.0f;
constexpr float DEAGLE_DAMAGE				= 60;
constexpr float DEAGLE_RANGE_MODIFER		= 1.106751211;	// 80% damage @1100 inches.
constexpr float DEAGLE_RELOAD_TIME			= 1.97f;
constexpr float DEAGLE_RELOAD_EMPTY_TIME	= 2.02f;
constexpr float DEAGLE_DRAW_TIME			= 0.7f;
constexpr float DEAGLE_DRAW_FIRST_TIME		= 1.8f;
constexpr float DEAGLE_HOLSTER_TIME			= 0.7f;
constexpr float DEAGLE_CHECKMAG_TIME		= 2.32f;
constexpr float	DEAGLE_BLOCK_UP_TIME		= 0.5333F;
constexpr float	DEAGLE_BLOCK_DOWN_TIME		= 0.5333F;
constexpr float	DEAGLE_LHAND_UP_TIME		= 0.6666F;
constexpr float	DEAGLE_LHAND_DOWN_TIME		= 0.6666F;
constexpr float DEAGLE_DASH_ENTER_TIME		= 0.8667f;
constexpr float DEAGLE_DASH_EXIT_TIME		= 0.3667f;
constexpr float DEAGLE_SH_RELOAD_TIME		= 2.2333f;
constexpr float DEAGLE_SH_RELOAD_EMPTY_TIME	= 2.7f;
constexpr float DEAGLE_SH_DASH_ENTER_TIME	= 0.7f;
constexpr float DEAGLE_SH_DASH_EXIT_TIME	= 0.7f;
constexpr float DEAGLE_FIRE_INTERVAL		= 0.225f;
constexpr int	DEAGLE_PENETRATION			= 2;
constexpr float	DEAGLE_EFFECTIVE_RANGE		= 4096.0f;
constexpr int	DEAGLE_GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
constexpr float	DEAGLE_SPREAD_BASELINE		= 4;

enum deagle_e
{
	DEAGLE_IDLE = 0,
	DEAGLE_SHOOT,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_RELOAD_EMPTY,
	DEAGLE_DRAW,
	DEAGLE_DRAW_FIRST,
	DEAGLE_HOLSTER,
	DEAGLE_CHECK_MAGAZINE,
	DEAGLE_LHAND_DOWN,
	DEAGLE_LHAND_UP,
	DEAGLE_BLOCK_UP,
	DEAGLE_BLOCK_DOWN,
	DEAGLE_DASH_ENTER,
	DEAGLE_DASHING,
	DEAGLE_DASH_EXIT,
	DEAGLE_SH_RELOAD,
	DEAGLE_SH_RELOAD_EMPTY,
	DEAGLE_SH_DASH_ENTER,
	DEAGLE_SH_DASHING,
	DEAGLE_SH_DASH_EXIT,
};

class CDEagle : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM =	(1 << DEAGLE_IDLE) |
												(1 << DEAGLE_DRAW) |
												(1 << DEAGLE_HOLSTER) |
												(1 << DEAGLE_CHECK_MAGAZINE) |
												(1 << DEAGLE_LHAND_DOWN) | (1 << DEAGLE_LHAND_UP) |
												(1 << DEAGLE_BLOCK_DOWN) | (1 << DEAGLE_BLOCK_UP) |
												(1 << DEAGLE_DASH_ENTER) | (1 << DEAGLE_DASHING) | (1 << DEAGLE_DASH_EXIT) |
												(1 << DEAGLE_SH_DASH_ENTER) | (1 << DEAGLE_SH_DASHING) | (1 << DEAGLE_SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return DEagleFire(GetSpread()); }
	virtual void	SecondaryAttack	(void)	{ return DefaultSteelSight(Vector(-1.905f, -2, 1.1f), 85); }
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(DEAGLE_DASHING); }
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(DEAGLE_HOLSTER, DEAGLE_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(DEAGLE_DASH_ENTER, DEAGLE_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(DEAGLE_DASH_ENTER, DEAGLE_DASH_ENTER_TIME, DEAGLE_DASH_EXIT, DEAGLE_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return DEAGLE_MAX_SPEED; }
	virtual void	ResetModel		(void);	// declare by marco.
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, DEAGLE_LHAND_UP, DEAGLE_LHAND_UP_TIME, DEAGLE_LHAND_DOWN, DEAGLE_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(DEAGLE_BLOCK_UP, DEAGLE_BLOCK_UP_TIME, DEAGLE_BLOCK_DOWN, DEAGLE_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void DEagleFire(float flSpread, float flCycleTime = DEAGLE_FIRE_INTERVAL);
};

const float FLASHBANG_MAX_SPEED        = 250.0f;
const float FLASHBANG_MAX_SPEED_SHIELD = 180.0f;

enum flashbang_e
{
	FLASHBANG_IDLE,
	FLASHBANG_PULLPIN,
	FLASHBANG_THROW,
	FLASHBANG_DRAW,
};

#define SVD_VIEW_MODEL	"models/weapons/v_svd.mdl"
#define SVD_WORLD_MODEL	"models/weapons/w_svd.mdl"
#define SVD_FIRE_SFX	"weapons/svd/svd_fire.wav"

constexpr float SVD_MAX_SPEED		= 210.0f;
constexpr float SVD_MAX_SPEED_ZOOM	= 150.0f;
constexpr float SVD_DAMAGE			= 115;
constexpr float SVD_RANGE_MODIFER	= 1.077137738;	// 70% damage @2400 inches.
constexpr float SVD_DEPLOY_TIME		= 1.13f;
constexpr float SVD_RELOAD_TIME		= 3.2f;
constexpr float SVD_FIRE_INTERVAL	= 0.25f;
constexpr int	SVD_PENETRATION		= 3;
constexpr float	SVD_EFFECTIVE_RANGE	= 8192.0f;
constexpr float	SVD_SPREAD_BASELINE = 0.0251f;

enum svd_e
{
	SVD_IDLE,
	SVD_SHOOT,
	SVD_SHOOT2,
	SVD_RELOAD,
	SVD_DRAW,
};

class CSVD : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return SVDFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void);
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void)	{ return DefaultSpread(SVD_SPREAD_BASELINE, 0.25f, 0.75f, 2.0f, 5.0f); }

public:	// new functions
	void SVDFire(float flSpread, float flCycleTime = SVD_FIRE_INTERVAL);
};

#define G18C_VIEW_MODEL		"models/weapons/v_glock18.mdl"
#define G18C_WORLD_MODEL	"models/w_glock18.mdl"	// FIXME
#define G18C_FIRE_SFX		"weapons/glock18/glock18_fire.wav"

constexpr float GLOCK18_MAX_SPEED		= 245.0f;
constexpr float GLOCK18_DAMAGE			= 17;
constexpr float GLOCK18_RANGE_MODIFER	= 1.187260896;	// 80% damage @650 inches.
constexpr float GLOCK18_DEPLOY_TIME		= 0.34f;
constexpr float GLOCK18_RELOAD_TIME		= 1.87f;
constexpr float GLOCK18_RPM				= 1200.0f;
constexpr int	GLOCK18_PENETRATION		= 1;
constexpr float	GLOCK18_EFFECTIVE_RANGE	= 4096.0f;
constexpr float	GLOCK18_SPREAD_BASELINE = 0.33f;	// don't mul it my 10 like other pistols. IT'S A DE FACTO SMG.

enum glock18_e
{
	GLOCK18_IDLE1,
	GLOCK18_IDLE2,
	GLOCK18_IDLE3,
	GLOCK18_SHOOT,
	GLOCK18_SHOOT2,
	GLOCK18_SHOOT3,
	GLOCK18_SHOOT_EMPTY,
	GLOCK18_RELOAD,
	GLOCK18_DRAW,
	GLOCK18_HOLSTER,
	GLOCK18_ADD_SILENCER,
	GLOCK18_DRAW2,
	GLOCK18_RELOAD2,
};

class CG18C : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return GLOCK18Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return GLOCK18_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void GLOCK18Fire(float flSpread, float flCycleTime = 60.0f / GLOCK18_RPM);
};

#define THROWABLE_VIEW_MODEL	"models/weapons/v_throwable.mdl"

constexpr float TIME_GR_IDLE_LOOP			= (11.0f / 3.0f);
constexpr float TIME_GR_QT_READY			= 0.86f;
constexpr float TIME_GR_QT_READY_2			= 0.86f;
constexpr float TIME_FB_QT_READY			= 0.96f;
constexpr float TIME_SG_QT_READY			= 0.96f;
constexpr float TIME_QT_THROWING_FAR		= 0.78f;
constexpr float TIME_SP_QT_THROWING_FAR		= 0.25f;
constexpr float TIME_QT_THROWING_LONGBOW	= 0.79f;
constexpr float TIME_SP_QT_THROWING_LB		= 0.18f;
constexpr float TIME_QT_THROWING_SOFT		= 0.79f;
constexpr float TIME_SP_QT_THROWING_SOFT	= 0.21f;

enum throwable_e
{
	GR_IDLE = 0,

	GR_QT_READY = 12,
	GR_QT_READY_2 = 25,

	FB_QT_READY = 38,

	SG_QT_READY = 51,

	QT_THROWING_FAR = 52,
	QT_THROWING_LONGBOW = 53,
	QT_THROWING_SOFT = 54,
};

constexpr float KNIFE_BODYHIT_VOLUME   = 128.0f;
constexpr float KNIFE_WALLHIT_VOLUME   = 512.0f;
constexpr float KNIFE_MAX_SPEED        = 250.0f;
constexpr float KNIFE_MAX_SPEED_SHIELD = 180.0f;
constexpr float KNIFE_QUICK_SLASH_TIME = 0.939f;
constexpr float KNIFE_QUICK_SLASH_DMG  = 55.0f;

enum knife_e
{
	KNIFE_IDLE,
	KNIFE_QUICK_SLASH = 22,	// model from HLMW.
};

namespace BasicKnife
{
#ifndef CLIENT_DLL	// SV exclusive namespace.

	// player currently running this code.
	extern EntityHandle<CBasePlayer>	m_pPlayer;

	// the weapon which calling quick slash.
	extern CBaseWeapon* m_pWeapon;

	void Precache();
	bool Deploy(CBaseWeapon* pWeapon);
	void Swing();
#endif
};

#define MK46_VIEW_MODEL		"models/weapons/v_mk46.mdl"
#define MK46_WORLD_MODEL	"models/weapons/w_mk46.mdl"
#define MK46_FIRE_SFX		"weapons/mk46/mk46_fire.wav"

constexpr float MK46_MAX_SPEED			= 210.0f;
constexpr float MK46_DAMAGE				= 32.0f;
constexpr float MK46_RANGE_MODIFER		= 1.077217345;	// 80% damage @1500 inches.
constexpr float MK46_DEPLOY_TIME		= 1.0F;
constexpr float MK46_DRAW_FIRST_TIME	= 1.68F;
constexpr float MK46_RELOAD_TIME		= 6.767f;
constexpr float MK46_RELOAD_EMPTY_TIME	= 6.232f;
constexpr float MK46_HOLSTER_TIME		= 0.6F;
constexpr float MK46_INSPECTION_TIME	= 1.5556f;
constexpr float	MK46_BLOCK_UP_TIME		= 0.525f;
constexpr float	MK46_BLOCK_DOWN_TIME	= 0.6f;
constexpr float	MK46_LHAND_UP_TIME		= 0.7333f;
constexpr float	MK46_LHAND_DOWN_TIME	= 0.7333f;
constexpr float MK46_DASH_ENTER_TIME	= 0.68F;
constexpr float MK46_DASH_EXIT_TIME		= 0.68F;
constexpr float MK46_RPM				= 750.0f;
constexpr int	MK46_PENETRATION		= 2;
constexpr float	MK46_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	MK46_GUN_VOLUME			= NORMAL_GUN_VOLUME;
constexpr float	MK46_SPREAD_BASELINE	= 0.3f;

enum mk46_e
{
	MK46_IDLE,
	MK46_SHOOT_UNSCOPE,
	MK46_SHOOT1,
	MK46_SHOOT2,
	MK46_SHOOT3,
	MK46_RELOAD_EMPTY,
	MK46_RELOAD,
	MK46_DRAW_FIRST,
	MK46_DRAW,
	MK46_JUMP,
	MK46_HOLSTER,
	MK46_INSPECTION,
	MK46_BLOCK_UP,
	MK46_BLOCK_DOWN,
	MK46_LHAND_UP,
	MK46_LHAND_DOWN,
	MK46_DASH_ENTER,
	MK46_DASHING,
	MK46_DASH_EXIT,
};

class CMK46 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return MK46Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(MK46_DASHING); }
	virtual bool	Reload			(void);
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(MK46_HOLSTER, MK46_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(MK46_DASH_ENTER, MK46_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(MK46_DASH_ENTER, MK46_DASH_ENTER_TIME, MK46_DASH_EXIT, MK46_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return MK46_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, MK46_LHAND_UP, MK46_LHAND_UP_TIME, MK46_LHAND_DOWN, MK46_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(MK46_BLOCK_UP, MK46_BLOCK_UP_TIME, MK46_BLOCK_DOWN, MK46_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void MK46Fire(float flSpread, float flCycleTime = (60.0f / MK46_RPM));
};

#define KSG12_VIEW_MODEL	"models/weapons/v_ksg12.mdl"
#define KSG12_WORLD_MODEL	"models/weapons/w_ksg12.mdl"
#define KSG12_FIRE_SFX		"weapons/ksg12/ksg12_fire.wav"

constexpr float KSG12_MAX_SPEED			= 230.0f;
constexpr float KSG12_DAMAGE			= 20.0f;
constexpr int	KSG12_PROJECTILE_COUNT	= 9;
constexpr float	KSG12_EFFECTIVE_RANGE	= 3000.0f;
constexpr float KSG12_FIRE_INTERVAL		= 0.95f;
constexpr float KSG12_SHELL_EJECT		= 0.66f;
constexpr float KSG12_TIME_START_RELOAD = 0.566f;
constexpr float KSG12_TIME_INSERT		= 0.75f;
constexpr float KSG12_TIME_ADD_AMMO		= 0.3f;
constexpr float KSG12_TIME_AFTER_RELOAD = 1.033f;
const	Vector	KSG12_CONE_VECTOR		= Vector(0.0675f, 0.0675f, 0.0f); // special shotgun spreads
constexpr float KSG12_RANGE_MODIFIER	= 1.172793196; // 80% damage @700 inches.

enum ksg12_e
{
	KSG12_IDLE,
	KSG12_FIRE1,
	KSG12_FIRE2,
	KSG12_INSERT,
	KSG12_AFTER_RELOAD,
	KSG12_START_RELOAD,
	KSG12_DRAW,
	KSG12_HOLSTER,
};

class CKSG12 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:
	bool	m_bAllowNextEmptySound;
	float	m_flNextInsertAnim;
	float	m_flNextAddAmmo;
	bool	m_bSetForceStopReload;

	struct	// shotgun needs to expand these push-pop stuff a little bit.
	{
		float	m_flNextInsertAnim;
		float	m_flNextAddAmmo;
	}
	m_Stack2;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return KSG12_MAX_SPEED; }
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void)	{ return KSG12_CONE_VECTOR.x; }
};

#define M4A1_VIEW_MODEL		"models/weapons/v_m4a1.mdl"
#define M4A1_WORLD_MODEL	"models/weapons/w_m4a1.mdl"
#define M4A1_FIRE_SFX		"weapons/AR15/ar15_shoot.wav"

constexpr float M4A1_MAX_SPEED			= 230.0f;
constexpr float M4A1_DAMAGE				= 32.0f;
constexpr float M4A1_RANGE_MODIFER		= 1.072221173;	// 80% damage @1600 inches.
constexpr float M4A1_DRAW_TIME			= 0.743f;
constexpr float M4A1_DRAW_FIRST_TIME	= 2.45f;
constexpr float M4A1_RELOAD_TIME		= 2.033f;
constexpr float M4A1_RELOAD_EMPTY_TIME	= 2.6f;
constexpr float M4A1_CHECK_MAGAZINE_TIME= 3.06f;
constexpr float M4A1_HOLSTER_TIME		= 0.6f;
constexpr float	M4A1_BLOCK_UP_TIME		= 0.3333f;
constexpr float	M4A1_BLOCK_DOWN_TIME	= 0.3333f;
constexpr float	M4A1_LHAND_UP_TIME		= 0.5526f;
constexpr float	M4A1_LHAND_DOWN_TIME	= 0.4210f;
constexpr float M4A1_DASH_ENTER_TIME	= 0.485f;
constexpr float M4A1_DASH_EXIT_TIME		= 0.485f;
constexpr float M4A1_RPM				= 700.0f;	// 700~950 RPM
constexpr int	M4A1_PENETRATION		= 2;
constexpr float	M4A1_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	M4A1_GUN_VOLUME			= NORMAL_GUN_VOLUME;
constexpr float	M4A1_SPREAD_BASELINE	= 0.14f;

enum m4a1_e
{
	M4A1_IDLE,
	M4A1_SHOOT_BACKWARD,
	M4A1_SHOOT_LEFTWARD,
	M4A1_SHOOT_RIGHTWARD,
	M4A1_SHOOT_ATTACHMENTS,
	M4A1_M870MCS_PUMP,
	M4A1_RELOAD,
	M4A1_RELOAD_EMPTY,
	M4A1_M203_RELOAD,
	M4A1_XM26_RELOAD,
	M4A1_XM26_RELOAD_EMPTY,
	M4A1_M870MCS_RELOAD_START,
	M4A1_M870MCS_RELOAD_FIRST_INSERT,
	M4A1_M870MCS_RELOAD_INSERT,
	M4A1_M870MCS_RELOAD_END,
	M4A1_M870MCS_RELOAD_END_EMPTY,
	M4A1_DRAW_FIRST,
	M4A1_DRAW,
	M4A1_JUMP,
	M4A1_CHECK_MAGAZINE,
	M4A1_SWITCH_SELECTOR,
	M4A1_HOLSTER,
	M4A1_BLOCK_UP,
	M4A1_BLOCK_DOWN,
	M4A1_LHAND_UP,
	M4A1_LHAND_DOWN,
	M4A1_DASH_ENTER,
	M4A1_DASHING,
	M4A1_DASH_EXIT,
};

class CM4A1 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return M4A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(M4A1_DASHING); }
	virtual bool	Reload			(void);
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(M4A1_HOLSTER, M4A1_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(M4A1_DASH_ENTER, M4A1_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(M4A1_DASH_ENTER, M4A1_DASH_ENTER_TIME, M4A1_DASH_EXIT, M4A1_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return M4A1_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, M4A1_LHAND_UP, M4A1_LHAND_UP_TIME, M4A1_LHAND_DOWN, M4A1_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(M4A1_BLOCK_UP, M4A1_BLOCK_UP_TIME, M4A1_BLOCK_DOWN, M4A1_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void M4A1Fire(float flSpread, float flCycleTime = (60.0f / M4A1_RPM));
};

#define PM9_VIEW_MODEL	"models/weapons/v_pm9.mdl"
#define PM9_WORLD_MODEL	"models/weapons/w_pm9.mdl"
#define PM9_FIRE_SFX	"weapons/pm9/pm9_fire.wav"

const float MAC10_MAX_SPEED     = 250.0f;
const float MAC10_DAMAGE        = 29.0f;
const float MAC10_RANGE_MODIFER = 1.149658245;	// 80% damage @800 inches.
const float MAC10_RELOAD_TIME   = 3.15f;

enum mac10_e
{
	MAC10_IDLE1,
	MAC10_RELOAD,
	MAC10_DRAW,
	MAC10_SHOOT1,
	MAC10_SHOOT2,
	MAC10_SHOOT3,
};

#define Anaconda_VIEW_MODEL		"models/weapons/v_anaconda.mdl"
#define Anaconda_WORLD_MODEL	"models/weapons/w_anaconda.mdl"
#define Anaconda_FIRE_SFX		"weapons/anaconda/anaconda_fire.wav"

constexpr float ANACONDA_MAX_SPEED			= 250.0f;
constexpr float ANACONDA_DAMAGE				= 55;
constexpr float ANACONDA_RANGE_MODIFER		= 1.052022439;	// 80% damage @2200 inches.
constexpr float ANACONDA_RELOAD_TIME		= 2.68f;
constexpr float ANACONDA_DEPLOY_TIME		= 0.367f;	// this gun has a extremely short deploy time.
constexpr float ANACONDA_FIRE_INTERVAL		= 0.15f;
constexpr int	ANACONDA_PENETRATION		= 1;
constexpr float	ANACONDA_EFFECTIVE_RANGE	= 4096.0f;
constexpr float ANACONDA_SPREAD_BASELINE	= 0.24;

enum anaconda_e
{
	ANACONDA_IDLE,
	ANACONDA_SHOOT1,
	ANACONDA_SHOOT2,
	ANACONDA_SHOOT3,
	ANACONDA_SHOOT_EMPTY,
	ANACONDA_RELOAD,
	ANACONDA_DRAW,
};

class CAnaconda : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive variables.
	float m_flShellRain;

public:	// we need some CL-exclusive VFX.
	virtual void	Think			(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return AnacondaFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return ANACONDA_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void AnacondaFire(float flSpread, float flCycleTime = ANACONDA_FIRE_INTERVAL);
};

#define P90_VIEW_MODEL	"models/weapons/v_p90.mdl"
#define P90_WORLD_MODEL	"models/weapons/w_p90.mdl"
#define P90_FIRE_SFX	"weapons/p90/p90_fire.wav"

const float P90_MAX_SPEED     = 245.0f;
const float P90_DAMAGE        = 24;
const float P90_RANGE_MODIFER = 1.106751211;	// 80% damage @1100 inches.
const float P90_RELOAD_TIME   = 3.4f;

enum p90_e
{
	P90_IDLE1,
	P90_RELOAD,
	P90_DRAW,
	P90_SHOOT1,
	P90_SHOOT2,
	P90_SHOOT3,
};

#define M200_VIEW_MODEL		"models/weapons/v_m200.mdl"
#define M200_WORLD_MODEL	"models/weapons/w_m200.mdl"
#define M200_FIRE_SFX		"weapons/m200/m200_fire.wav"

const float SCOUT_MAX_SPEED      = 260.0f;
const float SCOUT_MAX_SPEED_ZOOM = 220.0f;
const float SCOUT_DAMAGE         = 100;
const float SCOUT_RANGE_MODIFER  = 1.061766218;	// 75% damage @2400 inches.
const float SCOUT_RELOAD_TIME    = 2.0f;

enum scout_e
{
	SCOUT_IDLE,
	SCOUT_SHOOT,
	SCOUT_SHOOT2,
	SCOUT_RELOAD,
	SCOUT_DRAW,
};

const float SMOKEGRENADE_MAX_SPEED        = 250.0f;
const float SMOKEGRENADE_MAX_SPEED_SHIELD = 180.0f;

enum smokegrenade_e
{
	SMOKEGRENADE_IDLE,
	SMOKEGRENADE_PINPULL,
	SMOKEGRENADE_THROW,
	SMOKEGRENADE_DRAW,
};

#define MP7A1_VIEW_MODEL	"models/weapons/v_mp7a1.mdl"
#define MP7A1_WORLD_MODEL	"models/weapons/w_mp7a1.mdl"
#define MP7A1_FIRE_SFX		"weapons/mp7a1/mp7a1_fire.wav"

constexpr float MP7A1_MAX_SPEED			= 250.0f;
constexpr float MP7A1_DAMAGE			= 22;
constexpr float MP7A1_RANGE_MODIFER		= 1.106751211;	// 80% damage @1100 inches.
constexpr float MP7A1_RELOAD_TIME		= 2.575f;
constexpr float MP7A1_RPM				= 850.0f;
constexpr int	MP7A1_PENETRATION		= 1;
constexpr float	MP7A1_EFFECTIVE_RANGE	= 8192.0f;
constexpr float MP7A1_SPREAD_BASELINE	= 0.2f;

enum mp7a1_e
{
	MP7A1_IDLE1,
	MP7A1_RELOAD,
	MP7A1_DRAW,
	MP7A1_SHOOT1,
	MP7A1_SHOOT2,
	MP7A1_SHOOT3,
};

class CMP7A1 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return MP7A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return MP7A1_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void MP7A1Fire(float flSpread, float flCycleTime = (60.0f / MP7A1_RPM));
};

#define M1014_VIEW_MODEL	"models/weapons/v_m1014.mdl"
#define M1014_WORLD_MODEL	"models/weapons/w_m1014.mdl"
#define M1014_FIRE_SFX		"weapons/m1014/m1014_fire.wav"

constexpr float M1014_MAX_SPEED					= 240.0f;
constexpr float M1014_DAMAGE					= 20.0f;
constexpr int	M1014_PROJECTILE_COUNT			= 6;
constexpr float	M1014_EFFECTIVE_RANGE			= 3048.0f;
constexpr float M1014_FIRE_INTERVAL				= 0.25f;
constexpr float M1014_TIME_START_RELOAD			= 0.8f;
constexpr float M1014_TIME_START_RELOAD_FIRST	= 2.633f;
constexpr float M1014_TIME_INSERT				= 0.8f;
constexpr float M1014_TIME_ADD_AMMO				= 0.485f;
constexpr float M1014_TIME_ADD_AMMO_FIRST		= 1.433f;
constexpr float M1014_TIME_INSERT_SFX			= 0.27f;
constexpr float M1014_TIME_SIDELOAD_SFX			= 1.0f;
constexpr float M1014_TIME_AFTER_RELOAD			= 0.8f;
constexpr float M1014_DRAW_FIRST_TIME			= 1.9f;
constexpr float M1014_DRAW_TIME					= 0.833f;
constexpr float M1014_HOLSTER_TIME				= 0.833f;
constexpr float M1014_CHECKMAG_TIME				= 2.3f;
constexpr float M1014_INSPECTION_TIME			= 1.8f;
constexpr float M1014_BLOCK_UP_TIME				= 0.366F;
constexpr float M1014_BLOCK_DOWN_TIME			= 0.366F;
constexpr float M1014_LHAND_UP_TIME				= 0.7f;
constexpr float M1014_LHAND_DOWN_TIME			= 0.7f;
constexpr float M1014_DASH_ENTER_TIME			= 0.833F;
constexpr float M1014_DASH_EXIT_TIME			= 0.366F;
const	 Vector M1014_CONE_VECTOR				= Vector(0.0725, 0.0725, 0.0); // special shotgun spreads
constexpr int	M1014_GUN_VOLUME				= LOUD_GUN_VOLUME;
constexpr float M1014_RANGE_MODIFIER			= 1.172793196;	// 80% damage @700 inches.

enum m1014_e
{
	M1014_IDLE,
	M1014_SHOOT,
	M1014_SHOOT_LAST,
	M1014_AIM_SHOOT,
	M1014_AIM_SHOOT_LAST,
	M1014_START_RELOAD,
	M1014_START_RELOAD_FIRST,
	M1014_INSERT,
	M1014_AFTER_RELOAD,
	M1014_DRAW_FIRST,
	M1014_DRAW,
	M1014_HOLSTER,
	M1014_CHECKMAG,
	M1014_INSPECTION,
	M1014_BLOCK_UP,
	M1014_BLOCK_DOWN,
	M1014_LHAND_UP,
	M1014_LHAND_DOWN,
	M1014_DASH_ENTER,
	M1014_DASHING,
	M1014_DASH_EXIT,
};

class CM1014 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:
	// Bolt stop available anims.
	static constexpr int BITS_BOLT_STOP_ANIM =	(1 << M1014_IDLE) |
												(1 << M1014_DRAW) |
												(1 << M1014_HOLSTER) |
												(1 << M1014_INSPECTION) |
												(1 << M1014_BLOCK_UP) | (1 << M1014_BLOCK_DOWN) |
												(1 << M1014_LHAND_UP) | (1 << M1014_LHAND_DOWN) |
												(1 << M1014_DASH_ENTER) | (1 << M1014_DASHING) | (1 << M1014_DASH_EXIT);


public:
	bool	m_bAllowNextEmptySound;
	float	m_flNextInsertAnim;
	float	m_flNextAddAmmo;
	float	m_flNextInsertionSFX;
	bool	m_bSetForceStopReload;
	bool	m_bStartFromEmpty;

	struct	// shotgun needs to expand these push-pop stuff a little bit.
	{
		float	m_flNextInsertAnim;
		float	m_flNextAddAmmo;
		float	m_flNextInsertionSFX;
	}
	m_Stack2;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(M1014_DASHING); }
	virtual	bool	Reload			(void);
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(M1014_HOLSTER, M1014_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(M1014_DASH_ENTER, M1014_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(M1014_DASH_ENTER, M1014_DASH_ENTER_TIME, M1014_DASH_EXIT, M1014_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return M1014_MAX_SPEED; }
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear) { return DefaultSetLHand(bAppear, M1014_LHAND_UP, M1014_LHAND_UP_TIME, M1014_LHAND_DOWN, M1014_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(M1014_BLOCK_UP, M1014_BLOCK_UP_TIME, M1014_BLOCK_DOWN, M1014_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void)	{ return M1014_CONE_VECTOR.x; }
};

#define M45A1_VIEW_MODEL	"models/weapons/v_m45a1.mdl"
#define M45A1_WORLD_MODEL	"models/w_elite.mdl"	// FIXME, BUGBUG
#define M45A1_FIRE_SFX		"weapons/m45a1/m45a1_fire.wav"
#define M45A1_FIRE_SFX_SIL	"weapons/m45a1/m45a1_fire_sil.wav"

constexpr float M45A1_MAX_SPEED				= 250.0f;
constexpr float M45A1_RELOAD_EMPYT_TIME		= 2.6f;
constexpr float M45A1_RELOAD_TIME			= 2.166f;
constexpr float M45A1_DRAW_FIRST_TIME		= 1.067F;
constexpr float M45A1_DRAW_TIME				= 0.7F;
constexpr float M45A1_HOLSTER_TIME			= 0.5F;
constexpr float M45A1_DASH_ENTER_TIME		= 0.5333F;
constexpr float M45A1_DASH_EXIT_TIME		= 0.5333F;
constexpr float	M45A1_BLOCK_UP_TIME			= 0.4f;
constexpr float	M45A1_BLOCK_DOWN_TIME		= 0.4333f;
constexpr float	M45A1_LHAND_UP_TIME			= 0.4f;
constexpr float	M45A1_LHAND_DOWN_TIME		= 0.3333f;
constexpr float M45A1_SH_RELOAD_EMPTY_TIME	= 2.833F;
constexpr float M45A1_SH_RELOAD_TIME		= 2.1667F;
constexpr float M45A1_SH_DASH_ENTER_TIME	= 0.33333F;
constexpr float M45A1_SH_DASH_EXIT_TIME		= 0.65F;
constexpr float M45A1_CHECK_MAGAZINE_TIME	= 2.06667F;
constexpr float M45A1_DAMAGE				= 36.0f;
constexpr float M45A1_RANGE_MODIFER			= 1.131980371;	// 80% damage @900 inches.
constexpr float M45A1_FIRE_INTERVAL			= 0.2f;
constexpr int	M45A1_PENETRATION			= 1;
constexpr float	M45A1_EFFECTIVE_RANGE		= 4096.0f;
constexpr int	M45A1_GUN_VOLUME			= NORMAL_GUN_VOLUME;
constexpr float M45A1_SPREAD_BASELINE		= 0.9f;

enum m45a1_e
{
	M45A1_IDLE = 0,
	M45A1_SHOOT,
	M45A1_SHOOT_LAST,
	M45A1_AIM_SHOOT_A,
	M45A1_AIM_SHOOT_B,
	M45A1_AIM_SHOOT_LAST,
	M45A1_RELOAD_EMPTY,
	M45A1_RELOAD,
	M45A1_DRAW_FIRST,
	M45A1_DRAW,
	M45A1_JUMP,
	M45A1_HOLSTER,
	M45A1_BLOCK_UP,
	M45A1_BLOCK_DOWN,
	M45A1_LHAND_DOWN,
	M45A1_LHAND_UP,
	M45A1_DASH_ENTER,
	M45A1_DASHING,
	M45A1_DASH_EXIT,
	M45A1_SH_RELOAD_EMPTY,
	M45A1_SH_RELOAD,
	M45A1_SH_DASH_ENTER,
	M45A1_SH_DASHING,
	M45A1_SH_DASH_EXIT,
	M45A1_CHECK_MAGAZINE
};

class CM45A1 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM = (1 << M45A1_IDLE) |
												(1 << M45A1_DRAW) |
												(1 << M45A1_HOLSTER) |
												(1 << M45A1_CHECK_MAGAZINE) |
												(1 << M45A1_LHAND_DOWN) | (1 << M45A1_LHAND_UP) |
												(1 << M45A1_BLOCK_DOWN) | (1 << M45A1_BLOCK_UP) |
												(1 << M45A1_DASH_ENTER) | (1 << M45A1_DASHING) | (1 << M45A1_DASH_EXIT) |
												(1 << M45A1_SH_DASH_ENTER) | (1 << M45A1_SH_DASHING) | (1 << M45A1_SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return M45A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(M45A1_DASHING); }
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(M45A1_HOLSTER, M45A1_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(M45A1_DASH_ENTER, M45A1_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(M45A1_DASH_ENTER, M45A1_DASH_ENTER_TIME, M45A1_DASH_EXIT, M45A1_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return M45A1_MAX_SPEED; }
	virtual void	ResetModel		(void);	// declare by marco.
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, M45A1_LHAND_UP, M45A1_LHAND_UP_TIME, M45A1_LHAND_DOWN, M45A1_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(M45A1_BLOCK_UP, M45A1_BLOCK_UP_TIME, M45A1_BLOCK_DOWN, M45A1_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void M45A1Fire(float flSpread, float flCycleTime = M45A1_FIRE_INTERVAL);
};

#define FN57_VIEW_MODEL		"models/weapons/v_fiveseven.mdl"
#define FN57_WORLD_MODEL	"models/weapons/w_fiveseven.mdl"
#define FN57_FIRE_SFX		"weapons/fiveseven/fiveseven_fire.wav"
#define FN57_FIRE_SIL_SFX	"weapons/fiveseven/fiveseven_fire-sil.wav"

constexpr float FIVESEVEN_MAX_SPEED			= 250.0f;
constexpr float FIVESEVEN_DAMAGE			= 24;
constexpr float FIVESEVEN_RANGE_MODIFER		= 1.149658245;	// 80% damage @800 inches.
constexpr float FIVESEVEN_RELOAD_TIME		= 1.94F;
constexpr float FIVESEVEN_RELOAD_EMPTY_TIME	= 1.86F;
constexpr float FIVESEVEN_DRAW_FIRST_TIME	= 1.56F;
constexpr float FIVESEVEN_DRAW_TIME			= 0.7F;
constexpr float FIVESEVEN_HOLSTER_TIME		= 0.7F;
constexpr float FIVESEVEN_CHECKMAG_TIME		= 2.033F;
constexpr float FIVESEVEN_DASH_ENTER_TIME	= 0.8F;
constexpr float FIVESEVEN_DASH_EXIT_TIME	= 0.37F;
constexpr float	FIVESEVEN_BLOCK_UP_TIME		= 0.4666f;
constexpr float	FIVESEVEN_BLOCK_DOWN_TIME	= 0.4666f;
constexpr float	FIVESEVEN_LHAND_UP_TIME		= 0.2666f;
constexpr float	FIVESEVEN_LHAND_DOWN_TIME	= 0.2666f;
constexpr float FIVESEVEN_FIRE_INTERVAL		= 0.132f;
constexpr float	FIVESEVEN_EFFECTIVE_RANGE	= 4096.0f;
constexpr int	FIVESEVEN_PENETRATION		= 1;
constexpr int	FIVESEVEN_GUN_VOLUME		= NORMAL_GUN_VOLUME;
constexpr float FIVESEVEN_SPREAD_BASELINE	= 0.4f;

enum fiveseven_e
{
	FIVESEVEN_IDLE = 0,
	FIVESEVEN_SHOOT,
	FIVESEVEN_SHOOT_LAST,
	FIVESEVEN_AIM_SHOOT,
	FIVESEVEN_AIM_SHOOT_LAST,
	FIVESEVEN_RELOAD,
	FIVESEVEN_RELOAD_EMPTY,
	FIVESEVEN_DRAW,
	FIVESEVEN_DRAW_FIRST,
	FIVESEVEN_HOLSTER,
	FIVESEVEN_CHECK_MAGAZINE,
	FIVESEVEN_BLOCK_UP,
	FIVESEVEN_BLOCK_DOWN,
	FIVESEVEN_LHAND_UP,
	FIVESEVEN_LHAND_DOWN,
	FIVESEVEN_DASH_ENTER,
	FIVESEVEN_DASHING,
	FIVESEVEN_DASH_EXIT,
	FIVESEVEN_SH_RELOAD,
	FIVESEVEN_SH_RELOAD_EMPTY,
	FIVESEVEN_SH_DASH_ENTER,
	FIVESEVEN_SH_DASHING,
	FIVESEVEN_SH_DASH_EXIT,
};

class CFN57 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM = (1 << FIVESEVEN_IDLE) |
												(1 << FIVESEVEN_DRAW) |
												(1 << FIVESEVEN_HOLSTER) |
												(1 << FIVESEVEN_CHECK_MAGAZINE) |
												(1 << FIVESEVEN_LHAND_DOWN) | (1 << FIVESEVEN_LHAND_UP) |
												(1 << FIVESEVEN_BLOCK_DOWN) | (1 << FIVESEVEN_BLOCK_UP) |
												(1 << FIVESEVEN_DASH_ENTER) | (1 << FIVESEVEN_DASHING) | (1 << FIVESEVEN_DASH_EXIT) |
												(1 << FIVESEVEN_SH_DASH_ENTER) | (1 << FIVESEVEN_SH_DASHING) | (1 << FIVESEVEN_SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return FiveSevenFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void) { return DefaultIdle(FIVESEVEN_DASHING); }
	virtual bool	HolsterStart	(void) { return DefaultHolster(FIVESEVEN_HOLSTER, FIVESEVEN_HOLSTER_TIME); }
	virtual	void	DashStart		(void) { return DefaultDashStart(FIVESEVEN_DASH_ENTER, FIVESEVEN_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void) { return DefaultDashEnd(FIVESEVEN_DASH_ENTER, FIVESEVEN_DASH_ENTER_TIME, FIVESEVEN_DASH_EXIT, FIVESEVEN_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return FIVESEVEN_MAX_SPEED; }
	virtual void	ResetModel		(void);	// declare by marco.
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, FIVESEVEN_LHAND_UP, FIVESEVEN_LHAND_UP_TIME, FIVESEVEN_LHAND_DOWN, FIVESEVEN_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(FIVESEVEN_BLOCK_UP, FIVESEVEN_BLOCK_UP_TIME, FIVESEVEN_BLOCK_DOWN, FIVESEVEN_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void FiveSevenFire(float flSpread, float flCycleTime = FIVESEVEN_FIRE_INTERVAL);
};

#define UMP45_VIEW_MODEL	"models/weapons/v_ump45.mdl"
#define UMP45_WORLD_MODEL	"models/weapons/w_ump45.mdl"	
#define UMP45_FIRE_SFX		"weapons/ump45/ump45_fire.wav"
#define UMP45_FIRE_SIL_SFX	"weapons/ump45/ump45_fire.wav"	// FIXME

constexpr float UMP45_MAX_SPEED			= 250.0f;
constexpr float UMP45_DAMAGE			= 32;
constexpr float UMP45_RANGE_MODIFER		= 1.149658245;	// 80% damage @800 inches.
constexpr float UMP45_RELOAD_TIME		= 2.8667F;
constexpr float UMP45_RELOAD_EMPTY_TIME	= 3.0667F;
constexpr float UMP45_DRAW_TIME			= 0.6333F;
constexpr float UMP45_DRAW_FIRST_TIME	= 1.2F;
constexpr float UMP45_HOLSTER_TIME		= 0.6333F;
constexpr float UMP45_CHECKMAG_TIME		= 3.6F;
constexpr float UMP45_BLOCK_UP_TIME		= 0.3667F;
constexpr float UMP45_BLOCK_DOWN_TIME	= 0.3667F;
constexpr float UMP45_LHAND_DOWN_TIME	= 0.7F;
constexpr float UMP45_LHAND_UP_TIME		= 0.7F;
constexpr float UMP45_DASH_ENTER_TIME	= 0.3667F;
constexpr float UMP45_DASH_EXIT_TIME	= 0.3667F;
constexpr float UMP45_RPM				= 600.0f;
constexpr float	UMP45_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	UMP45_PENETRATION		= 1;
constexpr float UMP45_SPREAD_BASELINE	= 0.15f;
constexpr int	UMP45_GUN_VOLUME		= NORMAL_GUN_VOLUME;

enum ump45_e
{
	UMP45_IDLE1,
	UMP45_SHOOT,
	UMP45_SHOOT_AIM,
	UMP45_RELOAD,
	UMP45_RELOAD_EMPTY,
	UMP45_DRAW,
	UMP45_DRAW_FIRST,
	UMP45_HOLSTER,
	UMP45_CHECKMAG,
	UMP45_TO_SEMI,
	UMP45_TO_AUTO,
	UMP45_BLOCK_UP,
	UMP45_BLOCK_DOWN,
	UMP45_LHAND_DOWN,
	UMP45_LHAND_UP,
	UMP45_DASH_ENTER,
	UMP45_DASHING,
	UMP45_DASH_EXIT
};

class CUMP45 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return UMP45Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void)	{ return DefaultIdle(UMP45_DASHING); }
	virtual bool	HolsterStart	(void)	{ return DefaultHolster(UMP45_HOLSTER, UMP45_HOLSTER_TIME); }
	virtual	void	DashStart		(void)	{ return DefaultDashStart(UMP45_DASH_ENTER, UMP45_DASH_ENTER_TIME); }
	virtual void	DashEnd			(void)	{ return DefaultDashEnd(UMP45_DASH_ENTER, UMP45_DASH_ENTER_TIME, UMP45_DASH_EXIT, UMP45_DASH_EXIT_TIME); }

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return UMP45_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual bool	SetLeftHand		(bool bAppear)	{ return DefaultSetLHand(bAppear, UMP45_LHAND_UP, UMP45_LHAND_UP_TIME, UMP45_LHAND_DOWN, UMP45_LHAND_DOWN_TIME); }
	virtual void	PlayBlockAnim	(void)	{ return DefaultBlock(UMP45_BLOCK_UP, UMP45_BLOCK_UP_TIME, UMP45_BLOCK_DOWN, UMP45_BLOCK_DOWN_TIME); }
	virtual float	GetSpread		(void);

public:	// new functions
	void UMP45Fire(float flSpread, float flCycleTime = 60.0f / UMP45_RPM);
};

#define M14EBR_VIEW_MODEL	"models/weapons/v_m14ebr.mdl"
#define M14EBR_WORLD_MODEL	"models/weapons/w_m14ebr.mdl"
#define M14EBR_FIRE_SFX		"weapons/m14ebr/m14ebr_fire.wav"

const float SG550_MAX_SPEED      = 210.0f;
const float SG550_MAX_SPEED_ZOOM = 150.0f;
const float SG550_DAMAGE         = 90;
const float SG550_RANGE_MODIFER  = 0.98f;	// 85% damage @2400 inches.
const float SG550_RELOAD_TIME    = 3.35f;

enum sg550_e
{
	SG550_IDLE,
	SG550_SHOOT,
	SG550_SHOOT2,
	SG550_RELOAD,
	SG550_DRAW,
};

#define C4_VIEW_MODEL	"models/weapons/v_c4.mdl"
#define C4_WORLD_MODEL	"models/weapons/w_c4.mdl"
#define C4_PLACED_SFX	"zombieriot/plant.wav"

constexpr float C4_TIME_DRAW		= 0.7f;
constexpr float C4_TIME_HOLSTER		= 0.7f;
constexpr float C4_TIME_THROW		= 1.0333f;
constexpr float C4_TIME_THROW_SPAWN = 0.5f;
constexpr float C4_TIME_PLACE		= 1.0333f;
constexpr float C4_TIME_PLACE_SPAWN = 0.5f;
constexpr float C4_TIME_DET_ANIM	= 1.0333f;
constexpr float C4_TIME_DETONATE	= 0.4f;

enum c4_e
{
	C4_IDLE = 0,
	C4_DRAW,
	C4_HOLSTER,
	C4_THROW,
	C4_PLACE,
	C4_DETONATE,
};

#ifndef CLIENT_DLL
extern short g_sModelIndexLaser;
extern short g_sModelIndexLaserDot;

extern short g_sModelIndexFireball;
extern short g_sModelIndexSmoke;
extern short g_sModelIndexWExplosion;
extern short g_sModelIndexBubbles;
extern short g_sModelIndexBloodDrop;
extern short g_sModelIndexBloodSpray;
extern short g_sModelIndexSmokePuff;
extern short g_sModelIndexFireball2;
extern short g_sModelIndexFireball3;
extern short g_sModelIndexFireball4;
extern short g_sModelIndexCTGhost;
extern short g_sModelIndexTGhost;
extern short g_sModelIndexC4Glow;

extern short g_sModelIndexRadio;
extern MULTIDAMAGE gMultiDamage;

void WeaponsPrecache();
void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity);
void AnnounceFlashInterval(float interval, float offset = 0);

void ClearMultiDamage();
void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker);
void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);
void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
int DamageDecal(CBaseEntity *pEntity, int bitsDamageType);
void DecalGunshot(TraceResult *pTrace, int iBulletType, bool ClientOnly, entvars_t *pShooter, bool bHitMetal);
void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype, int entityIndex);
void UTIL_PrecacheOtherWeapon(WeaponIdType iId);
BOOL CanAttack(float attack_time, float curtime, BOOL isPredicted);
#endif
