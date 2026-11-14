#ifndef _PLAYER_BOT_H_
#define _PLAYER_BOT_H_

#include "worldserver.h"
#include "player.h"
#include "worldmap.h"
#include "WorldMonster.h"
#include <vector>
#include <string>
#include <map>
#include <pthread.h>

enum BotState {
    BOT_STATE_IDLE,
    BOT_STATE_ROAM,
    BOT_STATE_COMBAT,
    BOT_STATE_LOOT,
    BOT_STATE_REST,
    BOT_STATE_FOLLOW,
    BOT_STATE_DEAD,
    BOT_STATE_BUFF_BOT,
    BOT_STATE_SEEK_BUFF,
    BOT_STATE_VENDING,
    BOT_STATE_TOWN_STROLL,
    BOT_STATE_MIGRATE,
    BOT_STATE_DUEL,
    BOT_STATE_TOWN_REPAIR,
    BOT_STATE_LFG_SHOUT,
    BOT_STATE_RALLY_RAID,
    BOT_STATE_VENDOR_RUN,
    BOT_STATE_QUEST_RETURN,
    BOT_STATE_CLAN_HANGOUT,
    BOT_STATE_SIEGE_UNIT
};

struct BotBounty {
    UINT targetMobId;
    std::string mobName;
    int requiredKills;
    int currentKills;
    UINT npcMapId;
    fPoint npcPos;
    bool isComplete;
};

enum BotPersonality {
    BOT_PERSONALITY_RIVAL,
    BOT_PERSONALITY_HELPER,
    BOT_PERSONALITY_MERCHANT,
    BOT_PERSONALITY_BRAWLER,
    BOT_PERSONALITY_SPEEDRUNNER,
    BOT_PERSONALITY_SOCIAL,
    BOT_PERSONALITY_CASUAL,
    BOT_PERSONALITY_TYCOON
};

enum BotRole {
    BOT_ROLE_TANK,
    BOT_ROLE_HEALER,
    BOT_ROLE_DPS_PHYS_MELEE,
    BOT_ROLE_DPS_PHYS_RANGED,
    BOT_ROLE_DPS_MAGIC,
    BOT_ROLE_GENERAL_VISITOR
};

enum BotStance {
    BOT_STANCE_DEFENSIVE = 0, // Bodyguard: protects leader & party members (default)
    BOT_STANCE_AGGRESSIVE = 1, // Hunter: proactively pulls quest & nearby level-matched mobs
    BOT_STANCE_PASSIVE = 2,    // Pacifist: holds fire, sprints with leader, ignores aggro
    BOT_STANCE_ASSIST = 3      // Focus Fire: focuses leader's combat/selected target
};

enum BotTactic {
    BOT_TACTIC_BALANCED = 0, // Balanced mix of offense, defense, and resource management (default)
    BOT_TACTIC_SPEEDRUN = 1, // Max DPS burst, aggressive pulling, ignores mana conservation
    BOT_TACTIC_SAFE = 2,     // Ultra-defensive, high CC, emergency shields & triage healing > 70% HP
    BOT_TACTIC_ECO = 3       // Resource saver: minimal mana expenditure, basic attacks, heals < 30% HP
};

enum BotSubSpec {
    BOT_SUBSPEC_DEFAULT = 0, // Default spec weapon & rotation (e.g. 1H Sword & Shield, Wand, Bow)
    BOT_SUBSPEC_2H = 1,      // Berserker 2H Greatsword (cleave DPS)
    BOT_SUBSPEC_SMITE = 2,   // Battle Priest Mace & Shield (holy melee DPS)
    BOT_SUBSPEC_GUN = 3      // Guns / Dual Launchers rapid-fire burst
};

enum SoundCueType {
    CUE_LOW_HP = 1,
    CUE_VICTORY_HORN = 2,
    CUE_LIMIT_BREAK = 3
};

enum BotOutfitType {
    OUTFIT_DEFAULT = 0,
    OUTFIT_CASUAL = 1,
    OUTFIT_ARMORED = 2,
    OUTFIT_SWIMWEAR = 3,
    OUTFIT_FESTIVE = 4
};

enum BotTalentSpec {
    TALENT_NONE = 0,
    TALENT_BASTION = 1,
    TALENT_LUMINARY = 2,
    TALENT_SPELLWEAVER = 3,
    TALENT_SCAVENGER = 4
};

