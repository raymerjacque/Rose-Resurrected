#include "PlayerBot.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>

// -------------------------------------------------------------
// Authentic Player Name Pool
// -------------------------------------------------------------
static const char* s_botNames[] = {
    "Aero", "Blaze", "Viper", "Phoenix", "Valkyrie", "Shadow", "Nightfall", "Frost", "Tempest", "Zephyr",
    "Titan", "Aegis", "Vanguard", "RangerDan", "Sarah", "Kael", "GhostBlade", "Nova", "Zenith", "StarGazer",
    "SilverWolf", "IronHide", "HawkEye", "StormRider", "Solaris", "Lunaria", "Crimson", "DarkKnight", "SwiftArrow",
    "BountyHunter", "SpellBinder", "ArcaneLord", "BladeDancer", "IronClad", "WindWalker", "Thunder", "MysticRose", "DeathDealer", "SoulKeeper",
    "LightBringer", "FrostBite", "FireStorm", "CrystalMage", "WarMaster", "Dragoon", "SilentKill", "SniperWolf", "PaladinArt",
    "AuraKnight", "WildHunter", "ChronoMage", "Astra", "Echo", "Raven", "Dante", "Vergil", "Cloud", "Squall",
    "Tifa", "Aerith", "Sephiroth", "Geralt", "Yennefer", "Triss", "Ciri", "Arthas", "Jaina", "Illidan",
    "Sylvanas", "Thrall", "Anduin", "Tyrande", "Malfurion", "Rexxar", "Varian", "Garrosh", "Guldan",
    "Alucard", "Richter", "Simon", "Trevor", "Sypha", "Leon", "Claire", "Jill", "Chris", "Wesker",
    "Ryu", "Ken", "ChunLi", "Guile", "Akuma", "SubZero", "Scorpion", "Raiden", "LiuKang", "KungLao",
    "Kratos", "Atreus", "Freya", "Thor", "Odin", "Loki", "Baldur", "Tyr", "Heimdall", "Mimir",
    "Arthur", "Lancelot", "Gawain", "Galahad", "Percival", "Merlin", "Morgana", "Mordred", "Bedivere", "Tristan",
    "Zack", "Rikku", "Yuna", "Tidus", "Auron", "Noctis", "Prompto", "Ignis", "Gladiolus", "Aranea",
    "Cor", "Iris", "Ardyn", "Celes", "Terra", "Locke", "Edgar", "Sabin", "Cyan", "Setzer",
    "Strago", "Relm", "Mog", "Umaro", "Gogo", "Cecil", "Kain", "Rosa", "Rydia", "Edge",
    "Lyra", "Elysia", "Rowan", "Selene", "Theron", "Aethel", "Vesper", "Morrigan", "Fenris", "Hawke",
    "Alistair", "Leliana", "Wynne", "Sten", "Zevran", "Cassandra", "Varric", "Solas", "Cullen", "Dorian",
    "Sora", "Riku", "Kairi", "Roxas", "Axel", "Xion", "Ventus", "Aqua", "TerraX", "Ansem",
    "Grom", "Voljin", "Baine", "LorThemar", "Nathanos", "Taelia", "Flynn", "Shaw", "Valeera", "Brann",
    "Khadgar", "Medivh", "Alleria", "Turalyon", "Bolvar", "Uther", "Muradin", "Magni", "Falstad", "Kurdran"
};
static const size_t s_botNamesCount = sizeof( s_botNames ) / sizeof( s_botNames[0] );

// -------------------------------------------------------------
// CBotManager Singleton Implementation
// -------------------------------------------------------------
CBotManager* CBotManager::m_instance = NULL;

CBotManager* CBotManager::GetInstance( )
{
    if ( !m_instance )
    {
        m_instance = new CBotManager( );
    }
    return m_instance;
}

CBotManager::CBotManager( )
    : m_ambientInitialized( false ),
      m_buffBotsInitialized( false ),
      m_nameCounter( 0 )
{
    pthread_mutex_init( &m_botMutex, NULL );
    m_lastProximityCheck = clock( );
    m_lastAmbientCheck = clock( );
    m_lastPruneCheck = clock( );
    m_lastBuffBotCheck = clock( );
}

CBotManager::~CBotManager( )
{
    RemoveAllBots( );
    pthread_mutex_destroy( &m_botMutex );
}

std::string CBotManager::GenerateUniqueName( )
{
    pthread_mutex_lock( &m_botMutex );
    clock_t now = clock( );
    // Purge names older than 30 minutes to prevent model cache collisions in client
    clock_t timeout = 30 * 60 * CLOCKS_PER_SEC;
    std::map<std::string, clock_t>::iterator it = m_recentNames.begin( );
    while ( it != m_recentNames.end( ) )
    {
        if ( ( now - it->second ) > timeout )
        {
            m_recentNames.erase( it++ );
        }
        else
        {
            ++it;
        }
    }

    for ( int attempts = 0; attempts < 100; attempts++ )
    {
        int idx = rand( ) % (int)s_botNamesCount;
        char buf[32];
        if ( attempts < 40 )
        {
            snprintf( buf, sizeof(buf), "%s", s_botNames[idx] );
        }
        else
        {
            snprintf( buf, sizeof(buf), "%s%d", s_botNames[idx], (m_nameCounter++) % 90 + 10 );
        }
        buf[15] = '\0'; // Max 16 chars including null

        bool inUse = false;
        // Check active bots in world
        for ( size_t i = 0; i < m_bots.size( ); i++ )
        {
            if ( m_bots[i] && m_bots[i]->GetPlayer( ) && m_bots[i]->GetPlayer( )->CharInfo )
            {
                if ( strncmp( m_bots[i]->GetPlayer( )->CharInfo->charname, buf, 16 ) == 0 )
                {
                    inUse = true;
                    break;
                }
            }
        }
        // Check recently despawned bot names
        if ( !inUse && m_recentNames.find( buf ) != m_recentNames.end( ) )
        {
            inUse = true;
        }

        if ( !inUse )
        {
            m_recentNames[buf] = now;
            pthread_mutex_unlock( &m_botMutex );
            return std::string( buf );
        }
    }
    char fallback[32];
    snprintf( fallback, sizeof(fallback), "RoseBot%d", (m_nameCounter++) % 900 + 100 );
    m_recentNames[fallback] = now;
    pthread_mutex_unlock( &m_botMutex );
    return std::string( fallback );
}

bool CBotManager::HasRealPlayerNearby( CPlayer* bot, float radius )
{
    if ( !bot || !bot->Position || bot->Position->Map >= (UINT)GServer->MapList.max ) return false;
    CMap* map = GServer->MapList.Index[bot->Position->Map];
    if ( !map || map == GServer->MapList.nullzone ) return false;

    for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( p && !p->is_bot && p->Session->inGame && p->Position )
        {
            if ( GServer->distance( bot->Position->current, p->Position->current ) <= radius )
            {
                return true;
            }
        }
    }
    return false;
}

bool CBotManager::HasRealPlayerInParty( CPlayer* bot )
{
    if ( !bot || !bot->Party || !bot->Party->party ) return false;
    CParty* party = bot->Party->party;
    for ( UINT i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* m = party->Members[i];
        if ( m && !m->is_bot && m->Session->inGame )
        {
            return true;
        }
    }
    return false;
}

void CBotManager::Update( )
{
    // 1. Update AI for all bots
    pthread_mutex_lock( &m_botMutex );
    std::vector<CPlayerBot*> botsCopy = m_bots;
    pthread_mutex_unlock( &m_botMutex );

    for ( size_t i = 0; i < botsCopy.size( ); i++ )
    {
        if ( botsCopy[i] )
        {
            botsCopy[i]->Update( );
        }
    }

    // 2. Ambient world population check
    CheckAmbientPopulation( );

    // 3. Persistent grind spot Buff Bots check
    CheckBuffBots( );

    // 4. Dynamic proximity spawner check
    CheckProximitySpawns( );

    // 5. Prune orphaned dynamic bots
    PruneOrphanedBots( );
}

void CBotManager::CheckAmbientPopulation( )
{
    clock_t now = clock( );
    if ( m_ambientInitialized )
    {
        if ( ( now - m_lastAmbientCheck ) < ( 30 * CLOCKS_PER_SEC ) ) return;
    }
    m_lastAmbientCheck = now;
    m_ambientInitialized = true;

    struct AmbientHub {
        int mapId;
        float x;
        float y;
        int minLvl;
        int maxLvl;
        int targetCount;
    };

    static const AmbientHub hubs[] = {
        { 22, 5839.76f, 5332.16f,  2,  8, 8 }, // Adventurer Plains (Start area)
        { 22, 5303.62f, 5099.92f,  6, 12, 6 }, // Adventurer Plains (Jelly / Choropy)
        { 22, 5080.18f, 5331.80f,  8, 14, 6 }, // Adventurer Plains (Pomic / Woopie)
        {  1, 5240.72f, 5189.85f, 10, 22, 8 }, // Zant (City & outskirts)
        { 21, 5102.48f, 5063.67f, 15, 28, 6 }, // Luxem Tower
        { 23, 5096.35f, 4905.92f, 22, 36, 6 }, // Breezy Hills
        { 25, 5379.49f, 5184.52f, 30, 48, 6 }, // Anima Lake
        {  2, 5655.32f, 5238.22f, 40, 75, 8 }  // Junon Polis
    };
    static const size_t hubCount = sizeof( hubs ) / sizeof( hubs[0] );

    for ( size_t h = 0; h < hubCount; h++ )
    {
        const AmbientHub& hub = hubs[h];
        if ( hub.mapId >= (UINT)GServer->MapList.max ) continue;
        CMap* map = GServer->MapList.Index[hub.mapId];
        if ( !map || map == GServer->MapList.nullzone ) continue;

        // Count existing ambient bots within 120m of this hub
        int count = 0;
        pthread_mutex_lock( &m_botMutex );
        for ( size_t i = 0; i < m_bots.size( ); i++ )
        {
            CPlayerBot* bAi = m_bots[i];
            if ( bAi && bAi->GetPlayer( ) && bAi->GetPlayer( )->Position )
            {
                if ( bAi->GetPlayer( )->Position->Map == hub.mapId )
                {
                    fPoint hubPos = { hub.x, hub.y, 0 };
                    if ( GServer->distance( bAi->GetPlayer( )->Position->current, hubPos ) <= 120.0f )
                    {
                        count++;
                    }
                }
            }
        }
        pthread_mutex_unlock( &m_botMutex );

        if ( count < hub.targetCount && GetBotCount( ) < 100 )
        {
            int lvl = hub.minLvl + ( rand( ) % ( hub.maxLvl - hub.minLvl + 1 ) );
            int job = 0;
            if ( lvl >= 70 )
            {
                int r = rand( ) % 4;
                int sub = rand( ) % 2;
                if ( r == 0 ) job = (sub == 0) ? 121 : 122; // Knight / Champion
                else if ( r == 1 ) job = (sub == 0) ? 221 : 222; // Mage / Cleric
                else if ( r == 2 ) job = (sub == 0) ? 321 : 322; // Raider / Scout
                else job = (sub == 0) ? 421 : 422; // Bourgeois / Artisan
            }
            else if ( lvl >= 10 )
            {
                int r = rand( ) % 4;
                if ( r == 0 ) job = 111;
                else if ( r == 1 ) job = 211;
                else if ( r == 2 ) job = 311;
                else job = 411;
            }

            float angle = ( (float)( rand( ) % 360 ) ) * ( 3.14159265f / 180.0f );
            float dist = 5.0f + ( (float)( rand( ) % 30 ) );
            fPoint spawnPos;
            spawnPos.x = hub.x + cos( angle ) * dist;
            spawnPos.y = hub.y + sin( angle ) * dist;
            spawnPos.z = 0;

            std::string name = GenerateUniqueName( );
            SpawnBot( name.c_str( ), job, lvl, hub.mapId, spawnPos, false );
        }
    }
}

