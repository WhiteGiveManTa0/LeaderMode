/*

Created Date: 07 Mar 2020

*/

#pragma once

#include <list>

#define RGB_YELLOWISH 0x00FFA000
#define RGB_REDISH 0x00FF1010
#define RGB_GREENISH 0x0000A000

#define DHN_DRAWZERO	BIT(0)
#define DHN_2DIGITS		BIT(1)
#define DHN_3DIGITS		BIT(2)
#define DHN_FILLZERO	BIT(3)
#define DHN_4DIGITS		BIT(4)
#define DHN_5DIGITS		BIT(5)
#define MIN_ALPHA 100

#define MAX_SPRITE_NAME_LENGTH 24

#define HUD_ACTIVE			BIT(0)
#define HUD_INTERMISSION	BIT(1)

#define MAX_PLAYER_NAME_LENGTH 128
#define MAX_MOTD_LENGTH 1536

#define FADE_TIME 100

extern cvar_t* cl_lw;
extern cvar_t* cl_righthand;
extern cvar_t* cl_radartype;
extern cvar_t* cl_dynamiccrosshair;
extern cvar_t* cl_crosshair_color;
extern cvar_t* cl_crosshair_size;
extern cvar_t* cl_crosshair_translucent;
extern cvar_t* cl_crosshair_type;
extern cvar_t* cl_killeffect;
extern cvar_t* cl_killmessage;
extern cvar_t* cl_headname;
extern cvar_t* cl_newmenu;
extern cvar_t* cl_newmenu_drawbox;
extern cvar_t* cl_newradar;
extern cvar_t* cl_newradar_size;
extern cvar_t* cl_newradar_dark;
extern cvar_t* cl_newchat;
extern cvar_t* cl_shadows;
extern cvar_t* cl_scoreboard;
extern cvar_t* cl_fog_skybox;
extern cvar_t* cl_fog_density;
extern cvar_t* cl_fog_r;
extern cvar_t* cl_fog_g;
extern cvar_t* cl_fog_b;
extern cvar_t* cl_minmodel;
extern cvar_t* cl_min_t;
extern cvar_t* cl_min_ct;
extern cvar_t* cl_corpsestay;
extern cvar_t* cl_corpsefade;

extern Vector g_LocationColor;

typedef struct client_sprite_s
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	wrect_t rc;
} client_sprite_t;

typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	Vector origin;

	// fields that can be changed by the cldll
	Vector viewangles;
	int		iWeaponBits;
	//	int		iAccessoryBits;
	float	fov;	// field of view
} client_data_t;

typedef struct SCREENINFO_s
{
	int		iSize;
	int		iWidth;
	int		iHeight;
	int		iFlags;
	int		iCharHeight;
	short	charWidths[256];
} SCREENINFO;

typedef struct hud_player_info_s
{
	char* name;
	short ping;
	byte thisplayer;  // TRUE if this is the calling player

	byte spectator;
	byte packetloss;

	char* model;
	short topcolor;
	short bottomcolor;

	uint64 m_nSteamID;
} hud_player_info_t;

class CBaseHUDElement
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
	CBaseHUDElement() {}
	virtual ~CBaseHUDElement() {}

public:
	int	  m_bitsFlags; // active, moving,

public:
	virtual int Init(void) { return 0; }
	virtual int VidInit(void) { return 0; }
	virtual int Draw(float flTime) { return 0; }
	virtual void Think(void) {}
	virtual void Reset(void) {}
	virtual void InitHUDData(void) {}		// called every time a server is connected to
	virtual void Shutdown(void) {}
};

// dummy classes derived from CBaseHUDElement.
class CHudAmmo;
class CHudHealth;
class CHudSpectator;
class CHudGeiger;
class CHudBattery;
class CHudTrain;
class CHudFlashlight;
class CHudMessage;
class CHudStatusBar;
class CHudDeathNotice;
class CHudSayText;
class CHudMenu;
class CHudNightVision;
class CHudTextMessage;
class CHudRoundTimer;
class CHudAccountBalance;
class CHudHeadName;
class CHudRadar;
class CHudStatusIcons;
class CHudScenarioStatus;
class CHudProgressBar;
class CHudVGUI2Print;

namespace gHUD
{
	// HUD exports
	void Init(void);
	void Shutdown(void);
	void VidInit(void);
	int Redraw(float flTime, int intermission);
	void Think(void);
	int UpdateClientData(client_data_t* cdata, float time);
	void CalcRefdef(ref_params_s* pparams);

