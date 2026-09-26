/*
    RosE Resurrected — Akram Arena / Colosseum Match Manager Implementation
*/
#include "Arena.h"
#include "worldserver.h"
#include "player.h"
#include <cstdarg>
#include <cstdio>
#include <cmath>

CArenaManager* CArenaManager::m_instance = NULL;

CArenaManager* CArenaManager::GetInstance( )
{
    if (m_instance == NULL)
    {
        m_instance = new CArenaManager( );
    }
    return m_instance;
}

CArenaManager::CArenaManager( )
{
    m_state = ARENA_STATE_IDLE;
    m_stateTimer = 0;
    m_roundDuration = 300; // 5 minutes
    m_maxKills = 10;
    m_team1Score = 0;
    m_team2Score = 0;
    m_lastNoticeTime = 0;
    m_lastReportedRemaining = -1;
    m_players.clear();
}

CArenaManager::~CArenaManager( )
{
    m_players.clear();
}

SArenaPlayer* CArenaManager::FindArenaPlayer( UINT clientid )
{
    for (size_t i = 0; i < m_players.size(); i++)
    {
        if (m_players[i].clientid == clientid)
            return &m_players[i];
    }
    return NULL;
}

int CArenaManager::AssignTeam( CPlayer* player )
{
    if (player == NULL) return 1;

    int team1Count = 0;
    int team2Count = 0;

    for (size_t i = 0; i < m_players.size(); i++)
    {
        if (m_players[i].team == 1) team1Count++;
        else if (m_players[i].team == 2) team2Count++;
    }

    int assignedTeam = (team1Count <= team2Count) ? 1 : 2;
    player->pvp_id = assignedTeam;
    return assignedTeam;
}

bool CArenaManager::JoinArena( CPlayer* player )
{
    if (player == NULL || player->Session == NULL || !player->Session->inGame)
        return false;

    if (player->Position->Map == 9)
    {
        GServer->SendPM( player, "[Akram Arena] You are already inside the Arena!" );
        return true;
    }

    int team = AssignTeam( player );

    SArenaPlayer* entry = FindArenaPlayer( player->clientid );
    if (entry == NULL)
    {
        SArenaPlayer p;
        p.clientid = player->clientid;
        p.team = team;
        p.kills = 0;
        p.deaths = 0;
        p.isDead = false;
        p.respawnTime = 0;
        m_players.push_back( p );
    }
    else
    {
        entry->team = team;
        entry->isDead = false;
    }

    // Teleport player to team base in Map 9
    fPoint spawnCoord;
    spawnCoord.z = 0.0f;
    if (team == 1)
    {
        spawnCoord.x = 5211.4f;
        spawnCoord.y = 4867.9f;
    }
    else
    {
        spawnCoord.x = 5200.51f;
        spawnCoord.y = 4725.06f;
    }

    player->Position->destiny = spawnCoord;
    GServer->MapList.Index[9]->TeleportPlayer( player, spawnCoord, false );

    const char* teamName = (team == 1) ? "RED" : "BLUE";
    GServer->SendPM( player, "[Akram Arena] Welcome! You have joined the %s Team (Team ID %d).", teamName, team );
    BroadcastArenaNotice( "[Akram Arena] %s has entered the arena on %s Team!", player->CharInfo->charname, teamName );

    return true;
}

bool CArenaManager::LeaveArena( CPlayer* player )
{
    if (player == NULL) return false;

    for (std::vector<SArenaPlayer>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if (it->clientid == player->clientid)
        {
            m_players.erase( it );
            break;
        }
    }

    player->pvp_id = -1;

    // Teleport out to Junon Polis
    fPoint exitCoord;
    exitCoord.x = 5485.0f;
    exitCoord.y = 5260.0f;
    exitCoord.z = 0.0f;
    player->Position->destiny = exitCoord;
    GServer->MapList.Index[2]->TeleportPlayer( player, exitCoord, false );

    GServer->SendPM( player, "[Akram Arena] You have left Akram Arena." );
    return true;
}

void CArenaManager::OnPlayerDisconnect( CPlayer* player )
{
    if (player == NULL) return;

    for (std::vector<SArenaPlayer>::iterator it = m_players.begin(); it != m_players.end(); ++it)
    {
        if (it->clientid == player->clientid)
        {
            m_players.erase( it );
            break;
        }
    }
}