struct BotGambitRule {
    std::string condition;
    std::string action;
    bool enabled;
};

struct BotDispatchMission {
    std::string missionId;
    std::string title;
    std::string assignedMerc;
    clock_t startTime;
    int durationSec;
    long long rewardZulies;
    int rewardExp;
    std::string rewardItem;
    bool completed;
    bool claimed;
};

class CPlayerBot {
public:
    CPlayerBot( CPlayer* player );
    ~CPlayerBot( );

    void Update( ); // AI decision cycle (throttled)

    // State Handlers
    void HandleIdle( );
    void HandleRoam( );
    void HandleCombat( );
    void HandleLoot( );
    void HandleRest( );
    void HandleFollow( );
    void HandleDead( );

    void HandleBuffBot( );
    void HandleSeekBuff( );
    void HandleVendingBot( );
    void SetupVendingShop( const char* shopTitle, int category );

    void HandleTownStroll( );
    void HandleMigrate( );
    void HandleDuel( );
    void HandleTownRepair( );
    void HandleRallyRaid( );
    void HandleVendorRun( );
    void TriggerVendorRun( );
    void HandleQuestReturn( );
    void HandleClanHangout( );
    void HandleSiegeUnit( );
    void StartDuel( CPlayer* challenger );
    void CheckPlayerGreetings( );
    void AssignClanTag( );
    void BroadcastRaidRally( CMonster* boss );
    void SetRaidTarget( CMonster* boss, fPoint rallyPos );
    void CheckClanRivalry( CPlayer* other );
    void AssignNewBounty( );
    void CheckBountyProgress( CMonster* mob );
    const BotBounty* GetBounty( ) const { return &m_bounty; }

    void RecordGratitude( CPlayer* player, int points = 1 );
    void RecordNemesis( CPlayer* player, int points = 1 );
    bool IsFriend( CPlayer* player ) const;
    bool IsNemesis( CPlayer* player ) const;
    void CheckNemesisRevenge( CPlayer* player );
    void CheckClanRecruitmentShout( );
    void HandleClanInviteRequest( CPlayer* sender );
    void HandleTradeRequest( CPlayer* sender );

    void CheckPlayerShops( );
    void CheckWhispers( );
    void CheckArenaQueue( );
    void CheckDungeonRuns( );
    void CheckLfgShouts( );
    void CheckWorldBossRaids( );
    void ProcessIncomingWhisper( CPlayer* sender, const char* msg );
    void TrackPvpResult( UINT playerCharId, bool won );
    void AssignTitle( );
    const char* GetTitleString( ) const { return m_title.c_str( ); }
    BotPersonality GetPersonality( ) const { return m_personality; }
    void WhisperPlayer( CPlayer* target, const char* msg );

    // D&D Dice Roll Helpers & Anti-Clustering Logic
    static int RollD20( ) { return 1 + ( rand( ) % 20 ); }
    static int RollD100( ) { return 1 + ( rand( ) % 100 ); }
    bool IsMobReserved( CMonster* mob );

    // Actions
    void SetState( BotState state );
    void MoveTo( fPoint dest );
    void AttackTarget( CCharacter* target );
    void StopMoving( );
    void SitDown( );
    void StandUp( );
    void AllocateStats( );
    void Respawn( );
    void Say( const char* msg );
    void DoEmote( BYTE emoteId );

    // Phase 1: Combat Intelligence & Potions
    void ConsumePotions( );
    bool KiteTarget( CCharacter* target );
    bool IsKillSteal( CMonster* mob );

    // Phase 2: Social Engine & Party System
    void CelebrateLevelUp( );
    void SayChatter( const char* category );
    void CheckPartyInvitations( );

    // Phase 3: Town Life, Migration & Custom Appearance
    void ApplyRandomCosmetics( );
    void CheckZoneMigration( );

    // Progression, Skills & Equipment
    void EquipTieredGear( bool broadcast = true );
    void UpdateSkills( );
    bool GetLearnedSkill( UINT baseSkillId, UINT& outSkillId, int& outSkillLevel );
    bool HasSkill( UINT baseSkillId );
    bool CastCombatSkill( CCharacter* target );
    bool CheckPartyHeal( );
    bool CheckPartyBuffs( );
    bool CheckPartyResurrect( );
    bool CheckPartyTaunt( );
    bool CheckProximityBuffs( );
    CPlayer* GetPartyLeader( );
    fPoint GetFormationOffset( int slotIndex, fPoint leaderCurrent, fPoint leaderDest );
    int GetPartySlotIndex( ) const;
    void ForcePartyBuff( );
    void CheckProgression( );
    bool NeedsBuffs( );

