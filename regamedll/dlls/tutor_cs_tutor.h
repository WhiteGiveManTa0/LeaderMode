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

#include <map>

enum TutorMessageClass
{
	TUTORMESSAGECLASS_NORMAL = 0,
	TUTORMESSAGECLASS_EXAMINE,
};

enum TutorMessageType
{
	TUTORMESSAGETYPE_DEFAULT      = BIT(0), // icon info  | color green
	TUTORMESSAGETYPE_FRIEND_DEATH = BIT(1), // icon skull | color red
	TUTORMESSAGETYPE_ENEMY_DEATH  = BIT(2), // icon skull | color blue
	TUTORMESSAGETYPE_SCENARIO     = BIT(3), // icon info  | color yellow
	TUTORMESSAGETYPE_BUY          = BIT(4), // icon info  | color green
	TUTORMESSAGETYPE_CAREER       = BIT(5), // icon info  | color green
	TUTORMESSAGETYPE_HINT         = BIT(6), // icon info  | color green
	TUTORMESSAGETYPE_INGAME_HINT  = BIT(7), // icon info  | color green
	TUTORMESSAGETYPE_END_GAME     = BIT(8), // icon info  | color yellow

	TUTORMESSAGETYPE_LAST,
	TUTORMESSAGETYPE_ALL          = (TUTORMESSAGETYPE_DEFAULT | TUTORMESSAGETYPE_FRIEND_DEATH | TUTORMESSAGETYPE_ENEMY_DEATH | TUTORMESSAGETYPE_SCENARIO | TUTORMESSAGETYPE_BUY | TUTORMESSAGETYPE_CAREER | TUTORMESSAGETYPE_HINT | TUTORMESSAGETYPE_INGAME_HINT | TUTORMESSAGETYPE_END_GAME)
};

enum TutorMessageInterruptFlag
{
	TUTORMESSAGEINTERRUPTFLAG_DEFAULT = 0,
	TUTORMESSAGEINTERRUPTFLAG_NOW_DAMMIT,
};

enum TutorMessageKeepOldType
{
	TUTORMESSAGEKEEPOLDTYPE_DONT_KEEP_OLD = 0,
	TUTORMESSAGEKEEPOLDTYPE_KEEP_OLD,
	TUTORMESSAGEKEEPOLDTYPE_UPDATE_CONTENT,
};

struct TutorMessage
{
	char *m_text;
	unsigned char m_priority;
	unsigned char m_duration;
	TutorMessageKeepOldType m_keepOld;
	TutorMessageClass m_class;
	unsigned char m_decay;
	TutorMessageType m_type;
	int m_lifetime;
	TutorMessageInterruptFlag m_interruptFlag;
	int m_duplicateID;
	float m_examineStartTime;
	int m_timesShown;
	float m_minDisplayTimeOverride;
	float m_minRepeatInterval;
	float m_lastCloseTime;
};

struct TutorMessageEventParam
{
	char *m_data;
	TutorMessageEventParam *m_next;
};