void CArenaManager::OnPlayerKill( CPlayer* killer, CPlayer* victim )
{
    if (victim == NULL) return;

    SArenaPlayer* victimEntry = FindArenaPlayer( victim->clientid );
    if (victimEntry != NULL)
    {
        victimEntry->deaths++;
        victimEntry->isDead = true;
        victimEntry->respawnTime = time(NULL) + 5; // Fast 5-second respawn at base
    }

    if (m_state != ARENA_STATE_ACTIVE)
        return;

    if (killer != NULL && killer->IsPlayer())
    {
        SArenaPlayer* killerEntry = FindArenaPlayer( killer->clientid );
        if (killerEntry != NULL)
        {
            killerEntry->kills++;
        }

        int killerTeam = (killerEntry != NULL) ? killerEntry->team : killer->pvp_id;
        int victimTeam = (victimEntry != NULL) ? victimEntry->team : victim->pvp_id;

        if (killerTeam == 1) m_team1Score++;
        else if (killerTeam == 2) m_team2Score++;

        const char* kTeam = (killerTeam == 1) ? "Red" : "Blue";
        const char* vTeam = (victimTeam == 1) ? "Red" : "Blue";

        BroadcastArenaNotice( "[Arena] %s (Team %s) defeated %s (Team %s)! [Score — Red: %d | Blue: %d]",
            killer->CharInfo->charname, kTeam, victim->CharInfo->charname, vTeam, m_team1Score, m_team2Score );

        // Check Kill Target limit
        if (m_team1Score >= m_maxKills)
        {
            EndMatch( 1 );
        }
        else if (m_team2Score >= m_maxKills)
        {
            EndMatch( 2 );
        }
    }
}

int CArenaManager::GetRemainingTime( ) const
{
    if (m_state != ARENA_STATE_ACTIVE) return 0;
    time_t now = time(NULL);
    if (now >= m_stateTimer) return 0;
    return (int)(m_stateTimer - now);
}

void CArenaManager::StartMatch( )
{
    m_state = ARENA_STATE_ACTIVE;
    m_stateTimer = time(NULL) + m_roundDuration;
    m_team1Score = 0;
    m_team2Score = 0;
    m_lastReportedRemaining = -1;

    CMap* arenaMap = GServer->MapList.Index[9];
    if (arenaMap != NULL)
    {
        for (UINT i = 0; i < arenaMap->PlayerList.size(); i++)
        {
            CPlayer* pl = arenaMap->PlayerList.at(i);
            if (pl == NULL || !pl->Session->inGame) continue;

            SArenaPlayer* entry = FindArenaPlayer( pl->clientid );
            int team = (entry != NULL) ? entry->team : pl->pvp_id;
            if (team <= 0) team = AssignTeam( pl );

            pl->pvp_id = team;
            pl->Stats->HP = pl->Stats->MaxHP;
            pl->Stats->MP = pl->Stats->MaxMP;

            RespawnPlayerAtBase( pl, team );
        }
    }

    BroadcastArenaNotice( "========================================" );
    BroadcastArenaNotice( "[Akram Arena] --- THE BATTLE HAS BEGUN! ---" );
    BroadcastArenaNotice( "[Akram Arena] Red Team vs Blue Team! First to %d kills or 5-min round.", m_maxKills );
    BroadcastArenaNotice( "========================================" );
}