void CBotManager::CheckBuffBots( )
{
    clock_t now = clock( );
    if ( m_buffBotsInitialized )
    {
        if ( ( now - m_lastBuffBotCheck ) < ( 30 * CLOCKS_PER_SEC ) ) return;
    }
    m_lastBuffBotCheck = now;
    m_buffBotsInitialized = true;

    struct BuffBotLocation {
        const char* name;
        int mapId;
        float x;
        float y;
    };

    static const BuffBotLocation spots[] = {
        { "SisterAngela",   2, 5509.0f, 5485.0f }, // Junon Polis (East Gate Tree - User's exact spot!)
        { "SisterMaria",   22, 5303.6f, 5100.0f }, // Adventurer Plains (Jelly / Choropy Tree)
        { "SisterTeresa",   1, 5240.7f, 5190.0f }, // Canyon City of Zant (South Outskirts)
        { "SisterClaire",  21, 5102.5f, 5063.7f }, // Valley of Lux (Luxem Tower Entrance)
        { "SisterRose",    23, 5096.4f, 4905.9f }, // Breezy Hills (Crossroads / Gate)
        { "SisterGrace",   25, 5379.5f, 5184.5f }, // Anima Lake (Lakeshore Campsite)
        { "SisterFaith",   27, 5771.0f, 5302.0f }  // Kenji Beach (Beach Path)
    };
    static const size_t spotCount = sizeof( spots ) / sizeof( spots[0] );

    for ( size_t s = 0; s < spotCount; s++ )
    {
        const BuffBotLocation& loc = spots[s];
        if ( loc.mapId >= (UINT)GServer->MapList.max ) continue;
        CMap* map = GServer->MapList.Index[loc.mapId];
        if ( !map || map == GServer->MapList.nullzone ) continue;

        // Check if this buff bot already exists
        bool exists = false;
        pthread_mutex_lock( &m_botMutex );
        for ( size_t i = 0; i < m_bots.size( ); i++ )
        {
            CPlayerBot* bAi = m_bots[i];
            if ( bAi && bAi->GetPlayer( ) && bAi->GetPlayer( )->CharInfo )
            {
                if ( strcmp( bAi->GetPlayer( )->CharInfo->charname, loc.name ) == 0 )
                {
                    exists = true;
                    break;
                }
            }
        }
        pthread_mutex_unlock( &m_botMutex );

        if ( !exists )
        {
            fPoint pos;
            pos.x = loc.x;
            pos.y = loc.y;
            pos.z = 0.0f;
            SpawnBuffBot( loc.name, loc.mapId, pos );
        }
    }
}

CPlayer* CBotManager::SpawnBuffBot( const char* name, int mapId, fPoint pos )
{
    // Spawn bot with Cleric job (222), level 100, static (isDynamic = false)
    CPlayer* bot = SpawnBot( name, 222, 100, mapId, pos, false );
    if ( !bot ) return NULL;

    CPlayerBot* botAi = GetBotByPlayer( bot );
    if ( botAi )
    {
        botAi->SetBuffBot( true );
        botAi->SetAutoRoam( false );
        botAi->SetDynamic( false );
        botAi->SetState( BOT_STATE_BUFF_BOT );
        botAi->EquipTieredGear( true );
        Log( MSG_INFO, "Buff Bot '%s' successfully spawned and initialized on map %d at (%.1f, %.1f)", name, mapId, pos.x, pos.y );
    }
    return bot;
}

CPlayerBot* CBotManager::FindNearbyBuffBot( UINT mapId, fPoint pos, float radius )
{
    pthread_mutex_lock( &m_botMutex );
    CPlayerBot* bestBot = NULL;
    float bestDist = radius;

    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        CPlayerBot* bAi = m_bots[i];
        if ( bAi && bAi->IsBuffBot( ) && bAi->GetPlayer( ) && bAi->GetPlayer( )->Position )
        {
            if ( bAi->GetPlayer( )->Position->Map == mapId )
            {
                float d = GServer->distance( pos, bAi->GetPlayer( )->Position->current );
                if ( d <= bestDist )
                {
                    bestDist = d;
                    bestBot = bAi;
                }
            }
        }
    }
    pthread_mutex_unlock( &m_botMutex );
    return bestBot;
}

void CBotManager::CheckProximitySpawns( )
{
    clock_t now = clock( );
    if ( ( now - m_lastProximityCheck ) < ( 5 * CLOCKS_PER_SEC ) ) return;
    m_lastProximityCheck = now;

    // Scan all active maps for real players
    for ( UINT m = 0; m < GServer->MapList.Map.size( ); m++ )
    {
        CMap* map = GServer->MapList.Map.at( m );
        if ( !map || map == GServer->MapList.nullzone || map->PlayerList.empty( ) ) continue;

        std::vector<CPlayer*> realPlayers;
        for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
        {
            CPlayer* p = map->PlayerList[i];
            if ( p && !p->is_bot && p->Session->inGame && p->Position )
            {
                realPlayers.push_back( p );
            }
        }
        if ( realPlayers.empty( ) ) continue;

        for ( size_t rpIdx = 0; rpIdx < realPlayers.size( ); rpIdx++ )
        {
            CPlayer* rp = realPlayers[rpIdx];
            if ( !rp || !rp->Position ) continue;

            // Count bots within 80m of this player
            int nearbyBotCount = 0;
            for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
            {
                CPlayer* other = map->PlayerList[i];
                if ( other && other->is_bot && other->Position )
                {
                    if ( GServer->distance( rp->Position->current, other->Position->current ) <= 80.0f )
                    {
                        nearbyBotCount++;
                    }
                }
            }

            // Maintain 8-10 bots near player
            int targetBots = 8;
            if ( nearbyBotCount < targetBots && GetBotCount( ) < 120 )
            {
                int toSpawn = ( targetBots - nearbyBotCount > 2 ) ? 2 : ( targetBots - nearbyBotCount );
                for ( int s = 0; s < toSpawn; s++ )
                {
                    // Level within +- 4 of player
                    int delta = ( rand( ) % 9 ) - 4;
                    int botLvl = rp->Stats->Level + delta;
                    if ( botLvl < 1 ) botLvl = 1;
                    if ( botLvl > 200 ) botLvl = 200;

                    int job = 0;
                    if ( botLvl >= 70 )
                    {
                        int r = rand( ) % 4;
                        int sub = rand( ) % 2;
                        if ( r == 0 ) job = (sub == 0) ? 121 : 122; // Knight / Champion
                        else if ( r == 1 ) job = (sub == 0) ? 221 : 222; // Mage / Cleric
                        else if ( r == 2 ) job = (sub == 0) ? 321 : 322; // Raider / Scout
                        else job = (sub == 0) ? 421 : 422; // Bourgeois / Artisan
                    }
                    else if ( botLvl >= 10 )
                    {
                        int r = rand( ) % 4;
                        if ( r == 0 ) job = 111;
                        else if ( r == 1 ) job = 211;
                        else if ( r == 2 ) job = 311;
                        else job = 411;
                    }

                    float angle = ( (float)( rand( ) % 360 ) ) * ( 3.14159265f / 180.0f );
                    float dist = 18.0f + ( (float)( rand( ) % 30 ) ); // 18m to 48m away
                    fPoint spawnPos;
                    spawnPos.x = rp->Position->current.x + cos( angle ) * dist;
                    spawnPos.y = rp->Position->current.y + sin( angle ) * dist;
                    spawnPos.z = rp->Position->current.z;

                    std::string name = GenerateUniqueName( );
                    SpawnBot( name.c_str( ), job, botLvl, map->id, spawnPos, true );
                }
            }
        }
    }
}

void CBotManager::PruneOrphanedBots( )
{
    clock_t now = clock( );
    if ( ( now - m_lastPruneCheck ) < ( 10 * CLOCKS_PER_SEC ) ) return;
    m_lastPruneCheck = now;

    pthread_mutex_lock( &m_botMutex );
    std::vector<std::string> toPrune;

    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        CPlayerBot* botAi = m_bots[i];
        if ( !botAi || !botAi->IsDynamic( ) || !botAi->GetPlayer( ) ) continue;

        CPlayer* bot = botAi->GetPlayer( );
        if ( HasRealPlayerInParty( bot ) )
        {
            botAi->SetLastPlayerNearby( now );
            continue;
        }

        if ( HasRealPlayerNearby( bot, 120.0f ) )
        {
            botAi->SetLastPlayerNearby( now );
            continue;
        }

        clock_t elapsed = now - botAi->GetLastPlayerNearby( );
        if ( elapsed >= ( 60 * CLOCKS_PER_SEC ) )
        {
            toPrune.push_back( std::string( bot->CharInfo->charname ) );
        }
    }
    pthread_mutex_unlock( &m_botMutex );

    for ( size_t i = 0; i < toPrune.size( ); i++ )
    {
        RemoveBot( toPrune[i].c_str( ) );
        Log( MSG_INFO, "[PlayerBot] Recycled orphaned dynamic bot '%s'", toPrune[i].c_str( ) );
    }
}