enum TutorMessageID
{
	YOU_FIRED_A_SHOT,
	YOU_SHOULD_RELOAD,
	YOU_ARE_OUT_OF_AMMO,
	YOU_KILLED_A_TEAMMATE,
	YOU_KILLED_PLAYER,
	YOU_KILLED_PLAYER_ONE_LEFT,
	YOU_KILLED_LAST_ENEMY,
	YOU_KILLED_PLAYER_HEADSHOT,
	YOU_KILLED_PLAYER_HEADSHOT_ONE_LEFT,
	YOU_KILLED_LAST_ENEMY_HEADSHOT,
	YOU_DIED,
	YOU_DIED_HEADSHOT,
	YOU_FELL_TO_YOUR_DEATH,
	YOU_WERE_JUST_HURT,
	YOU_ARE_BLIND_FROM_FLASHBANG,
	YOU_ATTACKED_TEAMMATE,
	BUY_TIME_BEGIN,
	BOMB_PLANTED_T,
	BOMB_PLANTED_CT,
	TEAMMATE_KILLED,
	TEAMMATE_KILLED_ONE_LEFT,
	LAST_TEAMMATE_KILLED,
	ENEMY_KILLED,
	ENEMY_KILLED_ONE_LEFT,
	LAST_ENEMY_KILLED,
	YOU_SPAWNED,
	YOU_SEE_FRIEND,
	YOU_SEE_ENEMY,
	YOU_SEE_FRIEND_CORPSE,
	YOU_SEE_ENEMY_CORPSE,
	YOU_SEE_LOOSE_WEAPON,
	YOU_HAVE_BEEN_SHOT_AT,
	ROUND_OVER,
	ROUND_DRAW,
	CT_WIN,
	T_WIN,
	DEATH_CAMERA_START,
	RADIO_COVER_ME,
	RADIO_YOU_TAKE_THE_POINT,
	RADIO_HOLD_THIS_POSITION,
	RADIO_REGROUP_TEAM,
	RADIO_FOLLOW_ME,
	RADIO_TAKING_FIRE,
	RADIO_GO_GO_GO,
	RADIO_TEAM_FALL_BACK,
	RADIO_STICK_TOGETHER_TEAM,
	RADIO_GET_IN_POSITION_AND_WAIT,
	RADIO_STORM_THE_FRONT,
	RADIO_REPORT_IN_TEAM,
	RADIO_AFFIRMATIVE,
	RADIO_ENEMY_SPOTTED,
	RADIO_NEED_BACKUP,
	RADIO_SECTOR_CLEAR,
	RADIO_IN_POSITION,
	RADIO_REPORTING_IN,
	RADIO_GET_OUT_OF_THERE,
	RADIO_NEGATIVE,
	RADIO_ENEMY_DOWN,
	BUY_NEED_PRIMARY,
	BUY_NEED_PRIMARY_AMMO,
	BUY_NEED_SECONDARY_AMMO,
	BUY_NEED_ARMOR,
	BUY_NEED_DEFUSE_KIT,
	BUY_NEED_GRENADE,
	CAREER_TASK_DONE_MORE_LEFT,
	CAREER_TASK_DONE_ONE_LEFT,
	CAREER_TASK_DONE_ALL_DONE,
	HINT_BEGIN,
	HINT_1,
	HINT_2,
	HINT_3,
	HINT_4,
	HINT_5,
	HINT_10,
	HINT_11,
	HINT_12,
	HINT_13,
	HINT_14,
	HINT_15,
	HINT_20,
	HINT_21,
	HINT_22,
	HINT_23,
	HINT_24,
	HINT_25,
	HINT_26,
	HINT_30,
	HINT_31,
	HINT_32,
	HINT_33,
	HINT_34,
	HINT_40,
	HINT_50,
	HINT_51,
	HINT_52,
	HINT_53,
	HINT_BOMB_START	= 139,
	HINT_60	= 139,
	HINT_61 = 140,
	HINT_BOMB_END = 140,
	HINT_HOSTAGE_START = 141,
	HINT_70 = 141,
	HINT_71,
	HINT_72,
	HINT_73 = 144,
	HINT_HOSTAGE_END = 144,
	HINT_END,
	INGAME_HINT_BEGIN,
	INGAME_HINT_1,
	INGAME_HINT_2,
	INGAME_HINT_END,
	TUTOR_NUM_MESSAGES
};

typedef std::map<std::string, TutorMessage *> TutorMessageMap;
typedef TutorMessageMap::iterator TutorMessageMapIter;

struct ClientCorpseStruct
{
	Vector m_position;
	int m_team;
};

typedef std::vector<ClientCorpseStruct *> ClientCorpseList;
typedef ClientCorpseList::iterator ClientCorpseListIter;

class CCSTutor: public CBaseTutor
{
public:
	CCSTutor();

	struct PlayerDeathStruct
	{
		bool m_hasBeenShown;
		TutorMessageEvent *m_event;
	};

	virtual ~CCSTutor();
	virtual void TutorThink(float time);
	virtual void PurgeMessages();
	virtual void CallEventHandler(GameEventType event, CBaseEntity *pEntity, CBaseEntity *pOther);
	virtual void ShowTutorMessage(TutorMessageEvent *event);
	virtual void HandleShotFired(Vector source, Vector target);
	virtual TutorMessage *GetTutorMessageDefinition(int messageID);