void CArenaManager::EndMatch( int winningTeam )
{
    m_state = ARENA_STATE_ENDED;
    m_stateTimer = time(NULL) + 15; // 15 seconds intermission

    BroadcastArenaNotice( "========================================" );
    if (winningTeam == 1)
    {
        BroadcastArenaNotice( "[Akram Arena] MATCH FINISHED! RED TEAM IS VICTORIOUS! [Score: %d - %d]", m_team1Score, m_team2Score );
    }
    else if (winningTeam == 2)
    {
        BroadcastArenaNotice( "[Akram Arena] MATCH FINISHED! BLUE TEAM IS VICTORIOUS! [Score: %d - %d]", m_team2Score, m_team1Score );
    }
    else
    {
        BroadcastArenaNotice( "[Akram Arena] MATCH FINISHED! IT IS A DRAW! [Score: %d - %d]", m_team1Score, m_team2Score );
    }
    BroadcastArenaNotice( "========================================" );

    // Award Rewards
    CMap* arenaMap = GServer->MapList.Index[9];
    if (arenaMap != NULL)
    {
        for (UINT i = 0; i < arenaMap->PlayerList.size(); i++)
        {
            CPlayer* pl = arenaMap->PlayerList.at(i);
            if (pl == NULL || !pl->Session->inGame) continue;

            SArenaPlayer* entry = FindArenaPlayer( pl->clientid );
            int playerTeam = (entry != NULL) ? entry->team : pl->pvp_id;

            uint64_t rewardZulies = 0;
            uint64_t rewardExp = 0;

            if (playerTeam == winningTeam && winningTeam != 0)
            {
                rewardZulies = 50000;
                rewardExp = 50000;
                GServer->SendPM( pl, "[Akram Arena] VICTORY REWARD: +50,000 Zulies, +50,000 EXP!" );
            }
            else if (winningTeam == 0)
            {
                rewardZulies = 25000;
                rewardExp = 25000;
                GServer->SendPM( pl, "[Akram Arena] DRAW REWARD: +25,000 Zulies, +25,000 EXP." );
            }
            else
            {
                rewardZulies = 15000;
                rewardExp = 15000;
                GServer->SendPM( pl, "[Akram Arena] CONSOLATION REWARD: +15,000 Zulies, +15,000 EXP." );
            }

            pl->CharInfo->Zulies += rewardZulies;
            pl->CharInfo->Exp += rewardExp;

            // Send updated stats to client
            BEGINPACKET( pak, 0x79b );
            ADDDWORD( pak, pl->CharInfo->Exp );
            ADDWORD( pak, pl->CharInfo->stamina );
            ADDWORD( pak, 0 );
            pl->client->SendPacket( &pak );

            BEGINPACKET( pakZuly, 0x71d );
            ADDQWORD( pakZuly, pl->CharInfo->Zulies );
            pl->client->SendPacket( &pakZuly );
        }
    }
}

void CArenaManager::ResetArena( )
{
    m_state = ARENA_STATE_IDLE;
    m_stateTimer = 0;
    m_team1Score = 0;
    m_team2Score = 0;
    m_lastReportedRemaining = -1;

    for (size_t i = 0; i < m_players.size(); i++)
    {
        m_players[i].kills = 0;
        m_players[i].deaths = 0;
        m_players[i].isDead = false;
        m_players[i].respawnTime = 0;
    }
}

void CArenaManager::RespawnPlayerAtBase( CPlayer* player, int team )
{
    if (player == NULL) return;

    fPoint spawnCoord;
    spawnCoord.z = 0.0f;
    if (team == 1)
    {
        spawnCoord.x = 5211.4f;
        spawnCoord.y = 4867.9f;
    }
    else
    {
        spawnCoord.x = 5200.51f;
        spawnCoord.y = 4725.06f;
    }

    player->Stats->HP = player->Stats->MaxHP;
    player->Stats->MP = player->Stats->MaxMP;
    player->Position->destiny = spawnCoord;
    GServer->MapList.Index[9]->TeleportPlayer( player, spawnCoord, false );

    BEGINPACKET( pak, 0x79f );
    ADDWORD( pak, player->clientid );
    ADDDWORD( pak, player->Stats->HP );
    player->client->SendPacket( &pak );
    GServer->SendToVisible( &pak, player );

    GServer->SendPM( player, "[Akram Arena] You have respawned at your team base with full HP/MP!" );
}

void CArenaManager::BroadcastArenaMessage( const char* message )
{
    if (message == NULL) return;
    CMap* arenaMap = GServer->MapList.Index[9];
    if (arenaMap == NULL) return;

    for (UINT i = 0; i < arenaMap->PlayerList.size(); i++)
    {
        CPlayer* pl = arenaMap->PlayerList.at(i);
        if (pl != NULL && pl->Session->inGame)
        {
            GServer->SendPM( pl, "%s", message );
        }
    }
}