    // Perception & Targeting
    CMap* GetMap( ) const;
    CMonster* FindNearbyMonster( float radius );
    CMonster* FindNearbyWorldBoss( float radius );
    CDrop* FindNearbyDrop( float radius );

    // Getters and Setters
    CPlayer* GetPlayer( ) const { return m_player; }
    BotState GetState( ) const { return m_state; }
    const char* GetStateString( ) const;

    bool IsBuffBot( ) const { return m_isBuffBot; }
    void SetBuffBot( bool buffBot ) { m_isBuffBot = buffBot; }

    bool IsVendingBot( ) const { return m_isVendingBot; }
    void SetVendingBot( bool vendingBot ) { m_isVendingBot = vendingBot; }
    int GetVendingCategory( ) const { return m_vendingCategory; }

    void SetFollowTarget( CPlayer* target ) { m_followTarget = target; }
    CPlayer* GetFollowTarget( ) const { return m_followTarget; }

    void SetRoamCenter( fPoint center ) { m_roamCenter = center; }
    fPoint GetRoamCenter( ) const { return m_roamCenter; }

    void SetRoamRadius( float radius ) { m_roamRadius = radius; }
    float GetRoamRadius( ) const { return m_roamRadius; }

    void SetAutoRoam( bool enable ) { m_autoRoam = enable; }
    bool IsAutoRoam( ) const { return m_autoRoam; }

    bool IsDynamic( ) const { return m_isDynamic; }
    void SetDynamic( bool dynamic ) { m_isDynamic = dynamic; }

    clock_t GetLastPlayerNearby( ) const { return m_lastPlayerNearby; }
    void SetLastPlayerNearby( clock_t t ) { m_lastPlayerNearby = t; }

    bool IsCompanion( ) const { return m_isCompanion; }
    void SetCompanion( bool isComp, CPlayer* owner = NULL ) { m_isCompanion = isComp; m_companionOwner = owner; }
    CPlayer* GetCompanionOwner( ) const { return m_companionOwner; }
    BotRole GetRole( ) const { return m_role; }
    void SetRole( BotRole role ) { m_role = role; }
    BotStance GetStance( ) const { return m_stance; }
    void SetStance( BotStance stance ) { m_stance = stance; }
    const char* GetStanceString( ) const;

    bool IsMonsterQuestTarget( CMonster* mob, CPlayer* leader );
    CMonster* FindNearbyQuestMonster( float radius );
    bool CheckSynchronizedRest( );

    void SyncLevelToLeader( int targetLevel );
    void AdjustSpecToRole( BotRole role, int level );
    static BotRole DetermineRoleFromJob( int job );

    // Phase 4: Convoy PAT, Affinity & Squad Tactics
    void SetMarkedTarget( UINT mobCid ) { m_markedTargetCid = mobCid; }
    UINT GetMarkedTarget( ) const { return m_markedTargetCid; }
    void StartPullTarget( CMonster* mob, fPoint returnPos );
    bool IsPullingTarget( ) const { return m_isPullingTarget; }

    int GetAffinity( ) const { return m_affinity; }
    void SetAffinity( int affinity ) { m_affinity = ( affinity > 100 ) ? 100 : ( ( affinity < 0 ) ? 0 : affinity ); }
    void AddAffinity( int amount );
    const char* GetAffinityTierString( ) const;
    const char* GetPersonalityString( ) const;
    bool EquipGiftedItem( int slot, const CItem& itm );
    bool IsDungeonMap( UINT mapId ) const;
    void HandleBossCleaveAndTactics( CMonster* mob );
    bool CheckMinionPeel( );
    bool CheckSpellInterrupt( CMonster* mob );

    // Phase 5: Field Services, Gambits, Mercenaries & Squad Arena
    BotTactic GetTactic( ) const { return m_tactic; }
    void SetTactic( BotTactic tactic ) { m_tactic = tactic; }
    const char* GetTacticString( ) const;