	void CreateAndAddEventToList(TutorMessageID mid, CBaseEntity *pEntity = nullptr, CBaseEntity *pOther = nullptr);
	TutorMessageEvent *CreateTutorMessageEvent(TutorMessageID mid, CBaseEntity *pEntity = nullptr, CBaseEntity *pOther = nullptr);
	void AddToEventList(TutorMessageEvent *event);
	void DeleteEventFromEventList(TutorMessageEvent *event);
	void ClearEventList();
	void ClearCurrentEvent(bool closeWindow = true, bool processDeathsForEvent = true);
	void DeleteEvent(TutorMessageEvent *event);
	bool ShouldShowMessageEvent(TutorMessageEvent *event, float time);
	bool ShouldUpdateCurrentMessage(TutorMessageID messageID);
	void ComputeDisplayTimesForMessage();
	void UpdateCurrentMessage(TutorMessageEvent *event);
	void ConstructMessageAndDisplay();
	void LookupHotKey(TutorMessageID mid, int paramNum, wchar_t *buf, int buflen);
	void CheckForWindowClose(float time);
	void CheckForContentUpdate();
	bool HasCurrentWindowBeenActiveLongEnough(float time);
	void CheckForInterruptingMessageEvent(float time);
	void CheckForInactiveEvents(float time);
	void CancelEvent(TutorMessageID mid);
	void ProcessShownDeathsForEvent(TutorMessageEvent *event);
	void TransferDeathEvents(TutorMessageEvent *oldEvent, TutorMessageEvent *newEvent);
	TutorMessageEvent *GetTutorMessageUpdateEvent();
	bool GetDuplicateMessagesFromEventList(TutorMessageEvent *&event1, TutorMessageEvent *&event2);

public:
	void HandleWeaponFired(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleWeaponFiredOnEmpty(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleWeaponReloaded(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandlePlayerDied(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandlePlayerSpawned(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleClientCorpseSpawned(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandlePlayerTookDamage(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandlePlayerBlindedByFlashbang(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleBuyTimeStart(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandlePlayerLeftBuyZone(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRoundStart(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleBeingShotAt(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRoundDraw(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleCTWin(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleTWin(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleDeathCameraStart(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioCoverMe(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioYouTakeThePoint(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioHoldThisPosition(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioRegroupTeam(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioFollowMe(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioTakingFire(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioGoGoGo(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioTeamFallBack(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioStickTogetherTeam(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioGetInPositionAndWait(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioStormTheFront(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioReportInTeam(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioAffirmative(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioEnemySpotted(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioNeedBackup(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioSectorClear(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioInPosition(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioReportingIn(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioGetOutOfThere(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioNegative(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleRadioEnemyDown(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleBuyMenuOpenned(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleAutoBuy(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleNotBuyingAnything(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleNeedToBuyPrimaryWeapon(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleNeedToBuyPrimaryAmmo(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleNeedToBuySecondaryAmmo(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleNeedToBuyArmor(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleNeedToBuyGrenade(CBaseEntity *pEntity, CBaseEntity *pOther);
	void HandleCareerTaskDone(CBaseEntity *pEntity, CBaseEntity *pOther);

	void GetNumPlayersAliveOnTeams(int &numT, int &numCT);
	void CheckForLooseWeaponViewable();
	void CheckForTimeRunningOut();
	void CheckHintMessages(float time);
	void CheckInGameHintMessages(float time);
	void CheckExamineMessages(float time);
	void CheckForNeedToReload(bool isPassiveCheck = false);
	bool CanLocalPlayerBuyStuff();
	void CheckBuyZoneMessages();
	void ReadTutorMessageFile();
	void ApplyPersistentDecay();
	CBaseEntity *GetEntityForMessageID(int messageID, CBaseEntity *last = nullptr);
	void ResetPlayerDeathInfo();
	void ConstructRecentDeathsList(TeamName team, char *buf, int buflen, TutorMessageEvent *event);

private:
	static const char *m_TutorIdentifierList[];

	float m_nextViewableCheckTime;
	TutorMessageMap m_messageMap;
	TutorMessageID m_currentlyShownMessageID;
	float m_currentlyShownMessageCloseTime;
	float m_currentlyShownMessageStartTime;
	float m_currentlyShownMessageMinimumCloseTime;
	TutorMessageEvent *m_currentMessageEvent;
	TutorMessageEvent *m_lastScenarioEvent;
	TutorMessageID m_lastHintShown;
	TutorMessageID m_lastInGameHintShown;
	ClientCorpseList m_clientCorpseList;
	int m_messageTypeMask;
	bool m_haveSpawned;
	PlayerDeathStruct m_playerDeathInfo[MAX_CLIENTS];
};

void ParseMessageParameters(const char *&messageData, TutorMessage *ret);
TutorMessage *ConstructTutorMessage(const char *&messageData, TutorMessage *defaults);
void ReadDefaultValues(const char *&messageData, TutorMessage *defaults);

// custom operator++
inline TutorMessageID operator++(TutorMessageID &iter, int)
{
	// bounds checking
	if (iter == TUTOR_NUM_MESSAGES)
	{
		return YOU_FIRED_A_SHOT;
	}

	iter = static_cast<TutorMessageID>(static_cast<int>(iter) + 1);
	return iter;
}