CPlayer* CBotManager::SpawnBot( const char* name, int job, int level, int mapId, fPoint pos, bool isDynamic )
{
    if ( !name || strlen( name ) == 0 ) return NULL;

    pthread_mutex_lock( &m_botMutex );

    // Check if a bot with this name already exists
    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        if ( m_bots[i] && m_bots[i]->GetPlayer( ) )
        {
            if ( strncmp( m_bots[i]->GetPlayer( )->CharInfo->charname, name, 16 ) == 0 )
            {
                CPlayer* existing = m_bots[i]->GetPlayer( );
                pthread_mutex_unlock( &m_botMutex );
                return existing;
            }
        }
    }

    if ( mapId < 0 || mapId >= GServer->MapList.max )
    {
        pthread_mutex_unlock( &m_botMutex );
        return NULL;
    }
    CMap* map = GServer->MapList.Index[mapId];
    if ( !map || map == GServer->MapList.nullzone )
    {
        pthread_mutex_unlock( &m_botMutex );
        return NULL;
    }

    // 1. Allocate socket-less dummy client
    CClientSocket* client = new (std::nothrow) CClientSocket( );
    if ( !client )
    {
        pthread_mutex_unlock( &m_botMutex );
        return NULL;
    }
    client->sock = INVALID_SOCKET;
    client->isActive = false;

    // 2. Allocate CPlayer entity
    CPlayer* bot = new (std::nothrow) CPlayer( client );
    if ( !bot )
    {
        delete client;
        pthread_mutex_unlock( &m_botMutex );
        return NULL;
    }
    client->player = (void*)bot;

    bot->is_bot = true;
    bot->is_invisible = false;
    bot->clientid = GServer->GetNewClientID( );
    if ( bot->clientid <= 1 )
    {
        delete bot;
        pthread_mutex_unlock( &m_botMutex );
        return NULL;
    }

    // 3. Initialize Session & Character Information
    bot->Session->userid = 0;
    bot->Session->inGame = true;
    strncpy( bot->Session->username, name, 16 );
    bot->Session->username[16] = '\0';

    strncpy( bot->CharInfo->charname, name, 16 );
    bot->CharInfo->charname[16] = '\0';
    bot->CharInfo->charid = 900000 + ( bot->clientid % 90000 );
    bot->CharInfo->Job = job;
    bot->CharInfo->Sex = rand( ) % 2;

    // Verified base face STB rows with existing ZMS models for male and female
    static const int s_validFaceIds[] = { 1, 8, 15, 22, 29, 36, 43 };
    static const size_t s_validFaceCount = sizeof( s_validFaceIds ) / sizeof( s_validFaceIds[0] );
    bot->CharInfo->Face = s_validFaceIds[ rand( ) % s_validFaceCount ];

    // Verified valid hair STB rows with existing ZMS models for male and female
    static const int s_validHairIds[] = {
        0, 1, 2, 3,
        5, 6, 7, 8,
        10, 11, 12, 13,
        15, 16, 17, 18,
        20, 21, 22, 23
    };
    static const size_t s_validHairCount = sizeof( s_validHairIds ) / sizeof( s_validHairIds[0] );
    bot->CharInfo->Hair = s_validHairIds[ rand( ) % s_validHairCount ];
    bot->CharInfo->Zulies = 1000 + ( level * 500 );

    // 4. Set Level & Attributes
    int botLevel = ( level < 1 ) ? 1 : ( ( level > 200 ) ? 200 : level );
    bot->Stats->Level = botLevel;
    bot->Attr->Str = 15;
    bot->Attr->Dex = 15;
    bot->Attr->Int = 15;
    bot->Attr->Con = 15;
    bot->Attr->Cha = 10;
    bot->Attr->Sen = 10;

    for ( int lvl = 1; lvl < botLevel; lvl++ )
    {
        if ( job == 111 || job == 121 || job == 122 ) // Soldier
        {
            bot->Attr->Str += 3;
            bot->Attr->Con += 2;
        }
        else if ( job == 211 || job == 221 || job == 222 ) // Muse
        {
            bot->Attr->Int += 3;
            bot->Attr->Con += 1;
            bot->Attr->Sen += 1;
        }
        else if ( job == 311 || job == 321 || job == 322 ) // Hawker
        {
            bot->Attr->Dex += 3;
            bot->Attr->Sen += 1;
            bot->Attr->Str += 1;
        }
        else if ( job == 411 || job == 421 || job == 422 ) // Dealer
        {
            bot->Attr->Con += 2;
            bot->Attr->Sen += 2;
            bot->Attr->Str += 1;
        }
        else // Visitor
        {
            bot->Attr->Str += 2;
            bot->Attr->Dex += 2;
            bot->Attr->Con += 1;
        }
    }

    // 5. Set Position
    bot->Position->Map = mapId;
    bot->Position->current = pos;
    bot->Position->destiny = pos;
    bot->Position->source = pos;
    bot->Position->lastMoveTime = clock( );

    // 6. Instantiate AI controller and equip tiered gear & skills
    CPlayerBot* botAi = new CPlayerBot( bot );
    botAi->SetDynamic( isDynamic );
    botAi->SetLastPlayerNearby( clock( ) );
    bot->bot_ai = botAi;

    botAi->EquipTieredGear( false );
    botAi->UpdateSkills( );

    // 7. Calculate stats and set status
    bot->SetStats( );
    bot->Stats->HP = bot->Stats->MaxHP;
    bot->Stats->MP = bot->Stats->MaxMP;
    bot->Status->Stance = RUNNING;
    bot->Status->CanMove = true;
    bot->Status->CanAttack = true;
    bot->Status->CanCastSkill = true;

    // 8. Add to Map PlayerList
    map->AddPlayer( bot );
    m_bots.push_back( botAi );

    pthread_mutex_unlock( &m_botMutex );

    Log( MSG_INFO, "[PlayerBot] Bot '%s' (Job: %d, Lvl: %d, Dynamic: %d) spawned on map %d at (%.2f, %.2f)",
         name, job, botLevel, isDynamic ? 1 : 0, mapId, pos.x, pos.y );

    return bot;
}

bool CBotManager::RemoveBot( const char* name )
{
    if ( !name ) return false;

    pthread_mutex_lock( &m_botMutex );
    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        CPlayerBot* botAi = m_bots[i];
        if ( botAi && botAi->GetPlayer( ) )
        {
            if ( strncmp( botAi->GetPlayer( )->CharInfo->charname, name, 16 ) == 0 )
            {
                CPlayer* bot = botAi->GetPlayer( );
                m_bots.erase( m_bots.begin( ) + i );
                m_recentNames[std::string( name )] = clock( );
                pthread_mutex_unlock( &m_botMutex );

                if ( bot )
                {
                    if ( bot->Position && bot->Position->Map < (UINT)GServer->MapList.max )
                    {
                        CMap* map = GServer->MapList.Index[bot->Position->Map];
                        if ( map && map != GServer->MapList.nullzone )
                        {
                            // Broadcast 0x794 despawn to all real players on this map
                            // so clients cleanly destroy the znzin 3D entity even if the bot moved out of visual range
                            BEGINPACKET( pak, 0x794 );
                            ADDWORD    ( pak, bot->clientid );
                            for ( UINT p = 0; p < map->PlayerList.size( ); p++ )
                            {
                                CPlayer* rp = map->PlayerList[p];
                                if ( rp && !rp->is_bot && rp->client && rp->client->isActive && rp->Session->inGame )
                                {
                                    rp->client->SendPacket( &pak );
                                    for ( size_t v = 0; v < rp->VisiblePlayers.size( ); v++ )
                                    {
                                        if ( rp->VisiblePlayers[v] == bot )
                                        {
                                            rp->VisiblePlayers.erase( rp->VisiblePlayers.begin( ) + v );
                                            break;
                                        }
                                    }
                                }
                            }
                            map->RemovePlayer( bot, false );
                        }
                    }
                    delete botAi;
                    delete bot;
                }
                Log( MSG_INFO, "[PlayerBot] Bot '%s' removed.", name );
                return true;
            }
        }
    }
    pthread_mutex_unlock( &m_botMutex );
    return false;
}

void CBotManager::RemoveAllBots( )
{
    pthread_mutex_lock( &m_botMutex );
    clock_t now = clock( );
    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        if ( m_bots[i] && m_bots[i]->GetPlayer( ) && m_bots[i]->GetPlayer( )->CharInfo )
        {
            m_recentNames[std::string( m_bots[i]->GetPlayer( )->CharInfo->charname )] = now;
        }
    }
    std::vector<CPlayerBot*> toRemove = m_bots;
    m_bots.clear( );
    pthread_mutex_unlock( &m_botMutex );

    for ( size_t i = 0; i < toRemove.size( ); i++ )
    {
        CPlayerBot* botAi = toRemove[i];
        if ( botAi )
        {
            CPlayer* bot = botAi->GetPlayer( );
            if ( bot )
            {
                if ( bot->Position && bot->Position->Map < (UINT)GServer->MapList.max )
                {
                    CMap* map = GServer->MapList.Index[bot->Position->Map];
                    if ( map && map != GServer->MapList.nullzone )
                    {
                        BEGINPACKET( pak, 0x794 );
                        ADDWORD    ( pak, bot->clientid );
                        for ( UINT p = 0; p < map->PlayerList.size( ); p++ )
                        {
                            CPlayer* rp = map->PlayerList[p];
                            if ( rp && !rp->is_bot && rp->client && rp->client->isActive && rp->Session->inGame )
                            {
                                rp->client->SendPacket( &pak );
                                for ( size_t v = 0; v < rp->VisiblePlayers.size( ); v++ )
                                {
                                    if ( rp->VisiblePlayers[v] == bot )
                                    {
                                        rp->VisiblePlayers.erase( rp->VisiblePlayers.begin( ) + v );
                                        break;
                                    }
                                }
                            }
                        }
                        map->RemovePlayer( bot, false );
                    }
                }
                delete botAi;
                delete bot;
            }
        }
    }
    Log( MSG_INFO, "[PlayerBot] All bots removed." );
}