	// HUD utils
	int GetSpriteIndex(const char* SpriteName);
	int DrawHudString(int xpos, int ypos, int iMaxX, const char* szIt, int r, int g, int b);
	int DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b);
	int DrawHudStringReverse(int xpos, int ypos, int iMinX, const char* szString, int r, int g, int b);
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b);
	int DrawHudNumber(int x, int y, int iNumber, int r, int g, int b);
	int GetNumWidth(int iNumber, int iFlags);
	int GetNumBits(int iNumber);
	void AddHudElem(CBaseHUDElement* phudelem);
	float GetSensitivity(void);
	hSprite GetSprite(int index);
	wrect_t& GetSpriteRect(int index);
	client_sprite_t* GetSpriteList(client_sprite_t* pList, const char* psz, int iRes, int iCount);

	// HUD bridges
	void SlotInput(int iSlot);

	// VARs
	extern std::list<CBaseHUDElement*> m_lstAllHUDElems;

	extern cvar_s* m_pCvarDraw;
	extern cvar_s* default_fov;

	extern client_sprite_t* m_pSpriteList;
	extern int m_iPlayerNum;
	extern float m_flTime;
	extern float m_fOldTime;
	extern double m_flTimeDelta;
	extern int m_iFOV;
	extern int m_iSpriteCount;
	extern int m_iSpriteCountAllRes;
	extern int m_iRes;
	extern hSprite* m_rghSprites;
	extern wrect_t* m_rgrcRects;
	extern char* m_rgszSpriteNames;
	extern char m_szGameMode[32];
	extern int m_HUD_number_0;
	extern int m_iFontHeight;
	extern int m_iFontEngineHeight;
	extern bool m_iIntermission;
	extern char m_szMOTD[2048];
	extern float m_flTimeLeft;
	extern int m_bitsHideHUDDisplay;
	extern float m_flMouseSensitivity;
	extern int m_PlayerFOV[MAX_CLIENTS];
	extern Vector m_vecOrigin;
	extern Vector m_vecAngles;
	extern int m_iKeyBits;
	extern int m_iWeaponBits;
	extern bool m_fPlayerDead;

	extern SCREENINFO m_scrinfo;

	// HUD elements.
	extern CHudAmmo m_Ammo;
	extern CHudHealth m_Health;
	extern CHudSpectator m_Spectator;
	extern CHudGeiger m_Geiger;
	extern CHudBattery m_Battery;
	extern CHudTrain m_Train;
	extern CHudFlashlight m_Flash;
	extern CHudMessage m_Message;
	extern CHudStatusBar m_StatusBar;
	extern CHudDeathNotice m_DeathNotice;
	extern CHudSayText m_SayText;
	extern CHudMenu m_Menu;
	extern CHudNightVision m_NightVision;
	extern CHudTextMessage m_TextMessage;
	extern CHudRoundTimer m_roundTimer;
	extern CHudAccountBalance m_accountBalance;
	extern CHudHeadName m_headName;
	extern CHudRadar m_Radar;
	extern CHudStatusIcons m_StatusIcons;
	extern CHudScenarioStatus m_scenarioStatus;
	extern CHudProgressBar m_progressBar;
	extern CHudVGUI2Print m_VGUI2Print;
};

extern hud_player_info_t g_PlayerInfoList[MAX_PLAYERS + 1];
extern int g_PlayerScoreAttrib[MAX_PLAYERS + 1];
extern TEMPENTITY* g_DeadPlayerModels[MAX_PLAYERS + 1];

// HUD command funcs
void CommandFunc_Slot1(void);
void CommandFunc_Slot2(void);
void CommandFunc_Slot3(void);
void CommandFunc_Slot4(void);
void CommandFunc_Slot5(void);
void CommandFunc_Slot6(void);
void CommandFunc_Slot7(void);
void CommandFunc_Slot8(void);
void CommandFunc_Slot9(void);
void CommandFunc_Slot10(void);
void CommandFunc_Close(void);
void CommandFunc_NextWeapon(void);
void CommandFunc_PrevWeapon(void);
void CommandFunc_Adjust_Crosshair(void);

#define ScreenHeight (gHUD::m_scrinfo.iHeight)
#define ScreenWidth (gHUD::m_scrinfo.iWidth)

#define XPROJECT(x)	((1.0f + (x)) * ScreenWidth * 0.5f)
#define YPROJECT(y) ((1.0f - (y)) * ScreenHeight * 0.5f)

// Use this to set any co-ords in 640x480 space
inline int XRES(float x) { return static_cast<int>(x * (static_cast<float>(ScreenWidth) / 640.0f) + 0.5f); }
inline int YRES(float y) { return static_cast<int>(y * (static_cast<float>(ScreenHeight) / 480.0f) + 0.5f); }

// for HUD_PostRunCmd
extern float g_lastFOV;