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
    BOT_STATE_VENDING
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

    // Progression, Skills & Equipment
    void EquipTieredGear( bool broadcast = true );
    void UpdateSkills( );
    bool CastCombatSkill( CCharacter* target );
    bool CheckPartyHeal( );
    bool CheckPartyBuffs( );
    bool CheckPartyResurrect( );
    bool CheckPartyTaunt( );
    fPoint GetFormationOffset( int slotIndex, fPoint leaderCurrent, fPoint leaderDest );
    int GetPartySlotIndex( ) const;
    void ForcePartyBuff( );
    void CheckProgression( );
    bool NeedsBuffs( );

    // Perception & Targeting
    CMap* GetMap( ) const;
    CMonster* FindNearbyMonster( float radius );
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

private:
    CPlayer* m_player;
    BotState m_state;
    clock_t m_lastAiTick;
    clock_t m_stateTimer;
    clock_t m_stuckTimer;
    clock_t m_lastSkillCastTime;
    int m_lastKnownLevel;
    fPoint m_lastPos;

    // Buff Bot
    bool m_isBuffBot;
    UINT m_bonfireCid;
    clock_t m_lastBuffSay;
    clock_t m_lastBuffCastTime;
    clock_t m_lastBuffSeekTime;

    // Vending Bot
    bool m_isVendingBot;
    int m_vendingCategory;
    std::string m_shopTitle;
    clock_t m_lastVendingSay;

    // Dynamic Spawner & Lifecycle
    bool m_isDynamic;
    clock_t m_lastPlayerNearby;

    // Roaming
    fPoint m_roamCenter;
    float m_roamRadius;
    bool m_autoRoam;

    // Follow
    CPlayer* m_followTarget;

    // Current Targets
    UINT m_targetMobCid;
    UINT m_targetDropCid;

    // Party Group Cooldown Timers
    clock_t m_lastPartyBuffTime;
    clock_t m_lastResurrectTime;
    clock_t m_lastTauntTime;
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
    bool m_ambientInitialized;
    bool m_buffBotsInitialized;
    bool m_vendingBotsInitialized;
    int m_nameCounter;
    std::map<std::string, clock_t> m_recentNames;
};

#endif // _PLAYER_BOT_H_