CPlayerBot* CBotManager::GetBot( const char* name )
{
    if ( !name ) return NULL;

    pthread_mutex_lock( &m_botMutex );
    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        if ( m_bots[i] && m_bots[i]->GetPlayer( ) )
        {
            if ( strncmp( m_bots[i]->GetPlayer( )->CharInfo->charname, name, 16 ) == 0 )
            {
                CPlayerBot* result = m_bots[i];
                pthread_mutex_unlock( &m_botMutex );
                return result;
            }
        }
    }
    pthread_mutex_unlock( &m_botMutex );
    return NULL;
}

CPlayerBot* CBotManager::GetBotByPlayer( CPlayer* player )
{
    if ( !player ) return NULL;

    pthread_mutex_lock( &m_botMutex );
    for ( size_t i = 0; i < m_bots.size( ); i++ )
    {
        if ( m_bots[i] && m_bots[i]->GetPlayer( ) == player )
        {
            CPlayerBot* result = m_bots[i];
            pthread_mutex_unlock( &m_botMutex );
            return result;
        }
    }
    pthread_mutex_unlock( &m_botMutex );
    return NULL;
}

size_t CBotManager::GetBotCount( )
{
    pthread_mutex_lock( &m_botMutex );
    size_t count = m_bots.size( );
    pthread_mutex_unlock( &m_botMutex );
    return count;
}

std::vector<CPlayerBot*> CBotManager::GetBots( )
{
    pthread_mutex_lock( &m_botMutex );
    std::vector<CPlayerBot*> copy = m_bots;
    pthread_mutex_unlock( &m_botMutex );
    return copy;
}


// -------------------------------------------------------------
// CPlayerBot AI Implementation
// -------------------------------------------------------------
CPlayerBot::CPlayerBot( CPlayer* player )
    : m_player( player ),
      m_state( BOT_STATE_IDLE ),
      m_targetMobCid( 0 ),
      m_targetDropCid( 0 ),
      m_followTarget( NULL ),
      m_autoRoam( true ),
      m_roamRadius( 35.0f ),
      m_isBuffBot( false ),
      m_bonfireCid( 0 ),
      m_lastBuffSay( 0 ),
      m_lastBuffCastTime( 0 ),
      m_lastBuffSeekTime( 0 ),
      m_isDynamic( false ),
      m_lastKnownLevel( 1 )
{
    m_lastAiTick = clock( );
    m_stateTimer = clock( );
    m_stuckTimer = clock( );
    m_lastSkillCastTime = clock( );
    m_lastPlayerNearby = clock( );

    if ( m_player )
    {
        m_roamCenter = m_player->Position->current;
        m_lastPos = m_player->Position->current;
        m_lastKnownLevel = m_player->Stats->Level;
    }
    else
    {
        m_roamCenter.x = 0;
        m_roamCenter.y = 0;
        m_roamCenter.z = 0;
        m_lastPos = m_roamCenter;
    }
}

CPlayerBot::~CPlayerBot( )
{
    if ( m_player )
    {
        m_player->bot_ai = NULL;
    }
}

const char* CPlayerBot::GetStateString( ) const
{
    switch ( m_state )
    {
        case BOT_STATE_IDLE:      return "IDLE";
        case BOT_STATE_ROAM:      return "ROAM";
        case BOT_STATE_COMBAT:    return "COMBAT";
        case BOT_STATE_LOOT:      return "LOOT";
        case BOT_STATE_REST:      return "REST";
        case BOT_STATE_FOLLOW:    return "FOLLOW";
        case BOT_STATE_DEAD:      return "DEAD";
        case BOT_STATE_BUFF_BOT:  return "BUFF_BOT";
        case BOT_STATE_SEEK_BUFF: return "SEEK_BUFF";
        default:                  return "UNKNOWN";
    }
}

void CPlayerBot::SetState( BotState state )
{
    m_state = state;
    m_stateTimer = clock( );
}

void CPlayerBot::Say( const char* msg )
{
    if ( !m_player || !msg ) return;
    BEGINPACKET( pak, 0x783 );
    ADDWORD    ( pak, m_player->clientid );
    ADDSTRING  ( pak, (char*)msg );
    ADDBYTE    ( pak, 0 );
    GServer->SendToVisible( &pak, m_player );
}

void CPlayerBot::MoveTo( fPoint dest )
{
    if ( !m_player ) return;

    // If already moving towards approximately the same destination, don't spam 0x79a packet
    if ( m_player->IsMoving( ) && GServer->distance( m_player->Position->destiny, dest ) < 1.0f )
    {
        return;
    }

    if ( m_player->Status->Stance == 1 )
    {
        StandUp( );
    }

    m_player->Position->destiny = dest;
    m_player->Position->lastMoveTime = clock( );
    m_player->Status->Stance = RUNNING;
    m_player->Stats->Move_Speed = m_player->GetMoveSpeed( );

    BEGINPACKET( pak, 0x79a );
    ADDWORD    ( pak, m_player->clientid );
    ADDWORD    ( pak, m_player->Battle->target );
    ADDWORD    ( pak, (DWORD)( GServer->distance( m_player->Position->current, m_player->Position->destiny ) * 100 ) );
    ADDFLOAT   ( pak, m_player->Position->destiny.x * 100 );
    ADDFLOAT   ( pak, m_player->Position->destiny.y * 100 );
    ADDWORD    ( pak, 0 );
    GServer->SendToVisible( &pak, m_player );
}

void CPlayerBot::StopMoving( )
{
    if ( !m_player ) return;

    // If already stopped, never spam 0x79a packet - doing so causes the client to reset height and bounce/fall
    if ( !m_player->IsMoving( ) ) return;

    m_player->Position->destiny = m_player->Position->current;

    if ( !m_player->IsOnBattle( ) )
    {
        BEGINPACKET( pak, 0x79a );
        ADDWORD    ( pak, m_player->clientid );
        ADDWORD    ( pak, 0 );
        ADDWORD    ( pak, 0 );
        ADDFLOAT   ( pak, m_player->Position->current.x * 100 );
        ADDFLOAT   ( pak, m_player->Position->current.y * 100 );
        ADDWORD    ( pak, 0 );
        GServer->SendToVisible( &pak, m_player );
    }
}

void CPlayerBot::AttackTarget( CCharacter* target )
{
    if ( !m_player || !target ) return;

    if ( target->IsMonster( ) )
    {
        CMonster* mon = reinterpret_cast<CMonster*>( target );
        if ( mon->IsBonfire( ) || mon->GetOwner( ) != NULL ) return;
    }

    if ( m_player->Status->Stance == 1 )
    {
        StandUp( );
    }

    m_targetMobCid = target->clientid;
    m_player->StartAction( target, NORMAL_ATTACK, 0 );
    m_player->Battle->contatk = true;
    m_player->Battle->target = target->clientid;
}

void CPlayerBot::SitDown( )
{
    if ( !m_player ) return;

    StopMoving( );
    m_player->Status->Stance = 1;
    m_player->Stats->Move_Speed = m_player->GetMoveSpeed( );

    BEGINPACKET( pak, 0x782 );
    ADDWORD    ( pak, m_player->clientid );
    ADDBYTE    ( pak, m_player->Status->Stance );
    ADDWORD    ( pak, m_player->Stats->Base_Speed );
    GServer->SendToVisible( &pak, m_player );
}

void CPlayerBot::StandUp( )
{
    if ( !m_player ) return;

    m_player->Status->Stance = RUNNING;
    m_player->Stats->Move_Speed = m_player->GetMoveSpeed( );

    BEGINPACKET( pak, 0x782 );
    ADDWORD    ( pak, m_player->clientid );
    ADDBYTE    ( pak, m_player->Status->Stance );
    ADDWORD    ( pak, m_player->Stats->Base_Speed );
    GServer->SendToVisible( &pak, m_player );
}

void CPlayerBot::AllocateStats( )
{
    if ( !m_player || m_player->CharInfo->StatPoints < 1 ) return;

    int job = m_player->CharInfo->Job;
    while ( m_player->CharInfo->StatPoints > 0 )
    {
        int statid = 0; // 0=STR, 1=DEX, 2=INT, 3=CON, 4=CHA, 5=SEN
        if ( job == 111 || job == 121 || job == 122 )
        {
            statid = ( rand( ) % 10 < 6 ) ? 0 : 3; // 60% STR, 40% CON
        }
        else if ( job == 211 || job == 221 || job == 222 )
        {
            statid = ( rand( ) % 10 < 6 ) ? 2 : 5; // 60% INT, 40% SEN
        }
        else if ( job == 311 || job == 321 || job == 322 )
        {
            statid = ( rand( ) % 10 < 6 ) ? 1 : 5; // 60% DEX, 40% SEN
        }
        else if ( job == 411 || job == 421 || job == 422 )
        {
            statid = ( rand( ) % 10 < 5 ) ? 3 : ( ( rand( ) % 5 < 3 ) ? 5 : 0 );
        }
        else
        {
            statid = ( rand( ) % 10 < 5 ) ? 0 : 1; // STR, DEX
        }

        int nstatval = 1;
        switch ( statid )
        {
            case 0: nstatval = (int)floor( (float)m_player->Attr->Str / 5.0f ); break;
            case 1: nstatval = (int)floor( (float)m_player->Attr->Dex / 5.0f ); break;
            case 2: nstatval = (int)floor( (float)m_player->Attr->Int / 5.0f ); break;
            case 3: nstatval = (int)floor( (float)m_player->Attr->Con / 5.0f ); break;
            case 4: nstatval = (int)floor( (float)m_player->Attr->Cha / 5.0f ); break;
            case 5: nstatval = (int)floor( (float)m_player->Attr->Sen / 5.0f ); break;
        }
        if ( nstatval < 1 ) nstatval = 1;

        if ( m_player->CharInfo->StatPoints < nstatval )
        {
            break;
        }

        switch ( statid )
        {
            case 0: m_player->Attr->Str++; break;
            case 1: m_player->Attr->Dex++; break;
            case 2: m_player->Attr->Int++; break;
            case 3: m_player->Attr->Con++; break;
            case 4: m_player->Attr->Cha++; break;
            case 5: m_player->Attr->Sen++; break;
        }
        m_player->CharInfo->StatPoints -= nstatval;
    }
    m_player->SetStats( );
}