void CArenaManager::BroadcastArenaNotice( const char* format, ... )
{
    char buffer[512];
    va_list args;
    va_start( args, format );
    vsnprintf( buffer, sizeof(buffer), format, args );
    va_end( args );

    BroadcastArenaMessage( buffer );
}

void CArenaManager::Update( )
{
    CMap* arenaMap = GServer->MapList.Index[9];
    if (arenaMap == NULL) return;

    time_t now = time(NULL);

    // Sync active players in Map 9 with m_players
    for (UINT i = 0; i < arenaMap->PlayerList.size(); i++)
    {
        CPlayer* pl = arenaMap->PlayerList.at(i);
        if (pl == NULL || !pl->Session->inGame) continue;

        SArenaPlayer* entry = FindArenaPlayer( pl->clientid );
        if (entry == NULL)
        {
            SArenaPlayer p;
            p.clientid = pl->clientid;
            p.team = (pl->pvp_id > 0) ? pl->pvp_id : AssignTeam( pl );
            p.kills = 0;
            p.deaths = 0;
            p.isDead = false;
            p.respawnTime = 0;
            m_players.push_back( p );
        }
        else if (pl->pvp_id <= 0)
        {
            pl->pvp_id = entry->team;
        }
    }

    // Prune players who left Map 9
    for (std::vector<SArenaPlayer>::iterator it = m_players.begin(); it != m_players.end(); )
    {
        CPlayer* pl = GServer->GetClientByID( it->clientid, 9 );
        if (pl == NULL || pl->Position->Map != 9)
        {
            it = m_players.erase( it );
        }
        else
        {
            ++it;
        }
    }

    // Arena State Machine
    switch (m_state)
    {
        case ARENA_STATE_IDLE:
        {
            // Auto-start countdown if there are at least 2 players on opposing teams
            int redCount = 0;
            int blueCount = 0;
            for (size_t i = 0; i < m_players.size(); i++)
            {
                if (m_players[i].team == 1) redCount++;
                else if (m_players[i].team == 2) blueCount++;
            }

            if (redCount >= 1 && blueCount >= 1)
            {
                m_state = ARENA_STATE_COUNTDOWN;
                m_stateTimer = now + 20; // 20-second countdown
                BroadcastArenaNotice( "[Akram Arena] Opposing fighters ready! Match begins in 20 seconds." );
            }
        }
        break;

        case ARENA_STATE_COUNTDOWN:
        {
            int remaining = (int)(m_stateTimer - now);
            if (remaining == 10 || remaining == 5)
            {
                if (now != m_lastNoticeTime)
                {
                    BroadcastArenaNotice( "[Akram Arena] Match starting in %d seconds!", remaining );
                    m_lastNoticeTime = now;
                }
            }
            if (remaining <= 0)
            {
                StartMatch( );
            }
        }
        break;

        case ARENA_STATE_ACTIVE:
        {
            int rem = GetRemainingTime( );
            if (rem == 180 || rem == 120 || rem == 60 || rem == 30 || rem == 10)
            {
                if (rem != m_lastReportedRemaining)
                {
                    BroadcastArenaNotice( "[Akram Arena] %d seconds remaining! Current Score: Red %d - %d Blue",
                        rem, m_team1Score, m_team2Score );
                    m_lastReportedRemaining = rem;
                }
            }

            if (rem <= 0)
            {
                // Round timeout: determine victor
                if (m_team1Score > m_team2Score) EndMatch( 1 );
                else if (m_team2Score > m_team1Score) EndMatch( 2 );
                else EndMatch( 0 ); // Draw
                return;
            }

            // Process automated base respawns
            for (size_t i = 0; i < m_players.size(); i++)
            {
                if (m_players[i].isDead && now >= m_players[i].respawnTime)
                {
                    CPlayer* pl = GServer->GetClientByID( m_players[i].clientid, 9 );
                    if (pl != NULL)
                    {
                        RespawnPlayerAtBase( pl, m_players[i].team );
                        m_players[i].isDead = false;
                    }
                }
            }
        }
        break;

        case ARENA_STATE_ENDED:
        {
            if (now >= m_stateTimer)
            {
                ResetArena( );
                BroadcastArenaNotice( "[Akram Arena] Arena is now open for new challengers." );
            }
        }
        break;
    }
}
