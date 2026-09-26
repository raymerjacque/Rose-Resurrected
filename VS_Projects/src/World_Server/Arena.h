/*
    RosE Resurrected — Akram Arena / Colosseum Match Manager
    Handles automated Arena rounds, team assignment (Red vs Blue),
    deathmatch scoring, fast base respawns without EXP penalty, and rewards.
*/
#ifndef __ARENA_H__
#define __ARENA_H__

#include <vector>
#include <string>
#include <ctime>
#include "datatypes.h"

class CPlayer;

enum eArenaState {
    ARENA_STATE_IDLE       = 0,
    ARENA_STATE_COUNTDOWN  = 1,
    ARENA_STATE_ACTIVE     = 2,
    ARENA_STATE_ENDED      = 3
};

struct SArenaPlayer {
    UINT clientid;
    int team;             // 1 = Red, 2 = Blue
    UINT kills;
    UINT deaths;
    bool isDead;
    time_t respawnTime;
};

class CArenaManager {
public:
    static CArenaManager* GetInstance( );

    void Update( );

    bool JoinArena( CPlayer* player );
    bool LeaveArena( CPlayer* player );
    void OnPlayerKill( CPlayer* killer, CPlayer* victim );
    void OnPlayerDisconnect( CPlayer* player );

    void StartMatch( );
    void EndMatch( int winningTeam );
    void ResetArena( );

    void BroadcastArenaMessage( const char* message );
    void BroadcastArenaNotice( const char* format, ... );

    eArenaState GetState( ) const { return m_state; }
    int GetScore( int team ) const { return (team == 1) ? m_team1Score : (team == 2) ? m_team2Score : 0; }
    int GetRemainingTime( ) const;

    SArenaPlayer* FindArenaPlayer( UINT clientid );

private:
    CArenaManager( );
    ~CArenaManager( );

    static CArenaManager* m_instance;

    eArenaState m_state;
    time_t m_stateTimer;
    int m_roundDuration; // seconds (default: 300s = 5 minutes)
    int m_maxKills;      // kill score target (default: 10)

    int m_team1Score;    // Red team
    int m_team2Score;    // Blue team

    time_t m_lastNoticeTime;
    int m_lastReportedRemaining;

    std::vector<SArenaPlayer> m_players;

    int AssignTeam( CPlayer* player );
    void RespawnPlayerAtBase( CPlayer* player, int team );
};

#endif // __ARENA_H__