void CPlayerBot::EquipTieredGear( bool broadcast )
{
    if ( !m_player ) return;
    int job = m_player->CharInfo->Job;
    int lvl = m_player->Stats->Level;

    int capNum = 0;
    int bodyNum = 29;
    int armNum = 21;
    int footNum = 29;
    int wepNum = 1;
    int shieldNum = 0;
    int ammoType = 0; // 0=none, 1=arrow (slot 132), 2=bullet (slot 133)

    if ( job == 111 || job == 121 || job == 122 ) // Soldier / Knight / Champion
    {
        if ( lvl < 20 )
        {
            capNum = 31; bodyNum = 31; armNum = 31; footNum = 31;
            wepNum = ( lvl >= 8 ) ? 2 : 1; // Short Sword / Wooden Sword
            shieldNum = 1; // Wooden Shield
        }
        else if ( lvl < 40 )
        {
            capNum = 32; bodyNum = 32; armNum = 32; footNum = 32;
            wepNum = ( lvl >= 32 ) ? 4 : 3; // Khukuri / Rapier
            shieldNum = 3; // Round Shield
        }
        else if ( lvl < 70 )
        {
            capNum = 34; bodyNum = 34; armNum = 34; footNum = 34;
            wepNum = ( lvl >= 58 ) ? 6 : 5; // Bushido / Long Sword
            shieldNum = 7; // Kite Shield
        }
        else
        {
            capNum = 37; bodyNum = 37; armNum = 37; footNum = 37;
            wepNum = ( lvl >= 84 ) ? 8 : 7; // Elven Sword / Saber
            shieldNum = 10; // Plate Shield
        }
    }
    else if ( job == 211 || job == 221 || job == 222 ) // Muse / Mage / Cleric
    {
        if ( lvl < 20 )
        {
            capNum = 61; bodyNum = 61; armNum = 61; footNum = 61;
            wepNum = 301; // Baobab Rod
        }
        else if ( lvl < 40 )
        {
            capNum = 63; bodyNum = 63; armNum = 63; footNum = 63;
            wepNum = 302; // Lemmings Rod
        }
        else if ( lvl < 70 )
        {
            capNum = 66; bodyNum = 66; armNum = 66; footNum = 66;
            wepNum = 304; // Mage's Rod
        }
        else
        {
            capNum = 71; bodyNum = 71; armNum = 71; footNum = 71;
            wepNum = 305; // White Staff
        }
    }
    else if ( job == 311 || job == 321 || job == 322 ) // Hawker / Raider / Scout
    {
        ammoType = 1;
        if ( lvl < 20 )
        {
            capNum = 91; bodyNum = 91; armNum = 91; footNum = 91;
            wepNum = 201; // Toy Bow
        }
        else if ( lvl < 40 )
        {
            capNum = 93; bodyNum = 93; armNum = 93; footNum = 93;
            wepNum = 202; // Short Bow
        }
        else if ( lvl < 70 )
        {
            capNum = 96; bodyNum = 96; armNum = 96; footNum = 96;
            wepNum = 203; // Long Bow
        }
        else
        {
            capNum = 99; bodyNum = 99; armNum = 99; footNum = 99;
            wepNum = 205; // White Wing Bow
        }
    }
    else if ( job == 411 || job == 421 || job == 422 ) // Dealer / Bourgeois / Artisan
    {
        ammoType = 2;
        if ( lvl < 20 )
        {
            capNum = 121; bodyNum = 121; armNum = 121; footNum = 121;
            wepNum = 231; // Bubble Gun
        }
        else if ( lvl < 40 )
        {
            capNum = 123; bodyNum = 123; armNum = 123; footNum = 123;
            wepNum = 232; // Air Gun
        }
        else if ( lvl < 70 )
        {
            capNum = 125; bodyNum = 125; armNum = 125; footNum = 125;
            wepNum = 234; // Iron Rifle
        }
        else
        {
            capNum = 126; bodyNum = 126; armNum = 126; footNum = 126;
            wepNum = 235; // Iron Shotgun
        }
    }
    else // Visitor / Novice
    {
        capNum = 0;
        bodyNum = 29;
        armNum = 21;
        footNum = 29;
        wepNum = 1;
    }

    int oldItems[6] = {
        m_player->items[2].itemnum,
        m_player->items[3].itemnum,
        m_player->items[5].itemnum,
        m_player->items[6].itemnum,
        m_player->items[7].itemnum,
        m_player->items[8].itemnum
    };

    auto SetEquip = []( CItem& itm, int type, int num ) {
        if ( num > 0 )
        {
            itm.itemtype = type;
            itm.itemnum = num;
            itm.count = 1;
            itm.durability = 50;
            itm.lifespan = 100;
            itm.appraised = true;
            itm.refine = 0;
        }
        else
        {
            itm.itemtype = 0;
            itm.itemnum = 0;
            itm.count = 0;
            itm.durability = 0;
            itm.lifespan = 0;
            itm.appraised = false;
            itm.refine = 0;
        }
    };

    SetEquip( m_player->items[2], CAP, capNum );
    SetEquip( m_player->items[3], BODY, bodyNum );
    SetEquip( m_player->items[5], GLOVE, armNum );
    SetEquip( m_player->items[6], SHOE, footNum );
    SetEquip( m_player->items[7], WEAPON, wepNum );
    SetEquip( m_player->items[8], SUBWEAPON, shieldNum );

    if ( ammoType == 1 )
    {
        m_player->items[132].itemtype = NATURAL;
        m_player->items[132].itemnum = 1;
        m_player->items[132].count = 9999;
        m_player->items[132].appraised = true;
    }
    else if ( ammoType == 2 )
    {
        m_player->items[133].itemtype = NATURAL;
        m_player->items[133].itemnum = 1;
        m_player->items[133].count = 9999;
        m_player->items[133].appraised = true;
    }

    m_player->SetStats( );

    // Broadcast visual equip changes ONLY when broadcast is requested (e.g. tier upgrade)
    // and ONLY for slots where the item actually changed, preventing redundant loadVisible in client
    if ( broadcast )
    {
        int slots[6] = { 2, 3, 5, 6, 7, 8 };
        for ( int s = 0; s < 6; s++ )
        {
            int slotIdx = slots[s];
            if ( m_player->items[slotIdx].itemnum == oldItems[s] )
            {
                continue; // Item unchanged, don't spam client
            }
            BEGINPACKET( pak, 0x7a5 );
            ADDWORD    ( pak, m_player->clientid );
            ADDWORD    ( pak, (slotIdx == 8) ? 8 : (slotIdx == 7) ? 7 : (slotIdx == 6) ? 6 : (slotIdx == 5) ? 5 : (slotIdx == 3) ? 3 : 2 );
            ADDWORD    ( pak, m_player->items[slotIdx].itemnum );
            ADDWORD    ( pak, GServer->BuildItemRefine( &m_player->items[slotIdx] ) );
            ADDWORD    ( pak, m_player->Stats->Move_Speed );
            GServer->SendToVisible( &pak, m_player );
        }
    }
}

void CPlayerBot::UpdateSkills( )
{
    if ( !m_player ) return;
    int job = m_player->CharInfo->Job;
    int lvl = m_player->Stats->Level;

    std::vector<UINT> skillsToLearn;

    // Visitor / Universal
    skillsToLearn.push_back( 101 ); // Dual Scratch

    if ( job == 111 || job == 121 || job == 122 ) // Soldier
    {
        if ( lvl >= 10 ) skillsToLearn.push_back( 261 ); // Double Attack
        if ( lvl >= 20 ) skillsToLearn.push_back( 291 ); // Fatal Thrust
        if ( lvl >= 35 ) skillsToLearn.push_back( 336 ); // Piercing Lunge
        if ( lvl >= 50 ) skillsToLearn.push_back( 351 ); // Voltage Crash
    }
    else if ( job == 211 || job == 221 || job == 222 ) // Muse
    {
        if ( lvl >= 10 ) { skillsToLearn.push_back( 851 ); skillsToLearn.push_back( 921 ); } // Mana Bolt, Cure
        if ( lvl >= 20 ) { skillsToLearn.push_back( 821 ); skillsToLearn.push_back( 926 ); } // Ice Bolt, Hustle Charm
        if ( lvl >= 35 ) { skillsToLearn.push_back( 861 ); skillsToLearn.push_back( 981 ); } // Mana Spear, Recovery
        if ( lvl >= 50 ) { skillsToLearn.push_back( 871 ); skillsToLearn.push_back( 1031 ); } // Voltage Jolt, Party Heal
    }
    else if ( job == 311 || job == 321 || job == 322 ) // Hawker
    {
        if ( lvl >= 10 ) skillsToLearn.push_back( 1441 ); // Double Arrow
        if ( lvl >= 20 ) skillsToLearn.push_back( 1431 ); // Clamp Arrow
        if ( lvl >= 35 ) skillsToLearn.push_back( 1486 ); // Triple Arrow
        if ( lvl >= 50 ) skillsToLearn.push_back( 1501 ); // Stun Arrow
    }
    else if ( job == 411 || job == 421 || job == 422 ) // Dealer
    {
        if ( lvl >= 10 ) skillsToLearn.push_back( 2036 ); // Twin Shot
        if ( lvl >= 25 ) skillsToLearn.push_back( 2061 ); // Sniping
        if ( lvl >= 40 ) skillsToLearn.push_back( 2301 ); // Triple Shot
        if ( lvl >= 55 ) skillsToLearn.push_back( 2076 ); // Demolition Expertise
    }

    int skillLvl = 1 + ( lvl / 10 );
    if ( skillLvl > 10 ) skillLvl = 10;

    for ( size_t i = 0; i < skillsToLearn.size( ) && i < MAX_ALL_SKILL; i++ )
    {
        UINT skId = skillsToLearn[i];
        m_player->cskills[i].id = skId;
        m_player->cskills[i].level = skillLvl;
        m_player->cskills[i].cooldown_skill = 0;
        m_player->cskills[i].thisskill = GServer->GetSkillByID( skId + skillLvl - 1 );
    }

    // Spend skill points
    m_player->CharInfo->SkillPoints = 0;
}