    int DoFieldAppraisal( );
    int DoFieldRepair( );
    bool DoFieldSalvage( int slot );
    void CheckAmmunitionResupply( );
    void CheckClanSync( );
    bool HandleOffTargetCrowdControl( CMonster* primaryMob );
    bool CheckPuzzleInteraction( );

    // Phase 6 / Milestone 12: Unison Ultimates, Field Crafting, Siege Warfare & Loyalty Quests
    int GetSynergy( ) const { return m_synergy; }
    void SetSynergy( int synergy ) { m_synergy = ( synergy > 100 ) ? 100 : ( ( synergy < 0 ) ? 0 : synergy ); }
    void AddSynergy( int amount );
    bool ExecuteUnisonUltimate( );

    bool DoFieldRefine( int slot );
    bool DoFieldSocket( int slot, int gemSlot );
    void HarvestReagents( CMonster* mob );

    void CheckUnionSync( );
    int GetSiegeMode( ) const { return m_siegeMode; }
    void SetSiegeMode( int mode ) { m_siegeMode = mode; }

    int GetStoryChapter( ) const;
    const char* GetStoryDialogue( ) const;
    const char* GetLoyaltyAuraName( ) const;

    bool PrepareCampfireFeast( );
    void ExecuteSquadDance( );
    void ExecuteVictoryCelebration( );

    // Phase 7 / Milestone 13: DPS Meter, Spec Loadouts, Raid Alliances, Pets & Treasures
    BotSubSpec GetSubSpec( ) const { return m_subSpec; }
    bool SetSubSpec( const char* specName );
    const char* GetSubSpecString( ) const;

    void RecordMeterDamage( long long dmg ) { m_meterDamage += dmg; }
    void RecordMeterHealing( long long heal );
    void RecordMeterDamageTaken( long long dmg ) { m_meterDamageTaken += dmg; }
    long long GetMeterDamage( ) const { return m_meterDamage; }
    long long GetMeterHealing( ) const { return m_meterHealing; }
    long long GetMeterDamageTaken( ) const { return m_meterDamageTaken; }
    void ResetMeter( );

    bool ToggleCompanionPet( );
    bool HasActivePet( ) const { return m_hasActivePet; }
    const char* GetPetName( ) const;
    void UpdateCompanionPet( );

    void CheckTreasureHunt( );
    void PlaySoundCue( int cueType );

    // Phase 8 / Milestone 14: Sparring Duels, Gambit Rules, Outfits, Talents, Inter-Bot Banter & Field Crafting
    bool StartSparringDuel( CPlayer* challenger );
    void EndSparringDuel( const char* resultMsg );
    bool IsSparring( ) const { return m_isSparring; }
    CPlayer* GetSparringChallenger( ) const { return m_sparringChallenger; }

    bool AddGambitRule( const char* cond, const char* act );
    void ClearGambitRules( );
    std::string GetGambitRulesSummary( ) const;
    bool EvaluateGambitRules( );

    bool SetOutfit( BotOutfitType outfit );
    BotOutfitType GetOutfit( ) const { return m_outfit; }
    const char* GetOutfitName( ) const;

    bool SetTalentSpec( BotTalentSpec spec );
    BotTalentSpec GetTalentSpec( ) const { return m_talentSpec; }
    const char* GetTalentSpecName( ) const;

    void CheckInterBotBanter( );
    bool FieldBrewPotions( const char* type, int count );
    bool FieldForgeAmmunition( const char* type, int count );

    // Phase 9 / Milestone 15: Natural Autonomous Life & Companion Living Dynamics
    void CheckAutonomousBodyguard( );
    void CheckLeaderLevelUpCelebration( );
    void CheckIdleDowntimeVignette( );
    void CheckStrangerAssistance( );
    void CheckNaturalGrindCycle( );
    void CheckTownLifeRoutine( );

private:
    CPlayer* m_player;
    BotState m_state;
    clock_t m_lastAiTick;
    clock_t m_stateTimer;
    clock_t m_stuckTimer;
    clock_t m_lastSkillCastTime;
    int m_lastKnownLevel;
    fPoint m_lastPos;

    // Phase 1 Timers
    clock_t m_lastHpPotionTime;
    clock_t m_lastMpPotionTime;

    // Phase 2 Timers
    clock_t m_lastChatterTime;
    clock_t m_lastEmoteTime;
    clock_t m_lastPartyInviteTime;