bool CPlayerBot::CastCombatSkill( CCharacter* target )
{
    if ( !m_player || !target || target->IsDead( ) || target->Stats->HP <= 0 ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 15 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastSkillCastTime ) < (clock_t)( 3.5f * CLOCKS_PER_SEC ) ) return false;

    int job = m_player->CharInfo->Job;
    int lvl = m_player->Stats->Level;
    UINT chosenSkill = 101; // Dual Scratch fallback

    if ( job == 111 || job == 121 || job == 122 ) // Soldier
    {
        if ( lvl >= 50 && ( rand( ) % 3 == 0 ) ) chosenSkill = 351; // Voltage Crash
        else if ( lvl >= 35 && ( rand( ) % 2 == 0 ) ) chosenSkill = 336; // Piercing Lunge
        else if ( lvl >= 20 && ( rand( ) % 2 == 0 ) ) chosenSkill = 291; // Fatal Thrust
        else if ( lvl >= 10 ) chosenSkill = 261; // Double Attack
    }
    else if ( job == 211 || job == 221 || job == 222 ) // Muse
    {
        if ( lvl >= 50 && ( rand( ) % 3 == 0 ) ) chosenSkill = 871; // Voltage Jolt
        else if ( lvl >= 35 && ( rand( ) % 2 == 0 ) ) chosenSkill = 861; // Mana Spear
        else if ( lvl >= 20 && ( rand( ) % 2 == 0 ) ) chosenSkill = 821; // Ice Bolt
        else if ( lvl >= 10 ) chosenSkill = 851; // Mana Bolt
    }
    else if ( job == 311 || job == 321 || job == 322 ) // Hawker
    {
        if ( lvl >= 50 && ( rand( ) % 3 == 0 ) ) chosenSkill = 1501; // Stun Arrow
        else if ( lvl >= 35 && ( rand( ) % 2 == 0 ) ) chosenSkill = 1486; // Triple Arrow
        else if ( lvl >= 20 && ( rand( ) % 2 == 0 ) ) chosenSkill = 1431; // Clamp Arrow
        else if ( lvl >= 10 ) chosenSkill = 1441; // Double Arrow
    }
    else if ( job == 411 || job == 421 || job == 422 ) // Dealer
    {
        if ( lvl >= 55 && ( rand( ) % 3 == 0 ) ) chosenSkill = 2076; // Demolition Expertise
        else if ( lvl >= 40 && ( rand( ) % 2 == 0 ) ) chosenSkill = 2301; // Triple Shot
        else if ( lvl >= 25 && ( rand( ) % 2 == 0 ) ) chosenSkill = 2061; // Sniping
        else if ( lvl >= 10 ) chosenSkill = 2036; // Twin Shot
    }

    int skillLvl = 1 + ( lvl / 10 );
    if ( skillLvl > 10 ) skillLvl = 10;
    UINT finalSkillId = chosenSkill + skillLvl - 1;

    CSkills* sk = GServer->GetSkillByID( finalSkillId );
    if ( !sk )
    {
        sk = GServer->GetSkillByID( chosenSkill );
        if ( !sk ) return false;
        finalSkillId = chosenSkill;
    }

    m_player->Battle->contatk = true;
    m_player->StartAction( target, SKILL_ATTACK, finalSkillId );
    m_lastSkillCastTime = now;
    return true;
}

bool CPlayerBot::CheckPartyHeal( )
{
    if ( !m_player || !m_player->Status->CanCastSkill || m_player->Stats->MP < 30 ) return false;
    int job = m_player->CharInfo->Job;
    if ( job != 211 && job != 221 && job != 222 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastSkillCastTime ) < (clock_t)( 4.0f * CLOCKS_PER_SEC ) ) return false;

    UINT healSkill = ( m_player->Stats->Level >= 35 ) ? 981 : 921; // Recovery or Cure
    CSkills* sk = GServer->GetSkillByID( healSkill );
    if ( !sk ) return false;

    // 1. Check self
    if ( m_player->Stats->HP < ( m_player->Stats->MaxHP * 60 / 100 ) )
    {
        m_player->StartAction( m_player, SKILL_BUFF, healSkill );
        m_lastSkillCastTime = now;
        Say( "Healing myself!" );
        return true;
    }

    // 2. Check party members
    if ( m_player->Party->party != NULL )
    {
        CParty* party = m_player->Party->party;
        for ( UINT i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* member = party->Members[i];
            if ( member && !member->IsDead( ) && member->Position->Map == m_player->Position->Map )
            {
                if ( member->Stats->HP < ( member->Stats->MaxHP * 65 / 100 ) &&
                     GServer->distance( m_player->Position->current, member->Position->current ) <= 20.0f )
                {
                    m_player->StartAction( member, SKILL_BUFF, healSkill );
                    m_lastSkillCastTime = now;
                    char healMsg[64];
                    snprintf( healMsg, sizeof(healMsg), "Healing %s!", member->CharInfo->charname );
                    Say( healMsg );
                    return true;
                }
            }
        }
    }

    return false;
}

void CPlayerBot::CheckProgression( )
{
    if ( !m_player ) return;
    int currentLvl = m_player->Stats->Level;
    if ( currentLvl != m_lastKnownLevel )
    {
        AllocateStats( );
        UpdateSkills( );

        int oldTier = ( m_lastKnownLevel < 20 ) ? 1 : ( m_lastKnownLevel < 40 ) ? 2 : ( m_lastKnownLevel < 70 ) ? 3 : 4;
        int newTier = ( currentLvl < 20 ) ? 1 : ( currentLvl < 40 ) ? 2 : ( currentLvl < 70 ) ? 3 : 4;
        if ( newTier != oldTier )
        {
            EquipTieredGear( );
        }

        char lvlMsg[64];
        snprintf( lvlMsg, sizeof(lvlMsg), "Ding! Level %d!", currentLvl );
        Say( lvlMsg );
        m_lastKnownLevel = currentLvl;
    }
}

CMap* CPlayerBot::GetMap( ) const
{
    if ( !m_player || !m_player->Position ) return NULL;
    if ( m_player->Position->Map >= (UINT)GServer->MapList.max ) return NULL;
    CMap* map = GServer->MapList.Index[m_player->Position->Map];
    if ( !map || map == GServer->MapList.nullzone ) return NULL;
    return map;
}

void CPlayerBot::Respawn( )
{
    if ( !m_player ) return;

    m_player->Stats->HP = m_player->Stats->MaxHP;
    m_player->Stats->MP = m_player->Stats->MaxMP;

    for ( int j = 0; j < 30; j++ )
    {
        m_player->MagicStatus[j].Duration = 0;
        m_player->MagicStatus[j].BuffTime = 0;
    }
    m_player->RefreshBuff( );
    m_player->SetStats( );

    CMap* map = GetMap( );
    if ( map )
    {
        map->TeleportPlayer( m_player, m_roamCenter, false );
    }

    StandUp( );
    if ( m_followTarget )
    {
        SetState( BOT_STATE_FOLLOW );
    }
    else
    {
        SetState( BOT_STATE_IDLE );
    }
}

CMonster* CPlayerBot::FindNearbyMonster( float radius )
{
    CMap* map = GetMap( );
    if ( !map || !m_player || !m_player->Position ) return NULL;

    CMonster* bestTarget = NULL;
    float bestDistSq = radius * radius;
    fPoint myPos = m_player->Position->current;

    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || !mob->Position ) continue;
        if ( mob->IsBonfire( ) ) continue; // Bonfires are non-combat entities
        if ( mob->GetOwner( ) != NULL ) continue; // Player/bot summons are friendly

        float dx = myPos.x - mob->Position->current.x;
        if ( dx > radius || dx < -radius ) continue;
        float dy = myPos.y - mob->Position->current.y;
        if ( dy > radius || dy < -radius ) continue;

        float distSq = dx * dx + dy * dy;
        if ( distSq < bestDistSq )
        {
            bestDistSq = distSq;
            bestTarget = mob;
        }
    }
    return bestTarget;
}

CDrop* CPlayerBot::FindNearbyDrop( float radius )
{
    CMap* map = GetMap( );
    if ( !map || !m_player || !m_player->Position ) return NULL;

    CDrop* bestDrop = NULL;
    float bestDistSq = radius * radius;
    fPoint myPos = m_player->Position->current;
    time_t nowTime = time( NULL );

    for ( UINT i = 0; i < map->DropsList.size( ); i++ )
    {
        CDrop* drop = map->DropsList[i];
        if ( !drop ) continue;

        if ( drop->owner != 0 && drop->owner != m_player->CharInfo->charid &&
             ( nowTime - drop->droptime ) < 30 )
        {
            continue;
        }

        float dx = myPos.x - drop->pos.x;
        if ( dx > radius || dx < -radius ) continue;
        float dy = myPos.y - drop->pos.y;
        if ( dy > radius || dy < -radius ) continue;

        float distSq = dx * dx + dy * dy;
        if ( distSq < bestDistSq )
        {
            bestDistSq = distSq;
            bestDrop = drop;
        }
    }
    return bestDrop;
}

void CPlayerBot::Update( )
{
    if ( !m_player ) return;

    // Check death condition
    if ( m_player->IsDead( ) || m_player->Stats->HP <= 0 )
    {
        if ( m_isBuffBot )
        {
            Respawn( );
            return;
        }

        if ( m_state != BOT_STATE_DEAD )
        {
            SetState( BOT_STATE_DEAD );
        }
    }

    // Buff bot execution bypass
    if ( m_isBuffBot )
    {
        m_player->Stats->MP = m_player->Stats->MaxMP;
        if ( m_player->Stats->HP < m_player->Stats->MaxHP )
        {
            m_player->Stats->HP = m_player->Stats->MaxHP;
        }
        HandleBuffBot( );
        return;
    }

    // AI decision throttled to 500ms
    clock_t now = clock( );
    if ( ( now - m_lastAiTick ) < ( CLOCKS_PER_SEC / 2 ) )
    {
        return;
    }
    m_lastAiTick = now;

    // Check progression, stat points, and tiered equipment
    CheckProgression( );

    // Self-defense check: If attacked by a monster while not in combat
    if ( m_state != BOT_STATE_COMBAT && m_state != BOT_STATE_DEAD )
    {
        if ( m_player->IsOnBattle( ) && m_player->Battle->target != 0 )
        {
            CMap* map = GetMap( );
            if ( map )
            {
                CMonster* mob = map->GetMonsterInMap( m_player->Battle->target );
                if ( mob && !mob->IsDead( ) && mob->Stats->HP > 0 && !mob->IsBonfire( ) )
                {
                    if ( m_state == BOT_STATE_REST )
                    {
                        StandUp( );
                    }
                    m_targetMobCid = mob->clientid;
                    SetState( BOT_STATE_COMBAT );
                }
            }
        }
    }

    switch ( m_state )
    {
        case BOT_STATE_IDLE:      HandleIdle( );      break;
        case BOT_STATE_ROAM:      HandleRoam( );      break;
        case BOT_STATE_COMBAT:    HandleCombat( );    break;
        case BOT_STATE_LOOT:      HandleLoot( );      break;
        case BOT_STATE_REST:      HandleRest( );      break;
        case BOT_STATE_FOLLOW:    HandleFollow( );    break;
        case BOT_STATE_DEAD:      HandleDead( );      break;
        case BOT_STATE_BUFF_BOT:  HandleBuffBot( );   break;
        case BOT_STATE_SEEK_BUFF: HandleSeekBuff( );  break;
    }
}

void CPlayerBot::HandleIdle( )
{
    if ( !m_player ) return;

    // Check for party heals first
    if ( CheckPartyHeal( ) ) return;

    // 1. Health check: Rest if low on HP or MP
    if ( m_player->Stats->HP < ( m_player->Stats->MaxHP * 40 / 100 ) ||
         m_player->Stats->MP < ( m_player->Stats->MaxMP * 30 / 100 ) )
    {
        SitDown( );
        SetState( BOT_STATE_REST );
        return;
    }

    // 2. Check if we need buffs and a Buff Bot is nearby (with 45s cooldown)
    clock_t now = clock( );
    if ( NeedsBuffs( ) && ( ( now - m_lastBuffSeekTime ) >= (clock_t)( 45 * CLOCKS_PER_SEC ) ) )
    {
        CPlayerBot* buffBot = CBotManager::GetInstance()->FindNearbyBuffBot( m_player->Position->Map, m_player->Position->current, 80.0f );
        if ( buffBot && buffBot->GetPlayer( ) && buffBot->GetPlayer( )->Position )
        {
            SetState( BOT_STATE_SEEK_BUFF );
            return;
        }
    }

    // 3. Look for nearby drops first
    CDrop* drop = FindNearbyDrop( 25.0f );
    if ( drop )
    {
        m_targetDropCid = drop->clientid;
        MoveTo( drop->pos );
        SetState( BOT_STATE_LOOT );
        return;
    }

    // 3. Look for nearby monsters to attack
    CMonster* mob = FindNearbyMonster( 25.0f );
    if ( mob )
    {
        AttackTarget( mob );
        SetState( BOT_STATE_COMBAT );
        return;
    }

    // 4. Roam if enabled and idle timer expired (> 2.5 seconds)
    if ( m_autoRoam )
    {
        clock_t elapsed = clock( ) - m_stateTimer;
        if ( elapsed >= (clock_t)( 2.5f * CLOCKS_PER_SEC ) )
        {
            float angle = ( (float)( rand( ) % 360 ) ) * ( 3.14159265f / 180.0f );
            float maxR = ( m_roamRadius > 5.0f ) ? m_roamRadius : 10.0f;
            float dist = 5.0f + ( (float)( rand( ) % (int)( maxR - 5.0f + 1.0f ) ) );
            fPoint dest;
            dest.x = m_roamCenter.x + cos( angle ) * dist;
            dest.y = m_roamCenter.y + sin( angle ) * dist;
            dest.z = 0;
            MoveTo( dest );
            SetState( BOT_STATE_ROAM );
            m_stuckTimer = clock( );
            m_lastPos = m_player->Position->current;
        }
    }
}

void CPlayerBot::HandleRoam( )
{
    if ( !m_player ) return;

    // Check for drops while roaming
    CDrop* drop = FindNearbyDrop( 20.0f );
    if ( drop )
    {
        m_targetDropCid = drop->clientid;
        MoveTo( drop->pos );
        SetState( BOT_STATE_LOOT );
        return;
    }

    // Check for monsters while roaming
    CMonster* mob = FindNearbyMonster( 20.0f );
    if ( mob )
    {
        AttackTarget( mob );
        SetState( BOT_STATE_COMBAT );
        return;
    }

    // Check if arrived at destination
    if ( !m_player->IsMoving( ) )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }

    // Stuck check: after 5 seconds, if moved less than 1 unit, cancel and idle
    clock_t now = clock( );
    if ( ( now - m_stuckTimer ) >= ( 5 * CLOCKS_PER_SEC ) )
    {
        float movedDist = GServer->distance( m_player->Position->current, m_lastPos );
        if ( movedDist < 1.0f )
        {
            StopMoving( );
            SetState( BOT_STATE_IDLE );
            return;
        }
        m_stuckTimer = now;
        m_lastPos = m_player->Position->current;
    }
}

void CPlayerBot::HandleCombat( )
{
    CMap* map = GetMap( );
    if ( !map )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }

    // Check party heal during combat
    CheckPartyHeal( );

    CMonster* mob = map->GetMonsterInMap( m_targetMobCid );
    if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) )
    {
        ClearBattle( m_player->Battle );
        m_targetMobCid = 0;

        // Check for loot right away
        CDrop* drop = FindNearbyDrop( 25.0f );
        if ( drop )
        {
            m_targetDropCid = drop->clientid;
            MoveTo( drop->pos );
            SetState( BOT_STATE_LOOT );
            return;
        }

        // Check if resting is needed
        if ( m_player->Stats->HP < ( m_player->Stats->MaxHP * 50 / 100 ) ||
             m_player->Stats->MP < ( m_player->Stats->MaxMP * 30 / 100 ) )
        {
            SitDown( );
            SetState( BOT_STATE_REST );
            return;
        }

        if ( m_followTarget )
        {
            SetState( BOT_STATE_FOLLOW );
            return;
        }

        SetState( BOT_STATE_IDLE );
        return;
    }

    // Target is valid: check attack range
    float dist = GServer->distance( m_player->Position->current, mob->Position->current );

    // If target is too far away, drop combat
    if ( dist > 35.0f )
    {
        m_targetMobCid = 0;
        ClearBattle( m_player->Battle );
        SetState( BOT_STATE_IDLE );
        return;
    }

    // Not yet in battle or targeting a different mob: start attacking
    if ( !m_player->IsOnBattle( ) || m_player->Battle->target != mob->clientid )
    {
        AttackTarget( mob );
        return;
    }

    // In battle: if reached target, try casting combat skills
    if ( m_player->IsTargetReached( mob ) )
    {
        CastCombatSkill( mob );
    }
}

void CPlayerBot::HandleLoot( )
{
    CMap* map = GetMap( );
    if ( !map )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }

    CDrop* drop = map->GetDropInMap( m_targetDropCid );
    if ( !drop )
    {
        m_targetDropCid = 0;
        SetState( BOT_STATE_IDLE );
        return;
    }

    float dist = GServer->distance( m_player->Position->current, drop->pos );
    if ( dist > 2.0f )
    {
        MoveTo( drop->pos );
    }
    else
    {
        StopMoving( );
        if ( drop->type == 1 )
        {
            m_player->CharInfo->Zulies += drop->amount;
        }
        else if ( drop->type == 2 && drop->item )
        {
            m_player->AddItem( *drop->item );
        }

        pthread_mutex_lock( &map->DropMutex );
        map->DeleteDrop( drop );
        pthread_mutex_unlock( &map->DropMutex );
        m_targetDropCid = 0;

        // Check for any more drops nearby
        CDrop* nextDrop = FindNearbyDrop( 20.0f );
        if ( nextDrop )
        {
            m_targetDropCid = nextDrop->clientid;
            MoveTo( nextDrop->pos );
        }
        else
        {
            if ( m_followTarget )
            {
                SetState( BOT_STATE_FOLLOW );
            }
            else
            {
                SetState( BOT_STATE_IDLE );
            }
        }
    }
}

void CPlayerBot::HandleRest( )
{
    if ( !m_player ) return;

    if ( m_player->Stats->HP >= ( m_player->Stats->MaxHP * 95 / 100 ) &&
         m_player->Stats->MP >= ( m_player->Stats->MaxMP * 95 / 100 ) )
    {
        StandUp( );
        if ( m_followTarget )
        {
            SetState( BOT_STATE_FOLLOW );
        }
        else
        {
            SetState( BOT_STATE_IDLE );
        }
    }
}

void CPlayerBot::HandleFollow( )
{
    if ( !m_player ) return;

    if ( !m_followTarget || !m_followTarget->Session->inGame ||
         m_followTarget->Position->Map != m_player->Position->Map ||
         ( m_player->Party->party && m_followTarget->Party->party != m_player->Party->party ) ||
         ( !m_player->Party->party ) )
    {
        m_followTarget = NULL;
        SetState( BOT_STATE_IDLE );
        return;
    }

    CMap* map = GetMap( );
    if ( !map )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }

    // 1. Check if party members need healing
    if ( CheckPartyHeal( ) ) return;

    // 2. Assist leader: check if leader is attacking a monster
    if ( m_followTarget->IsOnBattle( ) && m_followTarget->Battle->target != 0 )
    {
        CMonster* leaderMob = map->GetMonsterInMap( m_followTarget->Battle->target );
        if ( leaderMob && !leaderMob->IsDead( ) && leaderMob->Stats->HP > 0 )
        {
            AttackTarget( leaderMob );
            SetState( BOT_STATE_COMBAT );
            return;
        }
    }

    // 3. Defend leader: check if any monster is attacking leader
    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( mob && !mob->IsDead( ) && mob->Stats->HP > 0 )
        {
            if ( mob->Battle && mob->Battle->target == m_followTarget->clientid )
            {
                AttackTarget( mob );
                SetState( BOT_STATE_COMBAT );
                return;
            }
        }
    }

    // 4. Follow leader movement
    float dist = GServer->distance( m_player->Position->current, m_followTarget->Position->current );
    if ( dist > 50.0f )
    {
        map->TeleportPlayer( m_player, m_followTarget->Position->current, false );
    }
    else if ( dist > 3.5f )
    {
        MoveTo( m_followTarget->Position->current );
    }
    else
    {
        if ( m_player->IsMoving( ) )
        {
            StopMoving( );
        }
    }
}