    // Phase 3 Timers & Navigation
    clock_t m_lastMigrationCheck;
    fPoint m_strollDest;

    // Buff Bot
    bool m_isBuffBot;
    UINT m_bonfireCid;
    clock_t m_lastBuffSay;
    clock_t m_lastBuffCastTime;
    clock_t m_lastBuffSeekTime;
    size_t m_nextBuffCandidateIndex;

    // Vending Bot
    bool m_isVendingBot;
    int m_vendingCategory;
    std::string m_shopTitle;
    clock_t m_lastVendingSay;
    clock_t m_lastBuyListRefreshTime;

    // Dynamic Spawner & Lifecycle
    bool m_isDynamic;
    clock_t m_lastPlayerNearby;

    // Roaming
    fPoint m_roamCenter;
    float m_roamRadius;
    bool m_autoRoam;

    // Follow
    CPlayer* m_followTarget;

    // Companion Party & Role
    bool m_isCompanion;
    CPlayer* m_companionOwner;
    BotRole m_role;
    BotStance m_stance;
    int m_comboStep;
    clock_t m_lastQuestScanTime;
    clock_t m_lastRestBanterTime;

    // Current Targets
    UINT m_targetMobCid;
    UINT m_targetDropCid;

    // Party Group Cooldown Timers
    clock_t m_lastPartyBuffTime;
    clock_t m_lastResurrectTime;
    clock_t m_lastTauntTime;

    // Greetings & Duels
    clock_t m_lastGreetingTime;
    UINT m_duelTargetCid;
    clock_t m_duelStartTime;

    // Advanced Realism & Personality
    BotPersonality m_personality;
    std::string m_title;
    clock_t m_lastShopBrowseTime;
    clock_t m_lastWhisperTime;
    clock_t m_lastArenaQueueTime;
    clock_t m_lastDungeonCheckTime;
    clock_t m_lastLfgShoutTime;
    clock_t m_lastBossRaidCheckTime;
    clock_t m_lastTownRepairTime;
    std::map<UINT, int> m_pvpRecord; // Player CharID -> win(+1)/loss(-1)

    // Boss Raid Assemblies & Tactics
    UINT m_rallyBossCid;
    fPoint m_rallyPos;
    clock_t m_lastRaidKiteTime;

    // Interactive Economy & Vendor Runs
    clock_t m_lastVendorRunTime;
    clock_t m_lastHaggleTime;

    // Clan System & Dynamic Rivalries
    clock_t m_lastClanRivalryTime;
    fPoint m_clanHangoutPos;

    // Dynamic Questing & Daily Bounties
    BotBounty m_bounty;
    clock_t m_lastBountyCheckTime;

    // Social Memory Engine
    std::map<UINT, int> m_friendMemory;  // Player CharID -> Gratitude Score
    std::map<UINT, int> m_nemesisMemory; // Player CharID -> Nemesis Score
    clock_t m_lastRecruitmentShoutTime;
    clock_t m_lastCasualChatterTime;

    // Phase 4: Focus Marking, Pulling, Affinity & Convoy
    UINT m_markedTargetCid;
    bool m_isPullingTarget;
    fPoint m_pullAnchorPos;
    int m_affinity;
    clock_t m_lastAffinityBanterTime;
    clock_t m_lastInterruptTime;
    clock_t m_lastConvoyMountTime;

    // Phase 5: Tactical Gambits & Field Services
    BotTactic m_tactic;
    clock_t m_lastResupplyTime;
    clock_t m_lastClanSyncTime;
    clock_t m_lastCcTime;
    clock_t m_lastPuzzleCheck;

    // Phase 6 / Milestone 12: Unison Ultimates, Field Crafting, Siege Warfare & Loyalty Quests
    int m_synergy;
    int m_siegeMode;
    clock_t m_lastUnionSyncTime;
    clock_t m_lastFeastTime;
    clock_t m_feastRestStart;

    // Phase 7 / Milestone 13: Spec Loadouts, DPS Meter, Pets & Treasures
    BotSubSpec m_subSpec;
    long long m_meterDamage;
    long long m_meterHealing;
    long long m_meterDamageTaken;
    bool m_hasActivePet;
    clock_t m_lastPetPulseTime;
    clock_t m_lastTreasureHuntTime;
    clock_t m_lastLowHpWarningTime;