void CPlayerBot::HandleDead( )
{
    clock_t now = clock( );
    if ( ( now - m_stateTimer ) >= ( 5 * CLOCKS_PER_SEC ) )
    {
        Respawn( );
    }
}

bool CPlayerBot::NeedsBuffs( )
{
    if ( !m_player || !m_player->Status ) return false;
    if ( m_isBuffBot ) return false;
    return ( m_player->Status->Dash_up == 0xff ||
             m_player->Status->Haste_up == 0xff ||
             m_player->Status->Attack_up == 0xff ||
             m_player->Status->Defense_up == 0xff );
}

void CPlayerBot::HandleSeekBuff( )
{
    if ( !m_player ) return;

    if ( !NeedsBuffs( ) )
    {
        StopMoving( );
        Say( "Thanks for the buffs!" );
        m_lastBuffSeekTime = clock( );
        SetState( BOT_STATE_IDLE );
        return;
    }

    clock_t elapsed = clock( ) - m_stateTimer;
    if ( elapsed >= (clock_t)( 25 * CLOCKS_PER_SEC ) )
    {
        StopMoving( );
        m_lastBuffSeekTime = clock( );
        SetState( BOT_STATE_IDLE );
        return;
    }

    CPlayerBot* buffBot = CBotManager::GetInstance()->FindNearbyBuffBot( m_player->Position->Map, m_player->Position->current, 80.0f );
    if ( !buffBot || !buffBot->GetPlayer( ) || !buffBot->GetPlayer( )->Position )
    {
        StopMoving( );
        m_lastBuffSeekTime = clock( );
        SetState( BOT_STATE_IDLE );
        return;
    }

    // Position in a designated circle around the buff bot / campfire
    fPoint centerPos = buffBot->GetPlayer( )->Position->current;
    float angle = (float)( ( m_player->clientid * 53 ) % 360 ) * ( 3.14159265f / 180.0f );
    fPoint waitPos;
    waitPos.x = centerPos.x + cosf( angle ) * 3.2f;
    waitPos.y = centerPos.y + sinf( angle ) * 3.2f;
    waitPos.z = centerPos.z;

    float dist = GServer->distance( m_player->Position->current, waitPos );
    if ( dist > 1.5f )
    {
        MoveTo( waitPos );
    }
    else
    {
        StopMoving( );
    }
}

void CPlayerBot::HandleBuffBot( )
{
    if ( !m_player ) return;

    CMap* map = GetMap( );
    if ( !map ) return;

    // 1. Maintain standing stance and stationary position
    if ( m_player->Status->Stance == 1 )
    {
        StandUp( );
    }
    m_player->Position->destiny = m_player->Position->current;
    m_player->Stats->MP = m_player->Stats->MaxMP;
    m_player->Stats->HP = m_player->Stats->MaxHP;

    clock_t now = clock( );

    // 2. Bonfire maintenance: ensure an active large bonfire (806) stands next to the bot
    bool hasBonfire = false;
    if ( m_bonfireCid != 0 )
    {
        CMonster* mon = map->GetMonsterInMap( m_bonfireCid );
        if ( mon && !mon->IsDead( ) && mon->Stats->HP > 0 && mon->IsBonfire( ) )
        {
            hasBonfire = true;
        }
        else
        {
            m_bonfireCid = 0;
        }
    }

    if ( !hasBonfire )
    {
        for ( size_t m = 0; m < map->MonsterList.size( ); m++ )
        {
            CMonster* mon = map->MonsterList[m];
            if ( mon && !mon->IsDead( ) && mon->Stats->HP > 0 && mon->IsBonfire( ) )
            {
                if ( GServer->distance( m_player->Position->current, mon->Position->current ) <= 6.0f )
                {
                    hasBonfire = true;
                    m_bonfireCid = mon->clientid;
                    break;
                }
            }
        }
    }

    if ( !hasBonfire )
    {
        fPoint firePos = m_player->Position->current;
        firePos.x += 1.8f;
        firePos.y += 1.2f;
        firePos.z = m_player->Position->current.z;

        CMonster* fire = map->AddMonster( 806, firePos, m_player->clientid );
        if ( fire )
        {
            fire->life_time = 360; // 6 minutes
            fire->Stats->HP = fire->Stats->MaxHP;
            m_bonfireCid = fire->clientid;
            Log( MSG_INFO, "Buff Bot '%s' spawned Bonfire (CID %u) on map %d at (%.1f, %.1f)",
                 m_player->CharInfo->charname, fire->clientid, m_player->Position->Map, firePos.x, firePos.y );
            Say( "Campfire lit! Come rest and receive buffs!" );
            m_lastBuffSay = now;
        }
    }

    // Periodic welcoming shoutout every 75 seconds
    if ( ( now - m_lastBuffSay ) >= (clock_t)( 75 * CLOCKS_PER_SEC ) )
    {
        const char* cheers[] = {
            "Free buffs for all adventurers! Stop by the fire!",
            "May the goddess bless your hunt! Get your buffs here!",
            "Rest by the campfire and power up before grinding!"
        };
        int cIdx = rand( ) % 3;
        Say( cheers[cIdx] );
        m_lastBuffSay = now;
    }

    // 3. Buffing Queue: throttle cast interval to 1.2 seconds
    if ( ( now - m_lastBuffCastTime ) < (clock_t)( 1.2f * CLOCKS_PER_SEC ) )
    {
        return;
    }

    // Collect candidates within 16 meters: prioritize real players, then bots
    CPlayer* bestTarget = NULL;
    bool targetIsPlayer = false;

    // Scan real players first
    for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( !p || p == m_player || p->IsDead( ) ) continue;
        if ( p->Session == NULL || !p->Session->inGame ) continue;
        if ( p->bot_ai != NULL ) continue; // real player only

        float dist = GServer->distance( m_player->Position->current, p->Position->current );
        if ( dist <= 16.0f )
        {
            bool needsHelp = false;
            if ( p->Stats->HP < ( p->Stats->MaxHP * 85 / 100 ) ) needsHelp = true;
            else if ( p->Status->Dash_up == 0xff ||
                      p->Status->Haste_up == 0xff ||
                      p->Status->Attack_up == 0xff ||
                      p->Status->Defense_up == 0xff ||
                      p->Status->Accuracy_up == 0xff ||
                      p->Status->Critical_up == 0xff ||
                      p->Status->ExtraDamage_up == 0xff )
            {
                needsHelp = true;
            }

            if ( needsHelp )
            {
                bestTarget = p;
                targetIsPlayer = true;
                break;
            }
        }
    }

    // If no real player needs buffs, check nearby bots
    if ( !bestTarget )
    {
        for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
        {
            CPlayer* p = map->PlayerList[i];
            if ( !p || p == m_player || p->IsDead( ) ) continue;
            if ( p->bot_ai == NULL ) continue;
            CPlayerBot* bAi = reinterpret_cast<CPlayerBot*>( p->bot_ai );
            if ( bAi && bAi->IsBuffBot( ) ) continue; // Don't buff other buff bots

            float dist = GServer->distance( m_player->Position->current, p->Position->current );
            if ( dist <= 16.0f )
            {
                bool needsHelp = false;
                if ( p->Stats->HP < ( p->Stats->MaxHP * 85 / 100 ) ) needsHelp = true;
                else if ( p->Status->Dash_up == 0xff ||
                          p->Status->Haste_up == 0xff ||
                          p->Status->Attack_up == 0xff ||
                          p->Status->Defense_up == 0xff ||
                          p->Status->Accuracy_up == 0xff ||
                          p->Status->Critical_up == 0xff ||
                          p->Status->ExtraDamage_up == 0xff )
                {
                    needsHelp = true;
                }

                if ( needsHelp )
                {
                    bestTarget = p;
                    targetIsPlayer = false;
                    break;
                }
            }
        }
    }

    if ( !bestTarget ) return;

    // Decide which buff or heal to cast based on exact active status effects
    UINT skillToCast = 0;

    // 1. Healing priority
    if ( bestTarget->Stats->HP < ( bestTarget->Stats->MaxHP * 85 / 100 ) )
    {
        skillToCast = 985; // Recovery Lv 5
    }
    // 2. Hustle Charm (Speed) -> sets Status->Dash_up
    else if ( bestTarget->Status->Dash_up == 0xff )
    {
        skillToCast = 930; // Hustle Charm Lv 5
    }
    // 3. Battle Charm (Attack Speed) -> sets Status->Haste_up
    else if ( bestTarget->Status->Haste_up == 0xff )
    {
        skillToCast = 1254; // Battle Charm Lv 9
    }
    // 4. Clobber Charm (Attack Power) -> sets Status->Attack_up
    else if ( bestTarget->Status->Attack_up == 0xff )
    {
        skillToCast = 1270; // Clobber Charm Lv 5
    }
    // 5. Resilience Charm (Defense) -> sets Status->Defense_up
    else if ( bestTarget->Status->Defense_up == 0xff )
    {
        skillToCast = 1004; // Resilience Charm Lv 9
    }
    // 6. Precision Charm (Accuracy) -> sets Status->Accuracy_up
    else if ( bestTarget->Status->Accuracy_up == 0xff )
    {
        skillToCast = 1019; // Precision Charm Lv 9
    }
    // 7. Critical Charm (Crit Rate) -> sets Status->Critical_up
    else if ( bestTarget->Status->Critical_up == 0xff )
    {
        skillToCast = 1284; // Critical Charm Lv 9
    }
    // 8. Valkyrie Charm (Extra Damage) -> sets Status->ExtraDamage_up
    else if ( bestTarget->Status->ExtraDamage_up == 0xff )
    {
        skillToCast = 1294; // Valkyrie Charm Lv 9
    }

    if ( skillToCast > 0 )
    {
        CSkills* sk = GServer->GetSkillByID( skillToCast );
        if ( sk )
        {
            m_player->StartAction( bestTarget, SKILL_BUFF, skillToCast );
            m_lastBuffCastTime = now;

            if ( targetIsPlayer && ( rand( ) % 3 == 0 ) )
            {
                char msg[80];
                snprintf( msg, sizeof(msg), "Blessings upon you, %s!", bestTarget->CharInfo->charname );
                Say( msg );
            }
        }
    }
}