    // Phase 8 / Milestone 14: Sparring, Gambits, Outfits, Talents & Banter
    bool m_isSparring;
    CPlayer* m_sparringChallenger;
    clock_t m_sparringStartTime;
    std::vector<BotGambitRule> m_customGambits;
    BotOutfitType m_outfit;
    BotTalentSpec m_talentSpec;
    clock_t m_lastInterBotBanterTime;

    // Phase 9 / Milestone 15: Natural Living State
    int m_lastLeaderLevel;
    clock_t m_leaderStationaryTime;
    fPoint m_lastLeaderStationaryPos;
    clock_t m_lastDowntimeVignette;
    int m_grindKillCount;
    clock_t m_lastStrangerAssistTime;
    clock_t m_lastTownGreetingTime;
};

class CBotManager {
public:
    static CBotManager* GetInstance( );

    void Update( ); // Called periodically from MapProcess

    CPlayer* SpawnBot( const char* name, int job, int level, int mapId, fPoint pos, bool isDynamic = false );
    CPlayer* SpawnBuffBot( const char* name, int mapId, fPoint pos );
    CPlayer* SpawnVendingBot( const char* name, int job, int level, int mapId, fPoint pos, const char* shopTitle, int category );
    bool RemoveBot( const char* name );
    void RemoveAllBots( );

    CPlayerBot* GetBot( const char* name );
    CPlayerBot* GetBotByPlayer( CPlayer* player );
    size_t GetBotCount( );
    std::vector<CPlayerBot*> GetBots( );

    // Dynamic Proximity Spawner & World Population
    void CheckProximitySpawns( );
    void CheckAmbientPopulation( );
    void CheckBuffBots( );
    void CheckVendingBots( );
    CPlayerBot* FindNearbyBuffBot( UINT mapId, fPoint pos, float radius );
    void PruneOrphanedBots( );
    std::string GenerateUniqueName( );

    bool HasRealPlayerNearby( CPlayer* bot, float radius );
    bool HasRealPlayerInParty( CPlayer* bot );

    // Zone & Party Teleportation Helpers
    static void GetZoneLevelRange( UINT mapId, int& minLvl, int& maxLvl, bool& isCity );
    static bool GetMapForLevel( int botLvl, UINT& mapId, fPoint& spawnPos );
    void CheckAndRelocateUninvitedBot( CPlayer* bot );

    // Auto Companion Party System
    static BotRole GetPlayerRole( CPlayer* player );
    void EnsurePlayerCompanionParty( CPlayer* realPlayer );
    void CheckPartyRoleBalance( CPlayer* realPlayer );
    void RefillPartyCompanions( CPlayer* realPlayer );
    CPlayer* SpawnCompanionBot( const char* name, BotRole role, int level, int mapId, fPoint pos, CPlayer* leader );
    bool RecruitMercenary( CPlayer* leader, const char* mercName );
    void CheckPlayerCompanions( );
    void SendPartyFullUpdate( CParty* party );

    // Milestone 13: Raid Alliances & Dungeon Living World
    bool SpawnRaidAlliance( CPlayer* leader );
    bool DismissRaidAlliance( CPlayer* leader );
    void CheckDungeonAutonomousParties( );

    // Milestone 14: Mercenary Dispatch Missions
    bool StartMercenaryDispatch( CPlayer* player, const char* missionId, const char* mercName );
    bool ClaimMercenaryDispatch( CPlayer* player );
    std::string GetDispatchStatus( CPlayer* player );

private:
    CBotManager( );
    ~CBotManager( );

    static CBotManager* m_instance;
    std::vector<CPlayerBot*> m_bots;
    pthread_mutex_t m_botMutex;

    clock_t m_lastProximityCheck;
    clock_t m_lastAmbientCheck;
    clock_t m_lastPruneCheck;
    clock_t m_lastBuffBotCheck;
    clock_t m_lastVendingBotCheck;
    clock_t m_lastCompanionCheck;
    clock_t m_lastDungeonPartyCheck;
    bool m_ambientInitialized;
    bool m_buffBotsInitialized;
    bool m_vendingBotsInitialized;
    int m_nameCounter;
    std::map<std::string, clock_t> m_recentNames;
    std::map<UINT, BotDispatchMission> m_playerDispatches;
};

#endif // _PLAYER_BOT_H_
