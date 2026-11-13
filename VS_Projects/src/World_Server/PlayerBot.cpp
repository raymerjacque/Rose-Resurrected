#include "PlayerBot.h"
#include "VendingCatalog.h"
#include "Arena.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>

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

// Helper for space-tolerant, case-insensitive character name comparisons
static bool CompareCharNames( const char* name1, const char* name2 )
{
    if ( !name1 || !name2 ) return false;
    while ( *name1 == ' ' ) name1++;
    while ( *name2 == ' ' ) name2++;

    for ( int i = 0; i < 16; i++ )
    {
        char c1 = name1[i];
        char c2 = name2[i];
        if ( c1 == ' ' || c1 == '\0' || c1 == '\r' || c1 == '\n' ) c1 = '\0';
        if ( c2 == ' ' || c2 == '\0' || c2 == '\r' || c2 == '\n' ) c2 = '\0';

        if ( tolower( (unsigned char)c1 ) != tolower( (unsigned char)c2 ) )
            return false;

        if ( c1 == '\0' )
            return true;
    }
    return true;
}

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
      m_vendingBotsInitialized( false ),
      m_nameCounter( 0 )
{
    pthread_mutex_init( &m_botMutex, NULL );
    m_lastProximityCheck = clock( );
    m_lastAmbientCheck = clock( );
    m_lastPruneCheck = clock( );
    m_lastBuffBotCheck = clock( );
    m_lastVendingBotCheck = clock( );
    m_lastCompanionCheck = clock( );
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

    // 4. Persistent market hub Vending Bots check
    CheckVendingBots( );

    // 5. Dynamic proximity spawner check
    CheckProximitySpawns( );

    // 6. Prune orphaned dynamic bots
    PruneOrphanedBots( );

    // 7. Companion Party Check & Maintenance for Real Players
    CheckPlayerCompanions( );
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

void CBotManager::CheckVendingBots( )
{
    clock_t now = clock( );
    if ( m_vendingBotsInitialized )
    {
        if ( ( now - m_lastVendingBotCheck ) < ( 30 * CLOCKS_PER_SEC ) ) return;
    }
    m_lastVendingBotCheck = now;
    m_vendingBotsInitialized = true;

    struct VendingBotLocation {
        const char* name;
        int job;
        int level;
        int mapId;
        float x;
        float y;
        const char* shopTitle;
        int category;
    };

    static const VendingBotLocation spots[] = {
        // Junon Polis (Map 2) Hotspots
        { "Merchant_Koji",       321, 100, 2, 5655.0f, 5210.0f, "[Gems] T5-T7 Jewels & Diamonds",     VEND_CAT_GEMS },
        { "Trader_Jin",          111, 100, 2, 5662.0f, 5205.0f, "[Weapons] Rare Swords & Bows",        VEND_CAT_WEAPONS_HIGH },
        { "Shop_Milo",           211, 100, 2, 5670.0f, 5200.0f, "[Pots] HP/MP & Return Scrolls",       VEND_CAT_POTIONS_SCROLLS },
        { "Refiner_Orin",        322, 100, 2, 5724.0f, 5220.0f, "[Refine] Talismans & Runes",         VEND_CAT_REFINE },
        { "Artisan_Bax",         322, 100, 2, 5730.0f, 5230.0f, "[Crafting] Ores, Woods & Leathers",   VEND_CAT_MATERIALS },
        { "Armorer_Gale",        111, 100, 2, 5722.0f, 5280.0f, "[Armor] Class Sets & Shields",        VEND_CAT_ARMOR_HIGH },
        { "Mechanic_Torque",     321, 100, 2, 5510.0f, 5235.0f, "[PAT] Frames, Engines & Wheels",      VEND_CAT_PAT },
        { "Jeweler_Serena",      311, 100, 2, 5518.0f, 5240.0f, "[Jewelry] Stat Rings & Necklaces",    VEND_CAT_ACCESSORIES },
        { "WingMaster_Aero",     411, 100, 2, 5505.0f, 5245.0f, "[Wings] Angel, Devil & Fairies",     VEND_CAT_WINGS },
        { "QMaster_Rook",        411, 100, 2, 5295.0f, 5250.0f, "[Ammo] Elemental Arrows & Bullets",   VEND_CAT_AMMO },
        { "Dealer_Vance",        411, 100, 2, 5320.0f, 5100.0f, "[Gear] Dual Weapons & Katars",        VEND_CAT_DUAL_KATARS },
        // Expanded Junon Polis Vendors
        { "Dealer_Kael",         411, 100, 2, 5645.0f, 5218.0f, "[Katars] Assassins & Dual Blades",   VEND_CAT_DUAL_KATARS },
        { "Alchemist_Vara",      211, 100, 2, 5678.0f, 5192.0f, "[Pots] Elixirs & Greater Mana",       VEND_CAT_POTIONS_SCROLLS },
        { "Armorer_Thorne",      111, 100, 2, 5715.0f, 5275.0f, "[Armor] Heavy Plate & Guards",        VEND_CAT_ARMOR_HIGH },
        { "Bowcraft_Soren",      411, 100, 2, 5658.0f, 5198.0f, "[Bows] Compound Bows & Guns",         VEND_CAT_WEAPONS_HIGH },
        { "Jeweler_Cynthia",     311, 100, 2, 5525.0f, 5248.0f, "[Jewels] High Amulets & Rings",       VEND_CAT_ACCESSORIES },
        { "Forge_Karr",          322, 100, 2, 5718.0f, 5212.0f, "[Refine] Runes & Catalysts",          VEND_CAT_REFINE },
        { "Mechanic_Gears",      321, 100, 2, 5502.0f, 5228.0f, "[PAT] High Speed Engines",            VEND_CAT_PAT },
        { "WMaker_Zephyr",       411, 100, 2, 5498.0f, 5238.0f, "[Wings] Feathered Wings & Backbags",  VEND_CAT_WINGS },
        { "GemCutter_Talon",     321, 100, 2, 5648.0f, 5202.0f, "[Gems] Perfect Rubies & Sapphires",   VEND_CAT_GEMS },
        { "QMaster_Bane",        411, 100, 2, 5288.0f, 5242.0f, "[Ammo] High Capacity Cartridges",     VEND_CAT_AMMO },
        { "Supplier_Rowan",      322, 100, 2, 5738.0f, 5238.0f, "[Crafting] Refined Metals & Woods",    VEND_CAT_MATERIALS },
        { "Enchanter_Mira",      211, 100, 2, 5685.0f, 5208.0f, "[Scrolls] Town Portal & Buff Spells", VEND_CAT_POTIONS_SCROLLS },

        // Canyon City of Zant (Map 1) Hotspots
        { "Vendor_Pippin",       311,  50, 1, 5242.0f, 5115.0f, "[Starter] HP/MP Pots & Scrolls",      VEND_CAT_ZANT_STARTER },
        { "Scout_Robin",         411,  50, 1, 5238.0f, 5122.0f, "[Ammo] Hunting Arrows & Bullets",     VEND_CAT_ZANT_AMMO },
        { "Peddler_Toby",        311,  50, 1, 5245.0f, 5130.0f, "[Materials] Monster Drops & Iron",    VEND_CAT_ZANT_MATERIALS },
        { "Smith_Brant",         111,  50, 1, 5250.0f, 5210.0f, "[Weapons] Swords, Staffs & Guns",     VEND_CAT_ZANT_WEAPONS },
        { "Tailor_Lydia",        311,  50, 1, 5265.0f, 5218.0f, "[Armor] Novice & Leather Armor",      VEND_CAT_ZANT_ARMOR },
        { "GemTrader_Ruby",      321,  50, 1, 5300.0f, 5228.0f, "[Gems] Cut Jewels & Talismans",       VEND_CAT_ZANT_GEMS },
        { "Collector_Felix",     311,  50, 1, 5270.0f, 5250.0f, "[Accessories] Rings & Back Bags",    VEND_CAT_ZANT_ACCESSORIES },
        // Expanded Zant Vendors
        { "Vendor_Clara",        311,  50, 1, 5235.0f, 5108.0f, "[Starter] Health Rations & MP Herbs", VEND_CAT_ZANT_STARTER },
        { "Smith_Jax",           111,  50, 1, 5258.0f, 5202.0f, "[Weapons] Iron Blades & Axes",        VEND_CAT_ZANT_WEAPONS },
        { "Tailor_Elena",        311,  50, 1, 5272.0f, 5210.0f, "[Armor] Padded Vests & Robes",       VEND_CAT_ZANT_ARMOR },
        { "Hunter_Brant",        411,  50, 1, 5230.0f, 5115.0f, "[Ammo] Sharp Arrows & Musket Shots",  VEND_CAT_ZANT_AMMO },
        { "Collector_Otto",      311,  50, 1, 5252.0f, 5138.0f, "[Materials] Raw Hides & Ore Chunks",  VEND_CAT_ZANT_MATERIALS },
        { "Jeweler_Naya",        311,  50, 1, 5278.0f, 5242.0f, "[Accessories] Copper Rings & Charms", VEND_CAT_ZANT_ACCESSORIES },
        { "GemTrader_Silas",     321,  50, 1, 5308.0f, 5220.0f, "[Gems] Uncut Crystals & Shards",     VEND_CAT_ZANT_GEMS },

        // Adventurer's Plains (Map 22) Market Outpost
        { "Plains_Tariq",        311,  30, 22, 5120.0f, 5340.0f, "[Novice] Field Supplies & Pots",     VEND_CAT_ZANT_STARTER },
        { "Plains_Hark",         111,  30, 22, 5128.0f, 5345.0f, "[Weapons] Traveler Swords & Bows",   VEND_CAT_ZANT_WEAPONS },
        { "Plains_Bram",         411,  30, 22, 5112.0f, 5335.0f, "[Ammo] Practice Arrows & Shells",    VEND_CAT_ZANT_AMMO },

        // Breezy Hills (Map 12) Crossroads
        { "Hills_Vane",          311,  70, 12, 5300.0f, 5200.0f, "[Supplies] Travel Pots & Scrolls",   VEND_CAT_POTIONS_SCROLLS },
        { "Hills_Lukas",         322,  70, 12, 5310.0f, 5210.0f, "[Refine] Mid-Tier Enhancers",        VEND_CAT_REFINE },
        { "Hills_Dara",          111,  70, 12, 5290.0f, 5190.0f, "[Armor] Guard & Ranger Sets",        VEND_CAT_ARMOR_HIGH }
    };
    static const size_t spotCount = sizeof( spots ) / sizeof( spots[0] );

    for ( size_t s = 0; s < spotCount; s++ )
    {
        const VendingBotLocation& loc = spots[s];
        if ( loc.mapId >= (UINT)GServer->MapList.max ) continue;
        CMap* map = GServer->MapList.Index[loc.mapId];
        if ( !map || map == GServer->MapList.nullzone ) continue;

        // Check if this vending bot already exists
        bool exists = false;
        pthread_mutex_lock( &m_botMutex );
        for ( size_t i = 0; i < m_bots.size( ); i++ )
        {
            CPlayerBot* bAi = m_bots[i];
            if ( bAi && bAi->GetPlayer( ) && bAi->GetPlayer( )->CharInfo )
            {
                if ( strncmp( bAi->GetPlayer( )->CharInfo->charname, loc.name, 15 ) == 0 )
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
            SpawnVendingBot( loc.name, loc.job, loc.level, loc.mapId, pos, loc.shopTitle, loc.category );
        }
    }
}

CPlayer* CBotManager::SpawnVendingBot( const char* name, int job, int level, int mapId, fPoint pos, const char* shopTitle, int category )
{
    // Spawn bot with designated job, level, static (isDynamic = false)
    CPlayer* bot = SpawnBot( name, job, level, mapId, pos, false );
    if ( !bot ) return NULL;

    CPlayerBot* botAi = GetBotByPlayer( bot );
    if ( botAi )
    {
        botAi->SetVendingBot( true );
        botAi->SetAutoRoam( false );
        botAi->SetDynamic( false );
        botAi->SetState( BOT_STATE_VENDING );
        botAi->EquipTieredGear( true );
        botAi->SetupVendingShop( shopTitle, category );
        Log( MSG_INFO, "Vending Bot '%s' successfully spawned and initialized on map %d at (%.1f, %.1f) [Category %d: %s]",
             name, mapId, pos.x, pos.y, category, shopTitle ? shopTitle : "Shop" );
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

void CBotManager::GetZoneLevelRange( UINT mapId, int& minLvl, int& maxLvl, bool& isCity )
{
    isCity = false;
    minLvl = 1;
    maxLvl = 150;

    switch ( mapId )
    {
        // Cities & Main Towns (Wide variety of level ranges)
        case 1:  // Zant City & Outskirts
        case 2:  // Junon Polis City
        case 3:  // Eucar City
        case 4:  // Eldeon City
            isCity = true;
            minLvl = 1;
            maxLvl = 120;
            break;

        // Junon Grinding Zones
        case 22: // Adventurer Plains
            minLvl = 2;
            maxLvl = 12;
            break;

        case 21: // Luxem Tower / El Verco
            minLvl = 14;
            maxLvl = 28;
            break;

        case 23: // Breezy Hills
            minLvl = 22;
            maxLvl = 38;
            break;

        case 24: // Forest of Wisdom
            minLvl = 28;
            maxLvl = 45;
            break;

        case 25: // Anima Lake
            minLvl = 32;
            maxLvl = 52;
            break;

        case 26: // Kenji Beach
            minLvl = 48;
            maxLvl = 68;
            break;

        // Dungeons
        case 51: // Goblin Cave
            minLvl = 15;
            maxLvl = 35;
            break;

        case 52: // Marsh of Ghosts
            minLvl = 35;
            maxLvl = 55;
            break;

        case 53: // Pyramids
            minLvl = 50;
            maxLvl = 75;
            break;

        case 61: // Crystal Cave
            minLvl = 60;
            maxLvl = 85;
            break;

        // Luna & High Maps
        case 31:
        case 32:
        case 33:
            minLvl = 70;
            maxLvl = 110;
            break;

        default:
        {
            if ( mapId < (UINT)GServer->MapList.max )
            {
                CMap* map = GServer->MapList.Index[mapId];
                if ( map && !map->MonsterList.empty( ) )
                {
                    int lowest = 200, highest = 1;
                    for ( size_t m = 0; m < map->MonsterList.size( ); m++ )
                    {
                        CMonster* mob = map->MonsterList[m];
                        if ( mob && mob->Stats && mob->Stats->Level > 0 )
                        {
                            if ( mob->Stats->Level < lowest ) lowest = mob->Stats->Level;
                            if ( mob->Stats->Level > highest ) highest = mob->Stats->Level;
                        }
                    }
                    if ( highest >= lowest )
                    {
                        minLvl = lowest;
                        maxLvl = highest + 5;
                    }
                }
            }
            break;
        }
    }
}

bool CBotManager::GetMapForLevel( int botLvl, UINT& targetMapId, fPoint& spawnPos )
{
    if ( botLvl <= 12 )
    {
        targetMapId = 22; // Adventurer Plains
        spawnPos = { 5303.62f, 5099.92f, 0 };
        return true;
    }
    else if ( botLvl <= 25 )
    {
        targetMapId = 21; // Luxem Tower
        spawnPos = { 5102.48f, 5063.67f, 0 };
        return true;
    }
    else if ( botLvl <= 38 )
    {
        targetMapId = 23; // Breezy Hills
        spawnPos = { 5096.35f, 4905.92f, 0 };
        return true;
    }
    else if ( botLvl <= 52 )
    {
        targetMapId = 25; // Anima Lake
        spawnPos = { 5379.49f, 5184.52f, 0 };
        return true;
    }
    else
    {
        targetMapId = 26; // Kenji Beach / High Map
        spawnPos = { 5655.32f, 5238.22f, 0 };
        return true;
    }
}

void CBotManager::CheckAndRelocateUninvitedBot( CPlayer* bot )
{
    if ( !bot || !bot->is_bot || !bot->bot_ai || !bot->Position ) return;

    CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( bot->bot_ai );
    if ( botAi->IsBuffBot( ) || botAi->IsVendingBot( ) ) return;

    int botLvl = bot->Stats->Level;
    UINT currentMap = bot->Position->Map;

    int minLvl = 1, maxLvl = 200;
    bool isCity = false;
    GetZoneLevelRange( currentMap, minLvl, maxLvl, isCity );

    botAi->SetFollowTarget( NULL );
    botAi->SetAutoRoam( true );
    botAi->SetRoamCenter( bot->Position->current );

    if ( isCity || botLvl < ( minLvl - 5 ) || botLvl > ( maxLvl + 10 ) )
    {
        UINT targetMapId = currentMap;
        fPoint spawnPos = bot->Position->current;
        if ( GetMapForLevel( botLvl, targetMapId, spawnPos ) )
        {
            Log( MSG_INFO, "Uninvited Bot '%s' (Lvl %d) relocating from Map %u (isCity=%d) to Map %u for grinding",
                 bot->CharInfo->charname, botLvl, currentMap, (int)isCity, targetMapId );

            if ( targetMapId < (UINT)GServer->MapList.max && GServer->MapList.Index[targetMapId] )
            {
                GServer->MapList.Index[targetMapId]->TeleportPlayer( bot, spawnPos, false );
                botAi->SetRoamCenter( spawnPos );
            }
        }
    }

    botAi->SetState( BOT_STATE_IDLE );
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

            // Maintain 12-14 bots near real player
            int targetBots = 12;
            if ( nearbyBotCount < targetBots && GetBotCount( ) < 200 )
            {
                int toSpawn = ( targetBots - nearbyBotCount > 4 ) ? 4 : ( targetBots - nearbyBotCount );
                for ( int s = 0; s < toSpawn; s++ )
                {
                    int botLvl = 1;
                    int minLvl = 1, maxLvl = 200;
                    bool isCity = false;
                    GetZoneLevelRange( map->id, minLvl, maxLvl, isCity );

                    if ( isCity )
                    {
                        // Cities spawn a rich variety of level ranges (Level 1 to 115)
                        botLvl = 1 + ( rand( ) % 115 );
                    }
                    else
                    {
                        // Grinding zones spawn STRICTLY within the zone level bounds, regardless of player level
                        int span = ( maxLvl >= minLvl ) ? ( maxLvl - minLvl + 1 ) : 1;
                        botLvl = minLvl + ( rand( ) % span );
                    }

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
        if ( !botAi || !botAi->IsDynamic( ) || !botAi->GetPlayer( ) || botAi->IsCompanion( ) ) continue;

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
    bot->Status->Stance = RUNNING; // Default movement mode is ALWAYS RUNNING
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
                    if ( bot->Party && bot->Party->party )
                    {
                        bot->Party->party->RemovePlayer( bot );
                    }
                    if ( bot->Battle )
                    {
                        ClearBattle( bot->Battle );
                    }

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
        if ( m_bots[i] && m_bots[i]->GetPlayer( ) && m_bots[i]->GetPlayer( )->CharInfo )
        {
            if ( CompareCharNames( m_bots[i]->GetPlayer( )->CharInfo->charname, name ) )
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
      m_roamRadius( 400.0f ),
      m_isBuffBot( false ),
      m_bonfireCid( 0 ),
      m_lastBuffSay( 0 ),
      m_lastBuffCastTime( 0 ),
      m_lastBuffSeekTime( 0 ),
      m_nextBuffCandidateIndex( 0 ),
      m_isVendingBot( false ),
      m_vendingCategory( 0 ),
      m_lastVendingSay( 0 ),
      m_isDynamic( false ),
      m_lastKnownLevel( 1 ),
      m_lastPartyBuffTime( 0 ),
      m_lastResurrectTime( 0 ),
      m_lastTauntTime( 0 ),
      m_lastHpPotionTime( 0 ),
      m_lastMpPotionTime( 0 ),
      m_lastChatterTime( 0 ),
      m_lastEmoteTime( 0 ),
      m_lastPartyInviteTime( 0 ),
      m_lastMigrationCheck( 0 ),
      m_lastGreetingTime( 0 ),
      m_duelTargetCid( 0 ),
      m_duelStartTime( 0 ),
      m_personality( (BotPersonality)( rand( ) % 8 ) ),
      m_lastShopBrowseTime( 0 ),
      m_lastBuyListRefreshTime( 0 ),
      m_lastWhisperTime( 0 ),
      m_lastArenaQueueTime( 0 ),
      m_lastDungeonCheckTime( 0 ),
      m_rallyBossCid( 0 ),
      m_lastRaidKiteTime( 0 ),
      m_lastVendorRunTime( 0 ),
      m_lastHaggleTime( 0 ),
      m_lastClanRivalryTime( 0 ),
      m_lastBountyCheckTime( 0 ),
      m_lastRecruitmentShoutTime( 0 ),
      m_lastCasualChatterTime( 0 ),
      m_isCompanion( false ),
      m_companionOwner( NULL ),
      m_role( BOT_ROLE_GENERAL_VISITOR ),
      m_stance( BOT_STANCE_DEFENSIVE ),
      m_comboStep( 0 ),
      m_lastQuestScanTime( 0 ),
      m_lastRestBanterTime( 0 ),
      m_markedTargetCid( 0 ),
      m_isPullingTarget( false ),
      m_affinity( 50 ),
      m_lastAffinityBanterTime( 0 ),
      m_lastInterruptTime( 0 ),
      m_lastConvoyMountTime( 0 ),
      m_tactic( BOT_TACTIC_BALANCED ),
      m_lastResupplyTime( 0 ),
      m_lastClanSyncTime( 0 ),
      m_lastCcTime( 0 ),
      m_lastPuzzleCheck( 0 ),
      m_synergy( 0 ),
      m_siegeMode( 0 ),
      m_lastUnionSyncTime( 0 ),
      m_lastFeastTime( 0 ),
      m_feastRestStart( 0 )
{
    m_pullAnchorPos.x = 0.0f;
    m_pullAnchorPos.y = 0.0f;
    m_pullAnchorPos.z = 0.0f;
    if ( player && player->CharInfo )
    {
        m_role = DetermineRoleFromJob( player->CharInfo->Job );
    }
    m_lastAiTick = clock( );
    m_stateTimer = clock( );
    m_stuckTimer = clock( );
    m_lastSkillCastTime = clock( );
    m_lastPlayerNearby = clock( );

    // Stagger initial social and activity timers to prevent packet flooding on client login
    clock_t now = clock( );
    m_lastWhisperTime = now + ( ( 120 + ( rand( ) % 360 ) ) * CLOCKS_PER_SEC );
    m_lastGreetingTime = now + ( ( 45 + ( rand( ) % 180 ) ) * CLOCKS_PER_SEC );
    m_lastChatterTime = now + ( ( 60 + ( rand( ) % 240 ) ) * CLOCKS_PER_SEC );
    m_lastVendingSay = now + ( ( 30 + ( rand( ) % 120 ) ) * CLOCKS_PER_SEC );
    m_lastPartyInviteTime = now + ( ( 90 + ( rand( ) % 300 ) ) * CLOCKS_PER_SEC );
    m_lastLfgShoutTime = now + ( ( 120 + ( rand( ) % 480 ) ) * CLOCKS_PER_SEC );
    m_lastBossRaidCheckTime = now + ( ( 180 + ( rand( ) % 600 ) ) * CLOCKS_PER_SEC );
    m_lastTownRepairTime = now + ( ( 180 + ( rand( ) % 600 ) ) * CLOCKS_PER_SEC );
    m_lastArenaQueueTime = now + ( ( 180 + ( rand( ) % 600 ) ) * CLOCKS_PER_SEC );
    m_lastDungeonCheckTime = now + ( ( 180 + ( rand( ) % 600 ) ) * CLOCKS_PER_SEC );
    m_lastShopBrowseTime = now + ( ( 120 + ( rand( ) % 300 ) ) * CLOCKS_PER_SEC );
    m_lastVendorRunTime = now + ( ( 300 + ( rand( ) % 600 ) ) * CLOCKS_PER_SEC );

    if ( m_player )
    {
        m_roamCenter = m_player->Position->current;
        m_lastPos = m_player->Position->current;
        m_lastKnownLevel = m_player->Stats->Level;
        m_strollDest = m_roamCenter;
        AssignClanTag( );
        AssignTitle( );
        ApplyRandomCosmetics( );
        AssignNewBounty( );
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
        case BOT_STATE_BUFF_BOT:   return "BUFF_BOT";
        case BOT_STATE_SEEK_BUFF:  return "SEEK_BUFF";
        case BOT_STATE_VENDING:    return "VENDING";
        case BOT_STATE_TOWN_STROLL: return "TOWN_STROLL";
        case BOT_STATE_MIGRATE:    return "MIGRATE";
        case BOT_STATE_DUEL:       return "DUEL";
        case BOT_STATE_TOWN_REPAIR: return "TOWN_REPAIR";
        case BOT_STATE_LFG_SHOUT:  return "LFG_SHOUT";
        case BOT_STATE_RALLY_RAID:  return "RALLY_RAID";
        case BOT_STATE_VENDOR_RUN:  return "VENDOR_RUN";
        case BOT_STATE_QUEST_RETURN: return "QUEST_RETURN";
        case BOT_STATE_CLAN_HANGOUT: return "CLAN_HANGOUT";
        case BOT_STATE_SIEGE_UNIT:   return "SIEGE_UNIT";
        default:                   return "UNKNOWN";
    }
}

const char* CPlayerBot::GetStanceString( ) const
{
    switch ( m_stance )
    {
        case BOT_STANCE_AGGRESSIVE: return "Aggressive (Hunter)";
        case BOT_STANCE_PASSIVE:    return "Passive (Hold Fire)";
        case BOT_STANCE_ASSIST:     return "Assist (Focus Fire)";
        case BOT_STANCE_DEFENSIVE:
        default:                    return "Defensive (Bodyguard)";
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

void CPlayerBot::DoEmote( BYTE emoteId )
{
    if ( !m_player ) return;
    clock_t now = clock( );
    if ( ( now - m_lastEmoteTime ) < ( 2 * CLOCKS_PER_SEC ) ) return;
    m_lastEmoteTime = now;

    BEGINPACKET( pak, 0x781 );
    ADDWORD    ( pak, m_player->clientid );
    ADDBYTE    ( pak, emoteId );
    GServer->SendToVisible( &pak, m_player );
}

void CPlayerBot::ConsumePotions( )
{
    if ( !m_player || m_player->IsDead( ) || m_player->Stats->HP <= 0 ) return;
    clock_t now = clock( );

    // HP Potion Check (< 55% HP)
    if ( m_player->Stats->HP < ( m_player->Stats->MaxHP * 55 / 100 ) )
    {
        if ( ( now - m_lastHpPotionTime ) >= ( 3 * CLOCKS_PER_SEC ) )
        {
            m_lastHpPotionTime = now;
            long long heal = m_player->Stats->MaxHP * 35 / 100;
            if ( heal < 50 ) heal = 50;
            m_player->Stats->HP += heal;
            if ( m_player->Stats->HP > m_player->Stats->MaxHP )
                m_player->Stats->HP = m_player->Stats->MaxHP;

            m_player->lastShowTime = 0;
            m_player->RefreshHPMP( );

            BEGINPACKET( pak, 0x7a3 );
            ADDWORD    ( pak, m_player->clientid );
            ADDWORD    ( pak, 301 );
            GServer->SendToVisible( &pak, m_player );

            if ( m_isCompanion && m_state == BOT_STATE_COMBAT )
            {
                Say( "Using an HP Potion to stay in the fight!" );
            }
            else
            {
                SayChatter( "low_hp" );
            }
        }
    }

    // MP Potion Check (< 40% MP)
    if ( m_player->Stats->MP < ( m_player->Stats->MaxMP * 40 / 100 ) )
    {
        if ( ( now - m_lastMpPotionTime ) >= ( 3 * CLOCKS_PER_SEC ) )
        {
            m_lastMpPotionTime = now;
            long long mana = m_player->Stats->MaxMP * 40 / 100;
            if ( mana < 40 ) mana = 40;
            m_player->Stats->MP += mana;
            if ( m_player->Stats->MP > m_player->Stats->MaxMP )
                m_player->Stats->MP = m_player->Stats->MaxMP;

            m_player->lastShowTime = 0;
            m_player->RefreshHPMP( );

            BEGINPACKET( pak, 0x7a3 );
            ADDWORD    ( pak, m_player->clientid );
            ADDWORD    ( pak, 311 );
            GServer->SendToVisible( &pak, m_player );

            if ( m_isCompanion && m_state == BOT_STATE_COMBAT )
            {
                Say( "Drinking a Mana Potion for skill energy!" );
            }
        }
    }
}

bool CPlayerBot::KiteTarget( CCharacter* target )
{
    if ( !m_player || !target || target->IsDead( ) ) return false;

    int wType = m_player->getWeaponType( );
    bool isRanged = ( wType == BOW || wType == GUN || wType == LAUNCHER || wType == CROSSBOW || wType == WAND || wType == STAFF );
    if ( !isRanged ) return false;

    float dist = GServer->distance( m_player->Position->current, target->Position->current );
    if ( dist < 7.5f && target->Battle && target->Battle->target == m_player->clientid )
    {
        fPoint kitePos;
        float dx = m_player->Position->current.x - target->Position->current.x;
        float dy = m_player->Position->current.y - target->Position->current.y;
        float len = sqrtf( dx * dx + dy * dy );
        if ( len < 0.1f ) { dx = 1.0f; dy = 0.0f; len = 1.0f; }

        kitePos.x = m_player->Position->current.x + ( dx / len ) * 12.0f;
        kitePos.y = m_player->Position->current.y + ( dy / len ) * 12.0f;
        kitePos.z = m_player->Position->current.z;

        MoveTo( kitePos );
        return true;
    }
    return false;
}

bool CPlayerBot::IsKillSteal( CMonster* mob )
{
    if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 ) return false;
    if ( !mob->Battle || mob->Battle->target == 0 ) return false;

    if ( mob->Battle->target == m_player->clientid ) return false;

    if ( m_player->Party && m_player->Party->party )
    {
        CParty* party = m_player->Party->party;
        for ( size_t i = 0; i < party->Members.size( ); i++ )
        {
            if ( party->Members[i] && party->Members[i]->clientid == mob->Battle->target )
            {
                return false;
            }
        }
    }
    return true;
}

void CPlayerBot::CelebrateLevelUp( )
{
    if ( !m_player ) return;

    BEGINPACKET( pak, 0x7b1 );
    ADDWORD    ( pak, m_player->clientid );
    ADDWORD    ( pak, m_player->Stats->Level );
    GServer->SendToVisible( &pak, m_player );

    DoEmote( 6 );

    char msg[80];
    const char* dings[] = {
        "Ding! Level %d!",
        "Awesome! Reached level %d!",
        "Level %d at last! Onward!",
        "Ding! GG everyone!"
    };
    snprintf( msg, sizeof(msg), dings[rand() % 4], m_player->Stats->Level );
    Say( msg );
}

void CPlayerBot::SayChatter( const char* category )
{
    if ( !m_player || !category ) return;
    clock_t now = clock( );
    if ( ( now - m_lastChatterTime ) < ( 30 * CLOCKS_PER_SEC ) ) return;

    // Roll d20: 12 or higher required (~45% chance to speak on chatter event)
    if ( RollD20( ) < 12 ) return;

    if ( strcmp( category, "low_hp" ) == 0 )
    {
        const char* msgs[] = { "Whoa, that hit hard!", "Chugging a potion!", "Need to stay alive!" };
        Say( msgs[rand() % 3] );
        m_lastChatterTime = now + ( ( rand( ) % 15 ) * CLOCKS_PER_SEC );
    }
    else if ( strcmp( category, "rare_drop" ) == 0 )
    {
        const char* msgs[] = { "Nice drop!", "Sweet loot!", "Jackpot!", "I'm taking this!" };
        Say( msgs[rand() % 4] );
        m_lastChatterTime = now + ( ( rand( ) % 15 ) * CLOCKS_PER_SEC );
    }
    else if ( strcmp( category, "greeting" ) == 0 )
    {
        const char* msgs[] = { "Hey there! Good luck!", "Yo! Safe travels!", "Hi everyone!", "Happy grinding!" };
        Say( msgs[rand() % 4] );
        m_lastChatterTime = now + ( ( rand( ) % 15 ) * CLOCKS_PER_SEC );
    }
    else if ( strcmp( category, "combat_cheer" ) == 0 )
    {
        const char* msgs[] = { "Take that!", "Down you go!", "One more down!", "Bullseye!" };
        Say( msgs[rand() % 4] );
        m_lastChatterTime = now + ( ( rand( ) % 15 ) * CLOCKS_PER_SEC );
    }
}

void CPlayerBot::CheckPartyInvitations( )
{
    if ( !m_player || m_isBuffBot || m_isVendingBot ) return;
    clock_t now = clock( );
    if ( ( now - m_lastPartyInviteTime ) < ( 30 * CLOCKS_PER_SEC ) ) return;
    m_lastPartyInviteTime = now;

    if ( m_player->Party->party != NULL )
    {
        if ( m_player->Party->party->Members[0] != m_player ) return;
        if ( m_player->Party->party->Members.size( ) >= 5 ) return; // Limit grinding parties to 5 members maximum
    }

    // Roll d20: 12 or higher required to want to invite someone right now (~45% chance)
    if ( RollD20( ) < 12 ) return;

    CMap* map = GetMap( );
    if ( !map ) return;

    for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* other = map->PlayerList[i];
        if ( !other || other == m_player || other->IsDead( ) ) continue;
        if ( other->bot_ai && ( other->bot_ai->IsBuffBot( ) || other->bot_ai->IsVendingBot( ) ) ) continue;
        if ( other->Party->party != NULL ) continue;

        int lvlDiff = abs( (int)m_player->Stats->Level - (int)other->Stats->Level );
        if ( lvlDiff <= 7 )
        {
            float dist = GServer->distance( m_player->Position->current, other->Position->current );
            if ( dist <= 20.0f )
            {
                if ( m_player->Party->party == NULL )
                {
                    CParty* party = new CParty( );
                    party->AddPlayer( m_player );
                    party->AddPlayer( other );
                }
                else
                {
                    m_player->Party->party->AddPlayer( other );
                }

                char msg[80];
                snprintf( msg, sizeof(msg), "Hey %s, join my party! Let's team up!", other->CharInfo->charname );
                Say( msg );

                if ( other->bot_ai )
                {
                    CPlayerBot* otherBot = reinterpret_cast<CPlayerBot*>( other->bot_ai );
                    otherBot->SetFollowTarget( m_player );
                    otherBot->SetState( BOT_STATE_FOLLOW );
                }
                break;
            }
        }
    }
}

void CPlayerBot::ApplyRandomCosmetics( )
{
    if ( !m_player || !m_player->CharInfo ) return;

    static const int s_validFaceIds[] = { 1, 8, 15, 22, 29, 36, 43 };
    static const size_t s_validFaceCount = sizeof( s_validFaceIds ) / sizeof( s_validFaceIds[0] );
    m_player->CharInfo->Face = s_validFaceIds[ rand( ) % s_validFaceCount ];

    static const int s_validHairIds[] = {
        0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18, 20, 21, 22, 23
    };
    static const size_t s_validHairCount = sizeof( s_validHairIds ) / sizeof( s_validHairIds[0] );
    m_player->CharInfo->Hair = s_validHairIds[ rand( ) % s_validHairCount ];

    int lvl = m_player->Stats->Level;
    if ( lvl >= 30 )
    {
        int backId = ( rand( ) % 3 == 0 ) ? ( 1 + rand( ) % 15 ) : 0;
        if ( backId > 0 )
        {
            m_player->items[13].itemtype = 11;
            m_player->items[13].itemnum = backId;
            m_player->items[13].durability = 40;
        }
    }
}

void CPlayerBot::CheckZoneMigration( )
{
    if ( !m_player || m_isBuffBot || m_isVendingBot || m_followTarget || ( m_player->Party && m_player->Party->party ) ) return;
    clock_t now = clock( );
    if ( ( now - m_lastMigrationCheck ) < ( 60 * CLOCKS_PER_SEC ) ) return;
    m_lastMigrationCheck = now;

    int lvl = m_player->Stats->Level;
    int currMap = m_player->Position->Map;
    int targetMap = currMap;
    fPoint targetPos = m_player->Position->current;

    if ( lvl >= 45 && currMap != 2 )
    {
        targetMap = 2; // Junon Polis
        targetPos = { 5655.0f, 5238.0f, 0.0f };
    }
    else if ( lvl >= 25 && lvl < 45 && currMap == 22 )
    {
        targetMap = 23; // Breezy Hills
        targetPos = { 5096.0f, 4905.0f, 0.0f };
    }
    else if ( lvl >= 10 && lvl < 25 && currMap == 22 )
    {
        targetMap = 1; // Zant
        targetPos = { 5240.0f, 5190.0f, 0.0f };
    }

    if ( targetMap != currMap && targetMap < (UINT)GServer->MapList.max )
    {
        CMap* nextMap = GServer->MapList.Index[targetMap];
        if ( nextMap && nextMap != GServer->MapList.nullzone )
        {
            Log( MSG_INFO, "Bot '%s' (Lvl %d) migrating from map %d to map %d",
                 m_player->CharInfo->charname, lvl, currMap, targetMap );
            Say( "Time to move to a higher level zone!" );
            nextMap->TeleportPlayer( m_player, targetPos, false );
            m_roamCenter = targetPos;
            m_roamRadius = 400.0f;
            EquipTieredGear( true );
        }
    }
}

void CPlayerBot::MoveTo( fPoint dest )
{
    if ( !m_player ) return;

    // If already moving towards approximately the same destination, don't spam 0x79a packet
    if ( m_player->IsMoving( ) && GServer->distance( m_player->Position->destiny, dest ) < 3.0f )
    {
        return;
    }

    // Natural movement jitter offset (+/- 1.0 meters) applied ONLY when initiating a new movement
    if ( !m_isBuffBot && !m_isVendingBot && m_state != BOT_STATE_FOLLOW )
    {
        float jitterX = ( ( rand( ) % 200 ) - 100 ) / 100.0f;
        float jitterY = ( ( rand( ) % 200 ) - 100 ) / 100.0f;
        dest.x += jitterX;
        dest.y += jitterY;
    }

    if ( m_player->Status->Stance == 1 )
    {
        StandUp( );
    }

    float moveDist = GServer->distance( m_player->Position->current, dest );

    // Check if high enough level to drive a Kart/Car (Level 30+) for long distance travel (>25m) out of combat
    if ( m_player->Stats->Level >= 30 && moveDist >= 25.0f && !m_player->IsOnBattle( ) && !m_isBuffBot && !m_isVendingBot )
    {
        if ( m_player->Stats->Level >= 70 )
        {
            // Castle Gear 01 Set: Body 367, Engine 31, Legs 377, Arms 387
            m_player->items[135].itemnum = 367; // Body (CastleGear01_BODY01.txt)
            m_player->items[136].itemnum = 31;  // Engine (CastleGear01_ENGINE01.txt)
            m_player->items[136].lifespan = 1000; // Fuel
            m_player->items[137].itemnum = 377; // Legs (CastleGear01_LEG01.txt)
            m_player->items[138].itemnum = 387; // Arms (CastleGear01_ARM01.txt)
        }
        else
        {
            // Cart 01 Set: Body 2, Engine 35, Wheels 68
            m_player->items[135].itemnum = 2;   // Frame / Body (cart01_BODY03_CBK.txt)
            m_player->items[136].itemnum = 35;  // Engine (cart01_ENGINE06.txt)
            m_player->items[136].lifespan = 1000; // Fuel
            m_player->items[137].itemnum = 68;  // Wheels (cart01_WHEEL01.txt)
            m_player->items[138].itemnum = 0;   // No Arms for Cart
        }

        if ( m_player->Status->Stance != DRIVING )
        {
            m_player->Status->Stance = DRIVING;
            BEGINPACKET( pakStance, 0x0782 );
            ADDWORD    ( pakStance, m_player->clientid );
            ADDBYTE    ( pakStance, 0x04 ); // Stance 4: DRIVING
            ADDWORD    ( pakStance, m_player->Stats->Base_Speed );
            GServer->SendToVisible( &pakStance, m_player );
        }
    }
    else
    {
        // Demount if entering combat or close range
        if ( m_player->Status->Stance == DRIVING && ( moveDist < 12.0f || m_player->IsOnBattle( ) ) )
        {
            m_player->Status->Stance = RUNNING;
            BEGINPACKET( pakStance, 0x0782 );
            ADDWORD    ( pakStance, m_player->clientid );
            ADDBYTE    ( pakStance, 0x03 ); // Stance 3: RUNNING
            ADDWORD    ( pakStance, m_player->Stats->Base_Speed );
            GServer->SendToVisible( &pakStance, m_player );
        }
        else if ( m_player->Status->Stance != DRIVING )
        {
            m_player->Status->Stance = RUNNING;
        }
    }

    m_player->Position->destiny = dest;
    m_player->Position->lastMoveTime = clock( );
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
    if ( m_player->Status->Stance != DRIVING )
    {
        m_player->Status->Stance = RUNNING; // Default stance is ALWAYS RUNNING
    }

    if ( !m_player->IsOnBattle( ) )
    {
        BEGINPACKET( pak, 0x770 );
        ADDWORD    ( pak, m_player->clientid );
        ADDFLOAT   ( pak, m_player->Position->current.x * 100 );
        ADDFLOAT   ( pak, m_player->Position->current.y * 100 );
        ADDWORD    ( pak, (WORD)( m_player->Position->current.z * 100 ) );
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
        capNum = 0;
        if ( lvl < 20 )
        {
            bodyNum = 31; armNum = 31; footNum = 31;
            wepNum = ( lvl >= 8 ) ? 2 : 1; // Short Sword / Wooden Sword
            shieldNum = ( job == 122 ) ? 0 : 1; // Wooden Shield (Knight/Soldier tank uses shield, Champion does not)
        }
        else if ( lvl < 40 )
        {
            bodyNum = 32; armNum = 32; footNum = 32;
            wepNum = ( lvl >= 32 ) ? 4 : 3; // Khukuri / Rapier
            shieldNum = ( job == 122 ) ? 0 : 3; // Round Shield
        }
        else if ( lvl < 70 )
        {
            bodyNum = 34; armNum = 34; footNum = 34;
            wepNum = ( lvl >= 58 ) ? 6 : 5; // Bushido / Long Sword
            shieldNum = ( job == 122 ) ? 0 : 7; // Kite Shield
        }
        else
        {
            bodyNum = 37; armNum = 37; footNum = 37;
            wepNum = ( lvl >= 84 ) ? 8 : 7; // Elven Sword / Saber
            shieldNum = ( job == 122 ) ? 0 : 10; // Plate Shield
        }
    }
    else if ( job == 211 || job == 221 || job == 222 ) // Muse / Mage / Cleric
    {
        capNum = 0;
        if ( lvl < 20 )
        {
            bodyNum = 61; armNum = 61; footNum = 61;
            wepNum = 301; // Baobab Rod
        }
        else if ( lvl < 40 )
        {
            bodyNum = 63; armNum = 63; footNum = 63;
            wepNum = 302; // Lemmings Rod
        }
        else if ( lvl < 70 )
        {
            bodyNum = 66; armNum = 66; footNum = 66;
            wepNum = 304; // Mage's Rod
        }
        else
        {
            bodyNum = 71; armNum = 71; footNum = 71;
            wepNum = 305; // White Staff
        }
    }
    else if ( job == 311 || job == 321 || job == 322 ) // Hawker / Raider / Scout
    {
        capNum = 0;
        ammoType = 1;
        if ( lvl < 20 )
        {
            bodyNum = 91; armNum = 91; footNum = 91;
            wepNum = 201; // Toy Bow
        }
        else if ( lvl < 40 )
        {
            bodyNum = 93; armNum = 93; footNum = 93;
            wepNum = 202; // Short Bow
        }
        else if ( lvl < 70 )
        {
            bodyNum = 96; armNum = 96; footNum = 96;
            wepNum = 203; // Long Bow
        }
        else
        {
            bodyNum = 99; armNum = 99; footNum = 99;
            wepNum = 205; // White Wing Bow
        }
    }
    else if ( job == 411 || job == 421 || job == 422 ) // Dealer / Bourgeois / Artisan
    {
        capNum = 0;
        ammoType = 2;
        if ( lvl < 20 )
        {
            bodyNum = 121; armNum = 121; footNum = 121;
            wepNum = 231; // Bubble Gun
        }
        else if ( lvl < 40 )
        {
            bodyNum = 123; armNum = 123; footNum = 123;
            wepNum = 232; // Air Gun
        }
        else if ( lvl < 70 )
        {
            bodyNum = 125; armNum = 125; footNum = 125;
            wepNum = 234; // Iron Rifle
        }
        else
        {
            bodyNum = 126; armNum = 126; footNum = 126;
            wepNum = 235; // Iron Shotgun
        }
    }
    else // Visitor / Novice
    {
        capNum = 0;
        bodyNum = 29;
        armNum = 21;
        footNum = 29;
        if ( m_role == BOT_ROLE_TANK )
        {
            wepNum = ( lvl >= 8 ) ? 2 : 1;
            shieldNum = 1;
        }
        else if ( m_role == BOT_ROLE_HEALER || m_role == BOT_ROLE_DPS_MAGIC )
        {
            wepNum = 301; // Baobab Rod
            shieldNum = 0;
        }
        else if ( m_role == BOT_ROLE_DPS_PHYS_RANGED )
        {
            wepNum = 201; // Toy Bow
            shieldNum = 0;
            ammoType = 1;
        }
        else
        {
            wepNum = ( lvl >= 8 ) ? 2 : 1;
            shieldNum = 0;
        }
    }

    int oldItems[6] = {
        m_player->items[2].itemnum,
        m_player->items[3].itemnum,
        m_player->items[5].itemnum,
        m_player->items[6].itemnum,
        m_player->items[7].itemnum,
        m_player->items[8].itemnum
    };

    auto SetEquip = []( CItem& itm, int type, int num, int refine = 0 ) {
        if ( num > 0 )
        {
            itm.itemtype = type;
            itm.itemnum = num;
            itm.count = 1;
            itm.durability = 50;
            itm.lifespan = 100;
            itm.appraised = true;
            itm.refine = refine;
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

    int weaponRefine = 0;
    if ( lvl >= 100 ) weaponRefine = 10;
    else if ( lvl >= 70 ) weaponRefine = 9;
    else if ( lvl >= 35 ) weaponRefine = 7;
    else if ( lvl >= 20 ) weaponRefine = 4;

    SetEquip( m_player->items[2], CAP, capNum );
    SetEquip( m_player->items[3], BODY, bodyNum );
    SetEquip( m_player->items[5], GLOVE, armNum );
    SetEquip( m_player->items[6], SHOE, footNum );
    SetEquip( m_player->items[7], WEAPON, wepNum, weaponRefine );
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

bool CPlayerBot::GetLearnedSkill( UINT baseSkillId, UINT& outSkillId, int& outSkillLevel )
{
    if ( !m_player ) return false;
    for ( int i = 0; i < MAX_ALL_SKILL; i++ )
    {
        if ( m_player->cskills[i].id == baseSkillId && m_player->cskills[i].level > 0 )
        {
            outSkillLevel = m_player->cskills[i].level;
            outSkillId = m_player->cskills[i].id + outSkillLevel - 1;
            return true;
        }
    }
    return false;
}

bool CPlayerBot::HasSkill( UINT baseSkillId )
{
    UINT dummyId = 0;
    int dummyLvl = 0;
    return GetLearnedSkill( baseSkillId, dummyId, dummyLvl );
}

void CPlayerBot::UpdateSkills( )
{
    if ( !m_player ) return;
    int job = m_player->CharInfo->Job;
    int lvl = m_player->Stats->Level;

    // 1. Completely clear all skill slots so previous high-level skills are erased
    for ( int i = 0; i < MAX_ALL_SKILL; i++ )
    {
        m_player->cskills[i].id = 0;
        m_player->cskills[i].level = 0;
        m_player->cskills[i].cooldown_skill = 0;
        m_player->cskills[i].thisskill = NULL;
    }

    std::vector<UINT> skillsToLearn;

    // Visitor / Universal novice attack (available to all levels, including level 1)
    skillsToLearn.push_back( 101 ); // Dual Scratch

    // Strictly enforce level and job requirements: below level 10, characters have NO CLASS and NO CLASS SKILLS!
    if ( lvl >= 10 && job != 0 )
    {
        if ( job == 111 || job == 121 || job == 122 ) // Soldier / Knight / Champion
        {
            skillsToLearn.push_back( 261 ); // Double Attack (Lv 10+)
            if ( lvl >= 20 ) skillsToLearn.push_back( 291 ); // Fatal Thrust (Lv 20+)
            if ( lvl >= 35 ) skillsToLearn.push_back( 336 ); // Piercing Lunge (Lv 35+)
            if ( lvl >= 50 ) skillsToLearn.push_back( 351 ); // Voltage Crash (Lv 50+)
        }
        else if ( job == 211 || job == 221 || job == 222 ) // Muse / Mage / Cleric
        {
            // Level 10 Muse: Cure & Mana Bolt
            skillsToLearn.push_back( 921 ); // Cure (Lv 10+)
            skillsToLearn.push_back( 851 ); // Mana Bolt (Lv 10+)

            // Level 20 Muse: Ice Bolt & Hustle Charm
            if ( lvl >= 20 )
            {
                skillsToLearn.push_back( 821 ); // Ice Bolt
                skillsToLearn.push_back( 926 ); // Hustle Charm (Base ID 926, Dash / Move speed)
            }

            // Level 35 Muse: Mana Spear, Recovery, Clobber Charm
            if ( lvl >= 35 )
            {
                skillsToLearn.push_back( 861 ); // Mana Spear
                skillsToLearn.push_back( 976 ); // Recovery (Base ID 976)
                skillsToLearn.push_back( 1266 ); // Clobber Charm (Base ID 1266, Attack power)
            }

            // Level 50+ Cleric / High Muse
            if ( lvl >= 50 )
            {
                if ( job == 222 || job == 211 ) // Cleric or high Muse
                {
                    skillsToLearn.push_back( 996 );  // Resilience Charm (Base ID 996, Defense)
                    skillsToLearn.push_back( 1246 ); // Battle Charm (Base ID 1246, Attack speed)
                    skillsToLearn.push_back( 1011 ); // Precision Charm (Base ID 1011, Accuracy)
                    skillsToLearn.push_back( 1031 ); // Party Heal
                    skillsToLearn.push_back( 1051 ); // Resurrection
                }
                if ( job == 221 ) // Mage
                {
                    skillsToLearn.push_back( 871 ); // Voltage Jolt
                }
            }
        }
        else if ( job == 311 || job == 321 || job == 322 ) // Hawker / Raider / Scout
        {
            skillsToLearn.push_back( 1441 ); // Double Arrow (Lv 10+)
            if ( lvl >= 20 ) skillsToLearn.push_back( 1431 ); // Clamp Arrow (Lv 20+)
            if ( lvl >= 35 ) skillsToLearn.push_back( 1486 ); // Triple Arrow (Lv 35+)
            if ( lvl >= 50 ) skillsToLearn.push_back( 1501 ); // Stun Arrow (Lv 50+)
        }
        else if ( job == 411 || job == 421 || job == 422 ) // Dealer / Bourgeois / Artisan
        {
            skillsToLearn.push_back( 2036 ); // Twin Shot (Lv 10+)
            if ( lvl >= 25 ) skillsToLearn.push_back( 2061 ); // Sniping (Lv 25+)
            if ( lvl >= 40 ) skillsToLearn.push_back( 2301 ); // Triple Shot (Lv 40+)
            if ( lvl >= 55 ) skillsToLearn.push_back( 2076 ); // Demolition Expertise (Lv 55+)
        }
    }

    // Populate cskills with appropriate skill levels strictly scaled to current level
    for ( size_t i = 0; i < skillsToLearn.size( ) && i < MAX_ALL_SKILL; i++ )
    {
        UINT skId = skillsToLearn[i];
        CSkills* baseSk = GServer->GetSkillByID( skId );
        int skillLvl = 1;
        if ( baseSk )
        {
            int maxLvl = (int)baseSk->MaxLevel;
            if ( maxLvl < 1 ) maxLvl = 1;
            int reqLvl = (int)baseSk->clevel;
            if ( reqLvl < 10 ) reqLvl = 10;
            if ( lvl > reqLvl )
            {
                skillLvl = 1 + ( ( lvl - reqLvl ) / 7 );
            }
            if ( skillLvl > maxLvl ) skillLvl = maxLvl;
            if ( skillLvl < 1 ) skillLvl = 1;
        }

        m_player->cskills[i].id = skId;
        m_player->cskills[i].level = skillLvl;
        m_player->cskills[i].cooldown_skill = 0;
        m_player->cskills[i].thisskill = GServer->GetSkillByID( skId + skillLvl - 1 );
    }

    // Reset skill points
    m_player->CharInfo->SkillPoints = 0;
}

bool CPlayerBot::CastCombatSkill( CCharacter* target )
{
    if ( !m_player || !target || target->IsDead( ) || target->Stats->HP <= 0 ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 15 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastSkillCastTime ) < (clock_t)( 3.0f * CLOCKS_PER_SEC ) ) return false;

    int job = m_player->CharInfo->Job;
    int lvl = m_player->Stats->Level;
    UINT chosenSkill = 101; // Dual Scratch fallback
    const char* comboSay = NULL;

    // Level 1-9 or Visitor has NO class skills, only basic novice Dual Scratch
    if ( lvl < 10 || job == 0 )
    {
        chosenSkill = 101;
        if ( m_isCompanion && ( rand( ) % 4 == 0 ) )
        {
            comboSay = "Novice strike! Dual Scratch!";
        }
    }
    else if ( job == 111 || job == 121 ) // Tank (Soldier / Knight)
    {
        // 3-Step Tank Synergy Combo:
        // Step 0: Opener & Taunt (Piercing Lunge / Double Attack)
        // Step 1: Armor Break (Fatal Thrust)
        // Step 2: High Threat Finisher (Voltage Crash)
        if ( m_comboStep == 0 )
        {
            if ( lvl >= 35 && HasSkill( 336 ) ) { chosenSkill = 336; comboSay = "Piercing Lunge! Eyes on me, monster!"; }
            else if ( HasSkill( 261 ) ) { chosenSkill = 261; comboSay = "Double Attack! Focus on my shield!"; }
            else { chosenSkill = 101; }
            m_comboStep = 1;
        }
        else if ( m_comboStep == 1 )
        {
            if ( lvl >= 20 && HasSkill( 291 ) ) { chosenSkill = 291; comboSay = "Fatal Thrust! Piercing defenses!"; }
            else if ( HasSkill( 261 ) ) { chosenSkill = 261; }
            else { chosenSkill = 101; }
            m_comboStep = 2;
        }
        else // Step 2
        {
            if ( lvl >= 50 && HasSkill( 351 ) ) { chosenSkill = 351; comboSay = "Voltage Crash! Threat locked down!"; }
            else if ( lvl >= 35 && HasSkill( 336 ) ) { chosenSkill = 336; }
            else if ( HasSkill( 261 ) ) { chosenSkill = 261; }
            else { chosenSkill = 101; }
            m_comboStep = 0;
        }

        // Taunt lock: ensure monster focuses the tank!
        if ( target->Battle )
        {
            target->Battle->target = m_player->clientid;
            target->Battle->hitby = m_player->clientid;
        }
    }
    else if ( job == 122 || job == 321 || job == 422 ) // Melee DPS (Champion / Raider / Artisan)
    {
        // 3-Step Melee Burst Combo:
        if ( m_comboStep == 0 )
        {
            if ( HasSkill( 261 ) ) { chosenSkill = 261; comboSay = "Double Attack!"; }
            else { chosenSkill = 101; }
            m_comboStep = 1;
        }
        else if ( m_comboStep == 1 )
        {
            if ( lvl >= 20 && HasSkill( 291 ) ) { chosenSkill = 291; comboSay = "Fatal Thrust! Heavy puncture!"; }
            else if ( HasSkill( 261 ) ) { chosenSkill = 261; }
            else { chosenSkill = 101; }
            m_comboStep = 2;
        }
        else
        {
            if ( lvl >= 50 && HasSkill( 351 ) ) { chosenSkill = 351; comboSay = "Voltage Crash finisher!"; }
            else if ( HasSkill( 261 ) ) { chosenSkill = 261; }
            else { chosenSkill = 101; }
            m_comboStep = 0;
        }
    }
    else if ( job == 211 || job == 221 ) // Magic DPS (Muse / Mage)
    {
        // 3-Step Spell Synergy Combo:
        // Step 0: CC / Movement Slow (Ice Bolt)
        // Step 1: Magic Burst (Mana Spear)
        // Step 2: Finisher (Voltage Jolt)
        if ( m_comboStep == 0 )
        {
            if ( lvl >= 20 && HasSkill( 821 ) ) { chosenSkill = 821; comboSay = "Ice Bolt! Chilling and slowing target!"; }
            else if ( HasSkill( 851 ) ) { chosenSkill = 851; comboSay = "Mana Bolt!"; }
            else { chosenSkill = 101; }
            m_comboStep = 1;
        }
        else if ( m_comboStep == 1 )
        {
            if ( lvl >= 35 && HasSkill( 861 ) ) { chosenSkill = 861; comboSay = "Mana Spear! High arcane damage!"; }
            else if ( HasSkill( 851 ) ) { chosenSkill = 851; }
            else { chosenSkill = 101; }
            m_comboStep = 2;
        }
        else
        {
            if ( lvl >= 50 && HasSkill( 871 ) ) { chosenSkill = 871; comboSay = "Voltage Jolt! Lightning overload!"; }
            else if ( lvl >= 35 && HasSkill( 861 ) ) { chosenSkill = 861; }
            else if ( HasSkill( 851 ) ) { chosenSkill = 851; }
            else { chosenSkill = 101; }
            m_comboStep = 0;
        }
    }
    else if ( job == 222 ) // Healer (Cleric)
    {
        // Healer offensive support combo (when party HP is safe)
        if ( m_comboStep == 0 )
        {
            if ( lvl >= 20 && HasSkill( 821 ) ) { chosenSkill = 821; comboSay = "Frost magic support! Slowing mob down!"; }
            else if ( HasSkill( 851 ) ) { chosenSkill = 851; }
            else { chosenSkill = 101; }
            m_comboStep = 1;
        }
        else
        {
            if ( lvl >= 35 && HasSkill( 861 ) ) { chosenSkill = 861; }
            else if ( HasSkill( 851 ) ) { chosenSkill = 851; }
            else { chosenSkill = 101; }
            m_comboStep = 0;
        }
    }
    else if ( job == 311 || job == 322 ) // Hawker / Scout
    {
        // 3-Step Ranged Combo:
        // Step 0: Pin / Cripple (Clamp Arrow)
        // Step 1: Rapid Volley (Triple Arrow / Double Arrow)
        // Step 2: Stun Arrow
        if ( m_comboStep == 0 )
        {
            if ( lvl >= 20 && HasSkill( 1431 ) ) { chosenSkill = 1431; comboSay = "Clamp Arrow! You are pinned!"; }
            else if ( HasSkill( 1441 ) ) { chosenSkill = 1441; comboSay = "Double Arrow!"; }
            else { chosenSkill = 101; }
            m_comboStep = 1;
        }
        else if ( m_comboStep == 1 )
        {
            if ( lvl >= 35 && HasSkill( 1486 ) ) { chosenSkill = 1486; comboSay = "Triple Arrow burst!"; }
            else if ( HasSkill( 1441 ) ) { chosenSkill = 1441; }
            else { chosenSkill = 101; }
            m_comboStep = 2;
        }
        else
        {
            if ( lvl >= 50 && HasSkill( 1501 ) ) { chosenSkill = 1501; comboSay = "Stun Arrow! Hold still, beast!"; }
            else if ( lvl >= 35 && HasSkill( 1486 ) ) { chosenSkill = 1486; }
            else if ( HasSkill( 1441 ) ) { chosenSkill = 1441; }
            else { chosenSkill = 101; }
            m_comboStep = 0;
        }
    }
    else if ( job == 411 || job == 421 ) // Dealer / Bourgeois
    {
        // 3-Step Gun / PAT Heavy Fire Combo
        if ( m_comboStep == 0 )
        {
            if ( HasSkill( 2036 ) ) { chosenSkill = 2036; comboSay = "Twin Shot volley!"; }
            else { chosenSkill = 101; }
            m_comboStep = 1;
        }
        else if ( m_comboStep == 1 )
        {
            if ( lvl >= 25 && HasSkill( 2061 ) ) { chosenSkill = 2061; comboSay = "Precision Sniping!"; }
            else if ( lvl >= 40 && HasSkill( 2301 ) ) { chosenSkill = 2301; comboSay = "Triple Shot burst!"; }
            else if ( HasSkill( 2036 ) ) { chosenSkill = 2036; }
            else { chosenSkill = 101; }
            m_comboStep = 2;
        }
        else
        {
            if ( lvl >= 55 && HasSkill( 2076 ) ) { chosenSkill = 2076; comboSay = "Demolition blast!"; }
            else if ( lvl >= 40 && HasSkill( 2301 ) ) { chosenSkill = 2301; }
            else if ( HasSkill( 2036 ) ) { chosenSkill = 2036; }
            else { chosenSkill = 101; }
            m_comboStep = 0;
        }
    }

    UINT finalSkillId = chosenSkill;
    int finalSkillLvl = 1;
    if ( !GetLearnedSkill( chosenSkill, finalSkillId, finalSkillLvl ) )
    {
        finalSkillId = 101;
    }

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

    if ( m_isCompanion && comboSay && ( rand( ) % 3 == 0 ) )
    {
        Say( comboSay );
    }

    return true;
}

bool CPlayerBot::CheckPartyHeal( )
{
    if ( !m_player || !m_player->Status->CanCastSkill || m_player->Stats->MP < 20 ) return false;
    int job = m_player->CharInfo->Job;
    // Strictly Muse (211) and Cleric (222) only; Visitors (0) and Mages (221) cannot heal!
    if ( job != 211 && job != 222 ) return false;
    if ( m_player->Stats->Level < 10 ) return false; // Minimum Level 10 required for Cure

    clock_t now = clock( );
    if ( ( now - m_lastSkillCastTime ) < (clock_t)( 2.5f * CLOCKS_PER_SEC ) ) return false;

    UINT healSkill = 0;
    int healLvl = 0;
    if ( !GetLearnedSkill( 976, healSkill, healLvl ) ) // Recovery
    {
        if ( !GetLearnedSkill( 921, healSkill, healLvl ) ) // Cure
        {
            return false;
        }
    }

    CSkills* sk = GServer->GetSkillByID( healSkill );
    if ( !sk ) return false;

    // Tactical Gambit thresholding
    int healThreshold = 70;
    if ( m_tactic == BOT_TACTIC_SAFE ) healThreshold = 85;
    else if ( m_tactic == BOT_TACTIC_SPEEDRUN ) healThreshold = 40;
    else if ( m_tactic == BOT_TACTIC_ECO ) healThreshold = 30;

    // 1. Check Party Leader (Real Player) - Highest Priority!
    CPlayer* leader = GetPartyLeader( );
    if ( leader && !leader->IsDead( ) && leader->Position && leader->Position->Map == m_player->Position->Map )
    {
        if ( leader->Stats->HP < ( leader->Stats->MaxHP * healThreshold / 100 ) &&
             GServer->distance( m_player->Position->current, leader->Position->current ) <= 25.0f )
        {
            m_player->StartAction( leader, SKILL_BUFF, healSkill );
            m_lastSkillCastTime = now;
            char healMsg[64];
            snprintf( healMsg, sizeof(healMsg), "Healing leader %s!", leader->CharInfo->charname );
            Say( healMsg );
            return true;
        }
    }

    // 2. Check Party Tank
    if ( m_player->Party && m_player->Party->party != NULL )
    {
        CParty* party = m_player->Party->party;
        for ( UINT i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* member = party->Members[i];
            if ( !member || member->IsDead( ) || member->Position->Map != m_player->Position->Map ) continue;
            if ( member->CharInfo->Job == 121 || member->CharInfo->Job == 111 ) // Tank
            {
                if ( member->Stats->HP < ( member->Stats->MaxHP * ( healThreshold - 5 ) / 100 ) &&
                     GServer->distance( m_player->Position->current, member->Position->current ) <= 25.0f )
                {
                    m_player->StartAction( member, SKILL_BUFF, healSkill );
                    m_lastSkillCastTime = now;
                    char healMsg[64];
                    snprintf( healMsg, sizeof(healMsg), "Healing tank %s!", member->CharInfo->charname );
                    Say( healMsg );
                    return true;
                }
            }
        }
    }

    // 3. Check Self
    if ( m_player->Stats->HP < ( m_player->Stats->MaxHP * ( healThreshold - 10 ) / 100 ) )
    {
        m_player->StartAction( m_player, SKILL_BUFF, healSkill );
        m_lastSkillCastTime = now;
        Say( "Healing myself!" );
        return true;
    }

    // 4. Check other party members
    if ( m_player->Party && m_player->Party->party != NULL )
    {
        CParty* party = m_player->Party->party;
        for ( UINT i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* member = party->Members[i];
            if ( member && !member->IsDead( ) && member != leader && member->Position->Map == m_player->Position->Map )
            {
                if ( member->Stats->HP < ( member->Stats->MaxHP * ( healThreshold - 5 ) / 100 ) &&
                     GServer->distance( m_player->Position->current, member->Position->current ) <= 22.0f )
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

bool CPlayerBot::CheckPartyBuffs( )
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 20 ) return false;

    int job = m_player->CharInfo->Job;
    // Strictly Muse (211) and Cleric (222) only! Visitors (0) and Mages (221) CANNOT buff!
    if ( job != 211 && job != 222 ) return false;
    if ( m_player->Stats->Level < 20 ) return false; // Earliest buff (Hustle Charm) requires Level 20!

    clock_t now = clock( );
    if ( ( now - m_lastPartyBuffTime ) < (clock_t)( 2.5f * CLOCKS_PER_SEC ) ) return false;

    CParty* party = m_player->Party->party;
    CPlayer* targetToBuff = NULL;
    UINT skillToCast = 0;
    const char* buffName = NULL;

    // Prioritize leader first, then rest of the party
    CPlayer* leader = GetPartyLeader( );
    std::vector<CPlayer*> targetsToCheck;
    if ( leader && !leader->IsDead( ) && leader->Position && leader->Position->Map == m_player->Position->Map )
    {
        targetsToCheck.push_back( leader );
    }
    for ( UINT i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* m = party->Members[i];
        if ( m && m != leader ) targetsToCheck.push_back( m );
    }

    for ( size_t i = 0; i < targetsToCheck.size( ); i++ )
    {
        CPlayer* member = targetsToCheck[i];
        if ( !member || member->IsDead( ) || member->Position->Map != m_player->Position->Map ) continue;
        if ( GServer->distance( m_player->Position->current, member->Position->current ) > 22.0f ) continue;

        UINT castId = 0;
        int castLvl = 0;

        // Check for missing buffs in priority order, strictly verifying the bot has learned them
        if ( member->Status->Dash_up == 0xff && GetLearnedSkill( 926, castId, castLvl ) )
        {
            targetToBuff = member;
            skillToCast = castId; // Exact learned level of Hustle Charm
            buffName = "Hustle";
            break;
        }
        else if ( member->Status->Attack_up == 0xff && GetLearnedSkill( 1266, castId, castLvl ) )
        {
            targetToBuff = member;
            skillToCast = castId; // Exact learned level of Clobber Charm
            buffName = "Clobber Charm";
            break;
        }
        else if ( member->Status->Defense_up == 0xff && GetLearnedSkill( 996, castId, castLvl ) )
        {
            targetToBuff = member;
            skillToCast = castId; // Exact learned level of Resilience Charm
            buffName = "Resilience";
            break;
        }
        else if ( member->Status->Haste_up == 0xff && GetLearnedSkill( 1246, castId, castLvl ) )
        {
            targetToBuff = member;
            skillToCast = castId; // Exact learned level of Battle Charm
            buffName = "Battle Charm";
            break;
        }
        else if ( member->Status->Accuracy_up == 0xff && GetLearnedSkill( 1011, castId, castLvl ) )
        {
            targetToBuff = member;
            skillToCast = castId; // Exact learned level of Precision Charm
            buffName = "Precision";
            break;
        }
    }

    if ( targetToBuff && skillToCast > 0 )
    {
        CSkills* sk = GServer->GetSkillByID( skillToCast );
        if ( sk )
        {
            m_player->StartAction( targetToBuff, SKILL_BUFF, skillToCast );
            m_lastPartyBuffTime = now;
            m_lastSkillCastTime = now;

            char buffMsg[64];
            if ( targetToBuff == m_player )
            {
                snprintf( buffMsg, sizeof(buffMsg), "Buffing myself with %s!", buffName );
            }
            else
            {
                snprintf( buffMsg, sizeof(buffMsg), "%s for %s!", buffName, targetToBuff->CharInfo->charname );
            }
            Say( buffMsg );
            return true;
        }
    }

    return false;
}

void CPlayerBot::ForcePartyBuff( )
{
    m_lastPartyBuffTime = 0;
    CheckPartyBuffs( );
}

bool CPlayerBot::CheckProximityBuffs( )
{
    if ( !m_player ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 30 ) return false;

    // Only Muse (211) and Cleric (222) can perform proximity buffing; Visitors (0) CANNOT!
    int job = m_player->CharInfo->Job;
    if ( job != 211 && job != 222 ) return false;
    if ( m_player->Stats->Level < 20 ) return false; // Minimum Level 20 required for any buffs

    clock_t now = clock( );
    if ( ( now - m_lastPartyBuffTime ) < (clock_t)( 5.0f * CLOCKS_PER_SEC ) ) return false;

    // Roll d20: 14 or higher required (~35% chance to buff a passing stranger)
    if ( RollD20( ) < 14 ) return false;

    CMap* map = GetMap( );
    if ( !map ) return false;

    CPlayer* targetToBuff = NULL;
    UINT skillToCast = 0;
    const char* buffName = NULL;

    // Scan candidates: real players first, then player bots
    for ( int pass = 0; pass < 2 && !targetToBuff; pass++ )
    {
        for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
        {
            CPlayer* p = map->PlayerList[i];
            if ( !p || p == m_player || p->IsDead( ) ) continue;
            if ( p->Session == NULL || !p->Session->inGame ) continue;

            // Pass 0: real players only; Pass 1: player bots (excluding other buff bots)
            if ( pass == 0 && p->bot_ai != NULL ) continue;
            if ( pass == 1 && ( p->bot_ai == NULL || ( reinterpret_cast<CPlayerBot*>( p->bot_ai )->IsBuffBot( ) ) ) ) continue;

            // MUST BE WITHIN SPELL CAST RANGE (max 16.0m)
            float dist = GServer->distance( m_player->Position->current, p->Position->current );
            if ( dist > 16.0f ) continue;

            UINT castId = 0;
            int castLvl = 0;

            // Priority checks for missing buffs or healing, strictly using learned skills
            if ( p->Stats->HP < ( p->Stats->MaxHP * 85 / 100 ) )
            {
                if ( GetLearnedSkill( 976, castId, castLvl ) || GetLearnedSkill( 921, castId, castLvl ) )
                {
                    targetToBuff = p;
                    skillToCast = castId;
                    buffName = "Heal";
                    break;
                }
            }
            if ( p->Status->Dash_up == 0xff && GetLearnedSkill( 926, castId, castLvl ) )
            {
                targetToBuff = p;
                skillToCast = castId;
                buffName = "Hustle";
                break;
            }
            else if ( p->Status->Attack_up == 0xff && GetLearnedSkill( 1266, castId, castLvl ) )
            {
                targetToBuff = p;
                skillToCast = castId;
                buffName = "Clobber Charm";
                break;
            }
            else if ( p->Status->Defense_up == 0xff && GetLearnedSkill( 996, castId, castLvl ) )
            {
                targetToBuff = p;
                skillToCast = castId;
                buffName = "Resilience";
                break;
            }
            else if ( p->Status->Haste_up == 0xff && GetLearnedSkill( 1246, castId, castLvl ) )
            {
                targetToBuff = p;
                skillToCast = castId;
                buffName = "Battle Charm";
                break;
            }
            else if ( p->Status->Accuracy_up == 0xff && GetLearnedSkill( 1011, castId, castLvl ) )
            {
                targetToBuff = p;
                skillToCast = castId;
                buffName = "Precision";
                break;
            }
        }
    }

    if ( targetToBuff && skillToCast > 0 )
    {
        CSkills* sk = GServer->GetSkillByID( skillToCast );
        if ( sk && m_player->IsTargetReached( targetToBuff, sk ) )
        {
            m_player->StartAction( targetToBuff, SKILL_BUFF, skillToCast );
            m_lastPartyBuffTime = now;
            m_lastSkillCastTime = now;

            char buffMsg[64];
            snprintf( buffMsg, sizeof(buffMsg), "%s for %s!", buffName, targetToBuff->CharInfo->charname );
            Say( buffMsg );
            return true;
        }
    }

    return false;
}

bool CPlayerBot::CheckPartyResurrect( )
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastResurrectTime ) < (clock_t)( 5.0f * CLOCKS_PER_SEC ) ) return false;

    CParty* party = m_player->Party->party;
    CPlayer* leader = GetPartyLeader( );

    // 1. EMERGENCY LEADER RESURRECTION PROTOCOL (Absolute Highest Priority!)
    if ( leader && leader->IsDead( ) && leader->Position->Map == m_player->Position->Map )
    {
        float distToLeader = GServer->distance( m_player->Position->current, leader->Position->current );
        if ( distToLeader <= 25.0f )
        {
            m_lastResurrectTime = now;
            m_lastSkillCastTime = now;

            bool isHealer = ( m_player->CharInfo->Job == 211 || m_player->CharInfo->Job == 221 || m_player->CharInfo->Job == 222 );

            leader->Stats->HP = leader->Stats->MaxHP * 40 / 100;
            leader->Stats->MP = leader->Stats->MaxMP * 30 / 100;
            leader->Status->Stance = RUNNING;
            leader->RefreshBuff( );
            leader->SetStats( );

            // Broadcast HP packet (0x79f) to visible & send directly to leader
            BEGINPACKET( pak, 0x79f );
            ADDWORD    ( pak, leader->clientid );
            ADDDWORD   ( pak, (DWORD)leader->Stats->HP );
            GServer->SendToVisible( &pak, leader );

            if ( leader->client )
            {
                leader->client->SendPacket( &pak );
                BEGINPACKET( pakExp, 0x79b );
                ADDDWORD   ( pakExp, leader->CharInfo->Exp );
                ADDWORD    ( pakExp, leader->CharInfo->stamina );
                ADDWORD    ( pakExp, 0 );
                leader->client->SendPacket( &pakExp );
            }

            AddAffinity( 10 );

            if ( isHealer )
            {
                char rezMsg[96];
                snprintf( rezMsg, sizeof(rezMsg), "Hang on, %s! Bringing you back into the fight!", leader->CharInfo->charname );
                Say( rezMsg );
            }
            else
            {
                char rezMsg[96];
                snprintf( rezMsg, sizeof(rezMsg), "Emergency medical protocol! %s, take my revival scroll!", leader->CharInfo->charname );
                Say( rezMsg );
            }
            return true;
        }
    }

    // 2. Normal party member resurrection by Clerics / Muses
    int job = m_player->CharInfo->Job;
    if ( job != 211 && job != 221 && job != 222 ) return false; // Muse / Mage / Cleric
    if ( m_player->Stats->Level < 30 || !m_player->Status->CanCastSkill || m_player->Stats->MP < 60 ) return false;

    for ( UINT i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* member = party->Members[i];
        if ( !member || !member->IsDead( ) ) continue;
        if ( member->Position->Map != m_player->Position->Map ) continue;

        float dist = GServer->distance( m_player->Position->current, member->Position->current );
        if ( dist <= 20.0f )
        {
            m_lastResurrectTime = now;
            m_lastSkillCastTime = now;

            // Revive ally with 35% HP and 25% MP
            member->Stats->HP = member->Stats->MaxHP * 35 / 100;
            member->Stats->MP = member->Stats->MaxMP * 25 / 100;
            member->Status->Stance = RUNNING;
            member->RefreshBuff( );
            member->SetStats( );

            // Broadcast HP packet (0x79f)
            BEGINPACKET( pak, 0x79f );
            ADDWORD    ( pak, member->clientid );
            ADDDWORD   ( pak, (DWORD)member->Stats->HP );
            GServer->SendToVisible( &pak, member );

            if ( member->client )
            {
                member->client->SendPacket( &pak );
            }

            // If reviving a bot, restore its AI state to follow
            if ( member->is_bot && member->bot_ai )
            {
                member->bot_ai->SetFollowTarget( m_followTarget ? m_followTarget : m_player );
                member->bot_ai->SetState( BOT_STATE_FOLLOW );
                member->bot_ai->Say( "Thank you for the revive!" );
            }

            char rezMsg[64];
            snprintf( rezMsg, sizeof(rezMsg), "Arise, %s! You are revived!", member->CharInfo->charname );
            Say( rezMsg );
            return true;
        }
    }
    return false;
}

bool CPlayerBot::CheckPartyTaunt( )
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return false;
    int job = m_player->CharInfo->Job;
    // Strictly Soldier (111), Knight (121), Champion (122) only; Visitors (0) cannot taunt!
    if ( job != 111 && job != 121 && job != 122 ) return false;
    if ( m_player->Stats->Level < 10 ) return false; // Minimum Level 10 required

    clock_t now = clock( );
    if ( ( now - m_lastTauntTime ) < (clock_t)( 3.0f * CLOCKS_PER_SEC ) ) return false;

    CMap* map = GetMap( );
    if ( !map ) return false;

    CParty* party = m_player->Party->party;
    CPlayer* leader = GetPartyLeader( );

    UINT tauntSkill = 0;
    int tauntLvl = 0;
    if ( !GetLearnedSkill( 336, tauntSkill, tauntLvl ) ) // Piercing Lunge
    {
        if ( !GetLearnedSkill( 261, tauntSkill, tauntLvl ) ) // Double Attack
        {
            return false;
        }
    }

    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) ) continue;
        if ( !mob->Battle || mob->Battle->target == 0 || mob->Battle->target == m_player->clientid ) continue;

        // 1. Highest Priority: Peel mobs attacking the real player / party leader!
        if ( leader && mob->Battle->target == leader->clientid )
        {
            float dist = GServer->distance( m_player->Position->current, mob->Position->current );
            if ( dist <= 25.0f )
            {
                m_lastTauntTime = now;
                AttackTarget( mob );
                SetState( BOT_STATE_COMBAT );

                if ( m_player->Stats->MP >= 15 )
                {
                    m_player->StartAction( mob, SKILL_ATTACK, tauntSkill );
                }

                char tauntMsg[64];
                snprintf( tauntMsg, sizeof(tauntMsg), "Leave %s alone! Over here, beast!", leader->CharInfo->charname );
                Say( tauntMsg );
                return true;
            }
        }

        // 2. Second Priority: Peel mobs attacking healers or other squishy party members
        for ( UINT p = 0; p < party->Members.size( ); p++ )
        {
            CPlayer* member = party->Members[p];
            if ( member && member != m_player && member != leader && member->clientid == mob->Battle->target )
            {
                float dist = GServer->distance( m_player->Position->current, mob->Position->current );
                if ( dist <= 22.0f )
                {
                    m_lastTauntTime = now;
                    AttackTarget( mob );
                    SetState( BOT_STATE_COMBAT );

                    if ( m_player->Stats->MP >= 15 )
                    {
                        m_player->StartAction( mob, SKILL_ATTACK, tauntSkill );
                    }

                    char tauntMsg[64];
                    snprintf( tauntMsg, sizeof(tauntMsg), "Back off %s!", member->CharInfo->charname );
                    Say( tauntMsg );
                    return true;
                }
            }
        }
    }

    return false;
}

CPlayer* CPlayerBot::GetPartyLeader( )
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return NULL;
    CParty* party = m_player->Party->party;

    for ( size_t i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* member = party->Members[i];
        if ( member && member != m_player && member->Party && member->Party->IsMaster )
        {
            return member;
        }
    }
    for ( size_t i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* member = party->Members[i];
        if ( member && member != m_player && !member->is_bot )
        {
            return member;
        }
    }
    for ( size_t i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* member = party->Members[i];
        if ( member && member != m_player )
        {
            return member;
        }
    }
    return NULL;
}

int CPlayerBot::GetPartySlotIndex( ) const
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return 0;
    CParty* party = m_player->Party->party;
    int slot = 0;
    for ( UINT i = 0; i < party->Members.size( ); i++ )
    {
        if ( party->Members[i] == m_player ) return slot;
        if ( party->Members[i] != m_followTarget )
        {
            slot++;
        }
    }
    return slot;
}

fPoint CPlayerBot::GetFormationOffset( int slotIndex, fPoint leaderCurrent, fPoint leaderDest )
{
    float baseAngle = 0.0f;
    float dx = leaderDest.x - leaderCurrent.x;
    float dy = leaderDest.y - leaderCurrent.y;
    float len = sqrt( dx * dx + dy * dy );
    if ( len > 0.5f )
    {
        baseAngle = atan2( dy, dx );
    }

    // Dungeon Corridor Tight Column Formation:
    // Narrow corridors require staggered single/double file to avoid clipping cave walls
    if ( m_player && IsDungeonMap( m_player->Position->Map ) )
    {
        float colDist = 2.0f + ( (float)slotIndex * 1.5f );
        float sideSign = ( slotIndex % 2 == 0 ) ? 1.0f : -1.0f;
        float colAngle = baseAngle + 3.14159265f + ( sideSign * 0.12f );
        fPoint target;
        target.x = leaderCurrent.x + cos( colAngle ) * colDist;
        target.y = leaderCurrent.y + sin( colAngle ) * colDist;
        target.z = leaderCurrent.z;
        return target;
    }

    float relAngle = 3.14159265f;
    float dist = 3.5f;

    // Role-based tactical positioning
    switch ( m_role )
    {
        case BOT_ROLE_TANK:
            relAngle = 0.40f;   // Front-Right Vanguard
            dist = 3.6f;
            break;
        case BOT_ROLE_DPS_PHYS_MELEE:
            relAngle = -0.40f;  // Front-Left Flanker
            dist = 3.6f;
            break;
        case BOT_ROLE_DPS_PHYS_RANGED:
            relAngle = 3.14159265f - 0.75f; // Rear-Right Wing
            dist = 4.8f;
            break;
        case BOT_ROLE_DPS_MAGIC:
            relAngle = 3.14159265f + 0.75f; // Rear-Left Wing
            dist = 4.8f;
            break;
        case BOT_ROLE_HEALER:
            relAngle = 3.14159265f; // Rear-Center Safe Distance
            dist = 5.2f;
            break;
        default:
            switch ( slotIndex % 5 )
            {
                case 0: relAngle = 0.50f; dist = 3.5f; break;
                case 1: relAngle = 3.14159265f; dist = 5.0f; break;
                case 2: relAngle = 3.14159265f - 0.8f; dist = 4.5f; break;
                case 3: relAngle = 3.14159265f + 0.8f; dist = 4.5f; break;
                default: relAngle = -0.50f; dist = 3.5f; break;
            }
            break;
    }

    // Subtle individual breathing offset per bot (prevents robotic identical trajectories)
    if ( m_player )
    {
        float jitterA = ( ( (int)( m_player->clientid * 37 ) % 20 ) - 10 ) * 0.015f;
        float jitterD = ( ( (int)( m_player->clientid * 53 ) % 20 ) - 10 ) * 0.025f;
        relAngle += jitterA;
        dist += jitterD;
    }

    float finalAngle = baseAngle + relAngle;
    fPoint target;
    target.x = leaderCurrent.x + cos( finalAngle ) * dist;
    target.y = leaderCurrent.y + sin( finalAngle ) * dist;
    target.z = leaderCurrent.z;
    return target;
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
        CelebrateLevelUp( );
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
        fPoint respawnPos = m_player->Position->current;
        if ( m_followTarget && m_followTarget->Position )
        {
            CMap* targetMap = GServer->MapList.Index[m_followTarget->Position->Map];
            if ( targetMap )
            {
                respawnPos = m_followTarget->Position->current;
                respawnPos.x += ( ( rand( ) % 40 ) - 20 ) / 10.0f;
                respawnPos.y += ( ( rand( ) % 40 ) - 20 ) / 10.0f;
                m_roamCenter = respawnPos;
                targetMap->TeleportPlayer( m_player, respawnPos, false );
            }
            else
            {
                map->TeleportPlayer( m_player, respawnPos, false );
            }
        }
        else if ( !m_isBuffBot && !m_isVendingBot )
        {
            // Respawn right where the bot died (with slight offset so bots don't overlap)
            respawnPos.x += ( ( rand( ) % 40 ) - 20 ) / 10.0f;
            respawnPos.y += ( ( rand( ) % 40 ) - 20 ) / 10.0f;
            m_roamCenter = respawnPos;
            map->TeleportPlayer( m_player, respawnPos, false );
        }
        else
        {
            respawnPos = m_roamCenter;
            map->TeleportPlayer( m_player, respawnPos, false );
        }
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

CMonster* CPlayerBot::FindNearbyWorldBoss( float radius )
{
    CMap* map = GetMap( );
    if ( !map || !m_player || !m_player->Position ) return NULL;

    fPoint myPos = m_player->Position->current;
    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || !mob->Position ) continue;
        if ( mob->IsBonfire( ) || mob->GetOwner( ) != NULL ) continue;

        if ( mob->Stats->MaxHP >= 4000 || mob->montype >= 400 )
        {
            float dist = GServer->distance( myPos, mob->Position->current );
            if ( dist <= radius )
            {
                return mob;
            }
        }
    }
    return NULL;
}

bool CPlayerBot::IsMobReserved( CMonster* mob )
{
    if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 ) return true;

    CMap* map = GetMap( );
    if ( !map || !m_player ) return false;

    int claimCount = 0;

    for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* otherPlayer = map->PlayerList[i];
        if ( !otherPlayer || otherPlayer == m_player ) continue;

        // Skip party members if in a party (party members can assist on same mob)
        if ( m_player->Party != NULL && m_player->Party->party != NULL )
        {
            CParty* party = m_player->Party->party;
            bool isPartyMember = false;
            for ( size_t m = 0; m < party->Members.size( ); m++ )
            {
                if ( party->Members[m] == otherPlayer )
                {
                    isPartyMember = true;
                    break;
                }
            }
            if ( isPartyMember ) continue;
        }

        if ( otherPlayer->bot_ai != NULL )
        {
            CPlayerBot* otherBot = reinterpret_cast<CPlayerBot*>( otherPlayer->bot_ai );
            if ( otherBot && otherBot->m_targetMobCid == mob->clientid )
            {
                claimCount++;
            }
        }
    }

    // Allow up to 2 non-party bots to target the same monster
    if ( claimCount >= 2 ) return true;

    return false;
}

bool CPlayerBot::IsMonsterQuestTarget( CMonster* mob, CPlayer* leader )
{
    if ( !mob || !leader || !leader->Position || !mob->Position ) return false;
    if ( mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) ) return false;

    bool hasActiveQuest = false;
    for ( int q = 0; q < 10; q++ )
    {
        if ( leader->quest.quests[q].QuestID != 0 )
        {
            hasActiveQuest = true;
            break;
        }
    }
    if ( !hasActiveQuest ) return false;

    // 1. Direct die_quest trigger in monster data
    if ( mob->die_quest != 0 ) return true;
    if ( mob->thisnpc && mob->thisnpc->die_quest != 0 ) return true;

    // 2. Leader is actively engaging or targeting this monster
    if ( leader->Battle && leader->Battle->target == mob->clientid ) return true;

    return false;
}

CMonster* CPlayerBot::FindNearbyQuestMonster( float radius )
{
    CPlayer* leader = GetPartyLeader( );
    if ( !leader || !leader->Position ) return NULL;

    CMap* map = GetMap( );
    if ( !map || !m_player || !m_player->Position ) return NULL;

    fPoint leaderPos = leader->Position->current;
    CMonster* bestMob = NULL;
    float bestDistSq = radius * radius;

    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || !mob->Position ) continue;
        if ( mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) ) continue;

        if ( !IsMonsterQuestTarget( mob, leader ) ) continue;

        float dx = leaderPos.x - mob->Position->current.x;
        if ( dx > radius || dx < -radius ) continue;
        float dy = leaderPos.y - mob->Position->current.y;
        if ( dy > radius || dy < -radius ) continue;

        float distSq = dx * dx + dy * dy;
        if ( distSq < bestDistSq )
        {
            bestDistSq = distSq;
            bestMob = mob;
        }
    }

    return bestMob;
}

bool CPlayerBot::CheckSynchronizedRest( )
{
    if ( !m_isCompanion || !m_player || !m_player->Position ) return false;
    CPlayer* leader = GetPartyLeader( );
    if ( !leader || !leader->Position || !leader->Status ) return false;
    if ( m_player->Position->Map != leader->Position->Map ) return false;

    // Stance 2 is SITTING in ROSE Online
    bool leaderSitting = ( leader->Status->Stance == 2 );
    bool selfSitting = ( m_player->Status && m_player->Status->Stance == 2 );

    if ( leaderSitting )
    {
        // If we are fighting, don't sit down unless monster is dead
        if ( m_state == BOT_STATE_COMBAT )
        {
            if ( !m_player->IsOnBattle( ) || m_player->Battle->target == 0 )
            {
                SetState( BOT_STATE_REST );
            }
            else
            {
                return false;
            }
        }

        if ( !selfSitting )
        {
            SitDown( );
            SetState( BOT_STATE_REST );

            clock_t now = clock( );
            if ( ( now - m_lastRestBanterTime ) > (clock_t)( 20 * CLOCKS_PER_SEC ) )
            {
                m_lastRestBanterTime = now;
                const char* restQuotes[] = {
                    "Resting up with you, leader.",
                    "Good fight! Taking a quick breather by the camp.",
                    "Catching my breath. Ready when you are!",
                    "Sitting down to recover health and mana."
                };
                Say( restQuotes[rand( ) % 4] );
            }
        }
        return true;
    }
    else
    {
        // Leader stood up or is moving!
        if ( selfSitting || m_state == BOT_STATE_REST )
        {
            StandUp( );
            SetState( BOT_STATE_FOLLOW );
            return true;
        }
    }

    return false;
}

CMonster* CPlayerBot::FindNearbyMonster( float radius )
{
    CMap* map = GetMap( );
    if ( !map || !m_player || !m_player->Position ) return NULL;

    fPoint myPos = m_player->Position->current;

    struct MobCandidate {
        CMonster* mob;
        float distSq;
        bool reserved;
    };
    std::vector<MobCandidate> unreservedCandidates;
    std::vector<MobCandidate> allCandidates;

    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || !mob->Position ) continue;
        if ( mob->IsBonfire( ) ) continue; 
        if ( mob->GetOwner( ) != NULL ) continue; 
        if ( IsKillSteal( mob ) ) continue; 

        float dx = myPos.x - mob->Position->current.x;
        if ( dx > radius || dx < -radius ) continue;
        float dy = myPos.y - mob->Position->current.y;
        if ( dy > radius || dy < -radius ) continue;

        float distSq = dx * dx + dy * dy;
        bool reserved = IsMobReserved( mob );

        allCandidates.push_back( { mob, distSq, reserved } );
        if ( !reserved )
        {
            unreservedCandidates.push_back( { mob, distSq, false } );
        }
    }

    // Prefer unreserved candidates to spread bots out naturally across mobs
    std::vector<MobCandidate>& pool = !unreservedCandidates.empty( ) ? unreservedCandidates : allCandidates;
    if ( pool.empty( ) ) return NULL;

    std::sort( pool.begin( ), pool.end( ), []( const MobCandidate& a, const MobCandidate& b ) {
        return a.distSq < b.distSq;
    } );

    int poolSize = ( pool.size( ) < 3 ) ? (int)pool.size( ) : 3;
    int choice = rand( ) % poolSize;

    return pool[choice].mob;
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
        if ( m_isBuffBot || m_isVendingBot )
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

    // Vending bot execution bypass
    if ( m_isVendingBot )
    {
        m_player->Stats->MP = m_player->Stats->MaxMP;
        if ( m_player->Stats->HP < m_player->Stats->MaxHP )
        {
            m_player->Stats->HP = m_player->Stats->MaxHP;
        }
        HandleVendingBot( );
        return;
    }

    // AI decision throttled to 500ms
    clock_t now = clock( );
    if ( ( now - m_lastAiTick ) < ( CLOCKS_PER_SEC / 2 ) )
    {
        return;
    }
    m_lastAiTick = now;

    // Phase 1: Potion consumption check
    ConsumePotions( );

    // Check progression, stat points, and tiered equipment
    CheckProgression( );

    // Autonomous activities ONLY when NOT in a player party or companion mode
    if ( ( !m_player->Party || !m_player->Party->party ) && !m_isCompanion )
    {
        // Phase 2: Autonomous party invitations check
        CheckPartyInvitations( );

        // Phase 3: Zone migration check
        CheckZoneMigration( );

        // Phase 4 Realism: LFG Shouts & World Boss Raids & Clan Recruitment Shouts
        CheckLfgShouts( );
        CheckWorldBossRaids( );
        CheckClanRecruitmentShout( );

        // Akram Arena PvP Battleground Participation
        CheckArenaQueue( );

        // Dungeon Crawling & Raids
        CheckDungeonRuns( );
    }

    // Player Greetings: Wave and greet nearby real players
    CheckPlayerGreetings( );

    // Autonomous Player Shop Browsing & Economy Purchases
    if ( !m_isCompanion )
    {
        CheckPlayerShops( );

        // Ambient Faction Wars in PvP Zones
        CMap* pvpMap = GetMap( );
        if ( pvpMap && ( pvpMap->allowpvp != 0 || pvpMap->pvp_mode != PVP_MODE_OFF ) )
        {
            if ( ( now - m_lastClanRivalryTime ) >= ( 30 * CLOCKS_PER_SEC ) )
            {
                for ( UINT i = 0; i < pvpMap->PlayerList.size( ); i++ )
                {
                    CPlayer* rival = pvpMap->PlayerList[i];
                    if ( rival && rival != m_player && rival->is_bot && rival->bot_ai && !rival->IsDead( ) )
                    {
                        if ( m_player->Clan && rival->Clan && m_player->Clan->clanid != 0 && rival->Clan->clanid != 0 && m_player->Clan->clanid != rival->Clan->clanid )
                        {
                            float d = GServer->distance( m_player->Position->current, rival->Position->current );
                            if ( d <= 25.0f )
                            {
                                m_lastClanRivalryTime = now;
                                char warCry[96];
                                snprintf( warCry, sizeof(warCry), "Faction Clash! For Clan %u, attack %s!",
                                    m_player->Clan->clanid, rival->CharInfo->charname );
                                Say( warCry );
                                StartDuel( rival );
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Private Message Whispers & Social Interaction
    CheckWhispers( );

    // Proximity buffing: buff nearby unbuffed stationary real players (without chasing/moving toward them)
    if ( !m_isCompanion && CheckProximityBuffs( ) ) return;

    // Party Lock-On & State Enforcement: If bot is in a party, lock onto party leader and follow!
    if ( m_player->Party && m_player->Party->party )
    {
        CPlayer* leader = GetPartyLeader( );
        if ( leader )
        {
            m_followTarget = leader;

            // Synchronized resting: loyally sit/stand with party leader
            if ( m_isCompanion && CheckSynchronizedRest( ) && leader->Status && leader->Status->Stance == 2 )
            {
                HandleRest( );
                return;
            }

            float distToLeader = ( m_player->Position->Map == leader->Position->Map ) ?
                GServer->distance( m_player->Position->current, leader->Position->current ) : 9999.0f;

            // Instant Tether Check: If leader is in another zone/map or ran far away, break all actions and follow!
            if ( distToLeader > 28.0f || m_state == BOT_STATE_IDLE || m_state == BOT_STATE_ROAM || m_state == BOT_STATE_TOWN_STROLL || m_state == BOT_STATE_SEEK_BUFF || m_state == BOT_STATE_REST )
            {
                if ( m_state == BOT_STATE_COMBAT || m_state == BOT_STATE_LOOT || m_state == BOT_STATE_REST )
                {
                    ClearBattle( m_player->Battle );
                    m_targetMobCid = 0;
                    if ( m_state == BOT_STATE_REST ) StandUp( );
                }
                SetState( BOT_STATE_FOLLOW );
            }
        }
    }

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
        case BOT_STATE_IDLE:        HandleIdle( );        break;
        case BOT_STATE_ROAM:        HandleRoam( );        break;
        case BOT_STATE_COMBAT:      HandleCombat( );      break;
        case BOT_STATE_LOOT:        HandleLoot( );        break;
        case BOT_STATE_REST:        HandleRest( );        break;
        case BOT_STATE_FOLLOW:      HandleFollow( );      break;
        case BOT_STATE_DEAD:        HandleDead( );        break;
        case BOT_STATE_BUFF_BOT:    HandleBuffBot( );     break;
        case BOT_STATE_SEEK_BUFF:   HandleSeekBuff( );    break;
        case BOT_STATE_VENDING:     HandleVendingBot( );  break;
        case BOT_STATE_TOWN_STROLL: HandleTownStroll( );  break;
        case BOT_STATE_MIGRATE:     HandleMigrate( );     break;
        case BOT_STATE_DUEL:        HandleDuel( );        break;
        case BOT_STATE_TOWN_REPAIR: HandleTownRepair( );  break;
        case BOT_STATE_LFG_SHOUT:   break;
        case BOT_STATE_RALLY_RAID:   HandleRallyRaid( );   break;
        case BOT_STATE_VENDOR_RUN:   HandleVendorRun( );   break;
        case BOT_STATE_QUEST_RETURN: HandleQuestReturn( );  break;
        case BOT_STATE_CLAN_HANGOUT: HandleClanHangout( );  break;
        case BOT_STATE_SIEGE_UNIT:   HandleSiegeUnit( );   break;
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
    CDrop* drop = FindNearbyDrop( 100.0f );
    if ( drop )
    {
        m_targetDropCid = drop->clientid;
        MoveTo( drop->pos );
        SetState( BOT_STATE_LOOT );
        return;
    }

    // 3. Look for nearby monsters to attack (expanded to 1200.0m map-wide detection so bots in town or fields spot distant monsters)
    CMonster* mob = FindNearbyMonster( 1200.0f );
    if ( mob )
    {
        AttackTarget( mob );
        SetState( BOT_STATE_COMBAT );
        return;
    }

    // 4. Roam if enabled and idle timer expired (> 1.5 seconds)
    if ( m_autoRoam )
    {
        clock_t elapsed = clock( ) - m_stateTimer;
        if ( elapsed >= (clock_t)( 1.5f * CLOCKS_PER_SEC ) )
        {
            float angle = ( (float)( rand( ) % 360 ) ) * ( 3.14159265f / 180.0f );
            float maxR = ( m_roamRadius > 50.0f ) ? m_roamRadius : 200.0f;
            float dist = 10.0f + ( (float)( rand( ) % (int)( maxR - 10.0f + 1.0f ) ) );
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
    CDrop* drop = FindNearbyDrop( 100.0f );
    if ( drop )
    {
        m_targetDropCid = drop->clientid;
        MoveTo( drop->pos );
        SetState( BOT_STATE_LOOT );
        return;
    }

    // Check for monsters while roaming (expanded to 1200.0m map-wide detection)
    CMonster* mob = FindNearbyMonster( 1200.0f );
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

    // 0. Tether Check: If party leader moved far away or changed maps, drop combat and rejoin leader immediately!
    if ( m_followTarget != NULL && m_followTarget->Position != NULL )
    {
        float distToLeader = ( m_player->Position->Map == m_followTarget->Position->Map ) ?
            GServer->distance( m_player->Position->current, m_followTarget->Position->current ) : 9999.0f;
        if ( distToLeader > 32.0f )
        {
            ClearBattle( m_player->Battle );
            m_targetMobCid = 0;
            SetState( BOT_STATE_FOLLOW );
            return;
        }
    }

    // Check party support during combat
    CheckPartyHeal( );
    CheckPartyResurrect( );
    CheckPartyBuffs( );
    CheckPartyTaunt( );

    CMonster* mob = map->GetMonsterInMap( m_targetMobCid );
    if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) )
    {
        if ( mob && mob->IsDead( ) )
        {
            CheckBountyProgress( mob );
            if ( m_isCompanion )
            {
                bool isEpic = ( mob->Stats->MaxHP >= 4000 || mob->montype >= 400 || ( m_followTarget && IsMonsterQuestTarget( mob, m_followTarget ) ) );
                AddAffinity( isEpic ? 5 : 1 );
                AddSynergy( isEpic ? 20 : 5 );

                // Milestone 12: Artisan Mobile Reagent Harvesting
                HarvestReagents( mob );

                // Milestone 12: Squad Victory Celebration Fanfare on Boss defeat
                if ( isEpic )
                {
                    ExecuteVictoryCelebration( );
                }

                // Milestone 11: Clan Contribution Sharing on Epic / Boss Kill
                if ( isEpic && m_followTarget && m_followTarget->Clan && m_followTarget->Clan->clanid != 0 )
                {
                    m_followTarget->GiveCP( 5 );
                    m_followTarget->Clan->money += 10000;
                    Say( "Epic enemy vanquished! Contributed +5 Clan Points and 10,000 Zulies to the guild hall!" );
                }
            }
            if ( m_isCompanion && m_followTarget && IsMonsterQuestTarget( mob, m_followTarget ) )
            {
                char progMsg[96];
                snprintf( progMsg, sizeof(progMsg), "Quest target down! Great teamwork, %s!",
                    m_followTarget->CharInfo->charname );
                Say( progMsg );
            }
        }
        ClearBattle( m_player->Battle );
        m_targetMobCid = 0;
        m_isPullingTarget = false;

        // In a party, respect leader's loot: only grab drops right under feet (< 3.0m)
        if ( m_followTarget )
        {
            CDrop* drop = FindNearbyDrop( 3.0f );
            if ( drop )
            {
                m_targetDropCid = drop->clientid;
                MoveTo( drop->pos );
                SetState( BOT_STATE_LOOT );
                return;
            }
            SetState( BOT_STATE_FOLLOW );
            return;
        }

        // Solo bot: check for loot within 50m
        CDrop* drop = FindNearbyDrop( 50.0f );
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

        SetState( BOT_STATE_IDLE );
        return;
    }

    // Pulling Stance / Action: If kiting target back to party anchor
    if ( m_isPullingTarget )
    {
        float distToAnchor = GServer->distance( m_player->Position->current, m_pullAnchorPos );
        if ( distToAnchor > 5.0f )
        {
            MoveTo( m_pullAnchorPos );
            return;
        }
        else
        {
            m_isPullingTarget = false;
            Say( "Target lured into range! Ambush now!" );
        }
    }

    // Minion Peeling: DPS peel off boss if adds attack healer/leader
    if ( CheckMinionPeel( ) ) return;

    // Milestone 11: Tactical Crowd Control on off-target adds
    if ( m_tactic != BOT_TACTIC_ECO && HandleOffTargetCrowdControl( mob ) ) return;

    // Boss Cleave Facing & AOE Dodging
    HandleBossCleaveAndTactics( mob );

    // Spell Channel Interrupt
    CheckSpellInterrupt( mob );

    // Target is valid: check attack range
    float dist = GServer->distance( m_player->Position->current, mob->Position->current );

    // Ranged kiting AI check
    KiteTarget( mob );

    // If target is too far away (> 450m), drop combat
    if ( dist > 450.0f )
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

    if ( m_followTarget != NULL && m_followTarget->Position != NULL )
    {
        float distToLeader = ( m_player->Position->Map == m_followTarget->Position->Map ) ?
            GServer->distance( m_player->Position->current, m_followTarget->Position->current ) : 9999.0f;
        if ( distToLeader > 28.0f )
        {
            m_targetDropCid = 0;
            SetState( BOT_STATE_FOLLOW );
            return;
        }
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

    if ( m_followTarget != NULL && m_followTarget->Position != NULL )
    {
        float distToLeader = ( m_player->Position->Map == m_followTarget->Position->Map ) ?
            GServer->distance( m_player->Position->current, m_followTarget->Position->current ) : 9999.0f;
        if ( distToLeader > 25.0f )
        {
            StandUp( );
            SetState( BOT_STATE_FOLLOW );
            return;
        }

        // If leader is resting/sitting, companion bot loyally stays sitting with leader!
        if ( m_isCompanion && m_followTarget->Status && m_followTarget->Status->Stance == 2 )
        {
            PrepareCampfireFeast( );
            return;
        }
    }

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

    if ( !m_player->Party || !m_player->Party->party )
    {
        m_followTarget = NULL;
        SetState( BOT_STATE_IDLE );
        return;
    }

    CPlayer* leader = GetPartyLeader( );
    if ( !leader )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }
    m_followTarget = leader;

    // Auto Level Sync: Keep companion bot level matched with party leader
    if ( m_followTarget && m_followTarget->Stats )
    {
        SyncLevelToLeader( m_followTarget->Stats->Level );
    }

    // 1. Teleport follow: If party leader changed maps or moved far away (> 25m), follow leader instantly
    if ( m_player->Position->Map != m_followTarget->Position->Map ||
         GServer->distance( m_player->Position->current, m_followTarget->Position->current ) > 25.0f )
    {
        CMap* targetMap = GServer->MapList.Index[m_followTarget->Position->Map];
        if ( targetMap != NULL && targetMap != GServer->MapList.nullzone )
        {
            fPoint targetPos = m_followTarget->Position->current;
            targetPos.x += ( ( rand() % 40 ) - 20 ) / 10.0f;
            targetPos.y += ( ( rand() % 40 ) - 20 ) / 10.0f;
            targetMap->TeleportPlayer( m_player, targetPos, false );

            // Explicitly spawn 3D entity on leader's screen & register in leader's VisiblePlayers
            // CPlayer::SpawnToPlayer( receiver, entity ) sends entity to receiver!
            m_player->RespawnToPlayer( m_followTarget );
            m_followTarget->RespawnToPlayer( m_player );

            bool foundV = false;
            for ( size_t v = 0; v < m_followTarget->VisiblePlayers.size( ); v++ )
            {
                if ( m_followTarget->VisiblePlayers[v] == m_player ) { foundV = true; break; }
            }
            if ( !foundV ) m_followTarget->VisiblePlayers.push_back( m_player );

            bool foundB = false;
            for ( size_t v = 0; v < m_player->VisiblePlayers.size( ); v++ )
            {
                if ( m_player->VisiblePlayers[v] == m_followTarget ) { foundB = true; break; }
            }
            if ( !foundB ) m_player->VisiblePlayers.push_back( m_followTarget );

            return;
        }
    }

    CMap* map = GetMap( );
    if ( !map )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }

    // Synchronized resting check
    if ( m_isCompanion && CheckSynchronizedRest( ) ) return;

    // Synchronized PAT Convoy Mount Driving
    if ( m_isCompanion && m_followTarget )
    {
        if ( m_followTarget->Status && m_followTarget->Status->Stance == DRIVING )
        {
            if ( m_player->Stats->Level >= 30 && m_player->Status->Stance != DRIVING && !m_player->IsOnBattle( ) )
            {
                if ( m_player->Stats->Level >= 70 )
                {
                    m_player->items[135].itemnum = 367; // Castle Gear Body
                    m_player->items[136].itemnum = 31;  // Engine
                    m_player->items[136].lifespan = 1000;
                    m_player->items[137].itemnum = 377; // Legs
                    m_player->items[138].itemnum = 387; // Arms
                }
                else
                {
                    m_player->items[135].itemnum = 2;   // Cart Body
                    m_player->items[136].itemnum = 35;  // Engine
                    m_player->items[136].lifespan = 1000;
                    m_player->items[137].itemnum = 68;  // Wheels
                    m_player->items[138].itemnum = 0;
                }

                m_player->Status->Stance = DRIVING;
                BEGINPACKET( pakStance, 0x0782 );
                ADDWORD    ( pakStance, m_player->clientid );
                ADDBYTE    ( pakStance, 0x04 ); // DRIVING
                ADDWORD    ( pakStance, m_player->Stats->Base_Speed );
                GServer->SendToVisible( &pakStance, m_player );

                clock_t nowTime = clock( );
                if ( ( nowTime - m_lastConvoyMountTime ) >= ( 60 * CLOCKS_PER_SEC ) )
                {
                    m_lastConvoyMountTime = nowTime;
                    Say( "Mounting up with the leader! Convoy rolling out!" );
                }
            }
        }
        else if ( m_player->Status && m_player->Status->Stance == DRIVING )
        {
            m_player->Status->Stance = RUNNING;
            BEGINPACKET( pakStance, 0x0782 );
            ADDWORD    ( pakStance, m_player->clientid );
            ADDBYTE    ( pakStance, 0x03 ); // RUNNING
            ADDWORD    ( pakStance, m_player->Stats->Base_Speed );
            GServer->SendToVisible( &pakStance, m_player );
        }
    }

    // Check squad target focus (/party bot mark)
    if ( m_markedTargetCid != 0 && ( m_stance != BOT_STANCE_PASSIVE ) )
    {
        CMonster* markedMob = map->GetMonsterInMap( m_markedTargetCid );
        if ( markedMob && !markedMob->IsDead( ) && markedMob->Stats->HP > 0 && !markedMob->IsBonfire( ) )
        {
            AttackTarget( markedMob );
            SetState( BOT_STATE_COMBAT );
            return;
        }
        else
        {
            m_markedTargetCid = 0;
        }
    }

    // Periodic Quest Mob Scanner & Callouts
    clock_t now = clock( );
    if ( m_isCompanion && m_followTarget && ( now - m_lastQuestScanTime ) > (clock_t)( 25 * CLOCKS_PER_SEC ) )
    {
        m_lastQuestScanTime = now;
        CMonster* questMob = FindNearbyQuestMonster( 25.0f );
        if ( questMob )
        {
            char questCallout[96];
            snprintf( questCallout, sizeof(questCallout),
                "Spotted a quest target nearby! Ready when you are, %s!",
                m_followTarget->CharInfo->charname );
            Say( questCallout );
        }
    }

    // Milestone 11 & 12: Logistics, clan synchronization & union awareness
    CheckAmmunitionResupply( );
    CheckClanSync( );
    CheckUnionSync( );
    CheckPuzzleInteraction( );

    // 2. Party Member Support: Heals, Resurrections, Buffs (Always active)
    if ( CheckPartyHeal( ) ) return;
    if ( CheckPartyResurrect( ) ) return;
    if ( CheckPartyBuffs( ) ) return;

    // Stance Routing:
    if ( m_stance == BOT_STANCE_PASSIVE )
    {
        // Passive stance: Pacifist / Hold Fire mode. Strictly follows formation without engaging.
    }
    else if ( m_stance == BOT_STANCE_ASSIST )
    {
        // Assist stance: Focus fire strictly on the leader's active target
        if ( m_followTarget->IsOnBattle( ) && m_followTarget->Battle->target != 0 )
        {
            CMonster* leaderMob = map->GetMonsterInMap( m_followTarget->Battle->target );
            if ( leaderMob && !leaderMob->IsDead( ) && leaderMob->Stats->HP > 0 && !leaderMob->IsBonfire( ) )
            {
                AttackTarget( leaderMob );
                SetState( BOT_STATE_COMBAT );
                return;
            }
        }
    }
    else
    {
        // Defensive (Bodyguard) or Aggressive (Hunter) stance
        if ( CheckPartyTaunt( ) ) return;

        // 3. Assist Party Leader in Combat: check if leader is attacking a monster
        if ( m_followTarget->IsOnBattle( ) && m_followTarget->Battle->target != 0 )
        {
            CMonster* leaderMob = map->GetMonsterInMap( m_followTarget->Battle->target );
            if ( leaderMob && !leaderMob->IsDead( ) && leaderMob->Stats->HP > 0 && !leaderMob->IsBonfire( ) )
            {
                AttackTarget( leaderMob );
                SetState( BOT_STATE_COMBAT );
                return;
            }
        }

        // 4. Defend Party Members: check if any monster is attacking leader or any party member within 60m
        for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
        {
            CMonster* mob = map->MonsterList[i];
            if ( mob && !mob->IsDead( ) && mob->Stats->HP > 0 && !mob->IsBonfire( ) )
            {
                float distToLeader = GServer->distance( m_followTarget->Position->current, mob->Position->current );
                if ( distToLeader <= 60.0f && mob->Battle && mob->Battle->target != 0 )
                {
                    for ( size_t m = 0; m < m_player->Party->party->Members.size( ); m++ )
                    {
                        CPlayer* pm = m_player->Party->party->Members[m];
                        if ( pm && mob->Battle->target == pm->clientid )
                        {
                            AttackTarget( mob );
                            SetState( BOT_STATE_COMBAT );
                            return;
                        }
                    }
                }
            }
        }

        // Aggressive Stance (Hunter): Proactively pulls quest monsters or nearby mobs if party is idle!
        if ( m_stance == BOT_STANCE_AGGRESSIVE && m_isCompanion )
        {
            if ( m_role == BOT_ROLE_TANK || m_role == BOT_ROLE_DPS_MAGIC || m_role == BOT_ROLE_DPS_PHYS_RANGED || m_role == BOT_ROLE_DPS_PHYS_MELEE )
            {
                CMonster* questMob = FindNearbyQuestMonster( 22.0f );
                CMonster* pullMob = questMob ? questMob : FindNearbyMonster( 18.0f );
                if ( pullMob && !pullMob->IsDead( ) && pullMob->Stats->HP > 0 && !pullMob->IsBonfire( ) )
                {
                    float distToLeader = GServer->distance( m_followTarget->Position->current, pullMob->Position->current );
                    if ( distToLeader <= 22.0f )
                    {
                        AttackTarget( pullMob );
                        SetState( BOT_STATE_COMBAT );
                        if ( questMob )
                        {
                            char pullMsg[96];
                            snprintf( pullMsg, sizeof(pullMsg), "Engaging quest target, %s!", m_followTarget->CharInfo->charname );
                            Say( pullMsg );
                        }
                        else if ( rand( ) % 3 == 0 )
                        {
                            Say( "Aggressive stance: pulling a monster for the group to grind!" );
                        }
                        return;
                    }
                }
            }
        }
    }

    // 5. Tactical Formation Following
    int slot = GetPartySlotIndex( );
    fPoint destPos = GetFormationOffset( slot, m_followTarget->Position->current, m_followTarget->Position->destiny );

    // Flocking Separation: Prevent bots from stacking/bunching on top of each other or the leader
    if ( m_player->Party && m_player->Party->party )
    {
        CParty* party = m_player->Party->party;
        for ( size_t m = 0; m < party->Members.size( ); m++ )
        {
            CPlayer* otherMem = party->Members[m];
            if ( otherMem && otherMem != m_player && otherMem->Position && otherMem->Position->Map == m_player->Position->Map )
            {
                float sepDist = GServer->distance( destPos, otherMem->Position->current );
                if ( sepDist < 1.8f && sepDist > 0.05f )
                {
                    float pushX = ( destPos.x - otherMem->Position->current.x ) / sepDist;
                    float pushY = ( destPos.y - otherMem->Position->current.y ) / sepDist;
                    destPos.x += pushX * ( 1.8f - sepDist );
                    destPos.y += pushY * ( 1.8f - sepDist );
                }
            }
        }
    }

    float dist = GServer->distance( m_player->Position->current, destPos );
    if ( dist > 35.0f )
    {
        // Leader moved far away or teleported: snap to leader's side
        map->TeleportPlayer( m_player, destPos, false );
    }
    else if ( dist > 2.2f )
    {
        MoveTo( destPos );
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
    if ( ( now - m_stateTimer ) >= ( 10 * CLOCKS_PER_SEC ) )
    {
        Respawn( );
    }
}

void CPlayerBot::HandleTownStroll( )
{
    if ( !m_player ) return;

    // Check for monsters outside town gates (expanded to 1200.0m map-wide detection)
    CMonster* mob = FindNearbyMonster( 1200.0f );
    if ( mob )
    {
        AttackTarget( mob );
        SetState( BOT_STATE_COMBAT );
        return;
    }

    if ( !m_player->IsMoving( ) )
    {
        clock_t elapsed = clock( ) - m_stateTimer;
        if ( elapsed >= (clock_t)( 8 * CLOCKS_PER_SEC ) )
        {
            float angle = ( (float)( rand( ) % 360 ) ) * ( 3.14159265f / 180.0f );
            float dist = 4.0f + ( (float)( rand( ) % 12 ) );
            fPoint dest;
            dest.x = m_roamCenter.x + cosf( angle ) * dist;
            dest.y = m_roamCenter.y + sinf( angle ) * dist;
            dest.z = 0;
            MoveTo( dest );

            int routineRoll = rand( ) % 6;
            if ( routineRoll == 0 )
            {
                Say( "Browsing the merchant stalls... any rare items for sale?" );
                DoEmote( 2 ); // Nod
            }
            else if ( routineRoll == 1 )
            {
                Say( "Heading over toward the forge to check weapon durability." );
                DoEmote( 1 ); // Wave
            }
            else if ( routineRoll == 2 )
            {
                Say( "Town is bustling today! Good time to rest between expeditions." );
                DoEmote( 3 ); // Cheer
            }
            else if ( rand( ) % 2 == 0 )
            {
                BYTE emotes[] = { 1, 2, 3, 4, 6 };
                DoEmote( emotes[rand( ) % 5] );
            }
            if ( rand( ) % 4 == 0 )
            {
                SayChatter( "greeting" );
            }
            m_stateTimer = clock( );
        }
        else if ( elapsed >= (clock_t)( 20 * CLOCKS_PER_SEC ) )
        {
            SetState( BOT_STATE_IDLE );
        }
    }
}

void CPlayerBot::HandleMigrate( )
{
    SetState( BOT_STATE_IDLE );
}

void CPlayerBot::HandleTownRepair( )
{
    if ( !m_player ) return;
    clock_t now = clock( );

    if ( m_stateTimer == 0 || ( now - m_stateTimer ) > ( 25 * CLOCKS_PER_SEC ) )
    {
        if ( RollD20( ) >= 10 )
        {
            Say( "Gear repaired and inventory cleared! Heading back to hunt." );
        }
        SetState( BOT_STATE_ROAM );
        return;
    }

    if ( ( ( now - m_stateTimer ) / CLOCKS_PER_SEC ) == 3 )
    {
        if ( RollD20( ) >= 12 )
        {
            Say( "Visiting NPC Blacksmith to repair equipment and sell loot." );
            DoEmote( 2 ); // Nod / Cheer
        }
    }
}

void CPlayerBot::CheckLfgShouts( )
{
    if ( !m_player || m_isBuffBot || m_isVendingBot ) return;
    clock_t now = clock( );
    if ( ( now - m_lastLfgShoutTime ) < (clock_t)( ( 240 + ( rand( ) % 360 ) ) * CLOCKS_PER_SEC ) ) return;
    m_lastLfgShoutTime = now;

    // 15% chance to shout when timer expires
    if ( rand( ) % 100 >= 15 ) return;

    if ( m_player->Party->party != NULL && m_player->Party->party->Members.size() >= 4 ) return;

    int lvl = m_player->Stats->Level;
    int job = m_player->CharInfo->Job;

    const char* jobStr = "Visitor";
    switch(job) {
        case 111: jobStr = "Soldier"; break;
        case 121: jobStr = "Knight"; break;
        case 122: jobStr = "Champion"; break;
        case 211: jobStr = "Muse"; break;
        case 221: jobStr = "Mage"; break;
        case 222: jobStr = "Cleric"; break;
        case 311: jobStr = "Hawker"; break;
        case 321: jobStr = "Raider"; break;
        case 322: jobStr = "Scout"; break;
        case 411: jobStr = "Dealer"; break;
        case 421: jobStr = "Bourgeois"; break;
        case 422: jobStr = "Artisan"; break;
    }

    char msg[128];
    if ( m_player->Party->party == NULL )
    {
        if ( lvl >= 60 )
            snprintf( msg, sizeof(msg), "LFG Barka Dungeon - Level %d %s ready! PST!", lvl, jobStr );
        else if ( lvl >= 25 )
            snprintf( msg, sizeof(msg), "LFG Goblin Cave / Breezy Hills - Level %d %s!", lvl, jobStr );
        else
            snprintf( msg, sizeof(msg), "LFG Adventure Plains! Level %d %s looking for party!", lvl, jobStr );
    }
    else
    {
        snprintf( msg, sizeof(msg), "LF%dM Level %d+ Grind Party! PST or target invite!",
                  4 - (int)m_player->Party->party->Members.size(), lvl - 5 );
    }

    BEGINPACKET( pak, 0x0785 );
    ADDSTRING  ( pak, m_player->CharInfo->charname );
    ADDBYTE    ( pak, 0 );
    ADDSTRING  ( pak, msg );
    ADDBYTE    ( pak, 0 );
    GServer->SendToMap( &pak, m_player->Position->Map );
}

void CPlayerBot::CheckWorldBossRaids( )
{
    if ( !m_player || m_isBuffBot || m_isVendingBot || m_state == BOT_STATE_DEAD ) return;
    clock_t now = clock( );
    if ( ( now - m_lastBossRaidCheckTime ) < ( 25 * CLOCKS_PER_SEC ) ) return;
    m_lastBossRaidCheckTime = now;

    CMonster* boss = FindNearbyWorldBoss( 200.0f );
    if ( boss )
    {
        if ( m_state != BOT_STATE_COMBAT )
        {
            char msg[128];
            snprintf( msg, sizeof(msg), "WORLD BOSS ALERT: Boss spotted near (%.0f, %.0f)! Assemble raid!",
                      boss->Position->current.x, boss->Position->current.y );

            BEGINPACKET( pak, 0x0785 );
            ADDSTRING  ( pak, m_player->CharInfo->charname );
            ADDBYTE    ( pak, 0 );
            ADDSTRING  ( pak, msg );
            ADDBYTE    ( pak, 0 );
            GServer->SendToMap( &pak, m_player->Position->Map );

            BroadcastRaidRally( boss );
            AttackTarget( boss );
            SetState( BOT_STATE_COMBAT );
        }
    }
}

void CPlayerBot::ProcessIncomingWhisper( CPlayer* sender, const char* msg )
{
    if ( !m_player || !sender || !msg || m_isBuffBot || m_isVendingBot ) return;

    std::string text = msg;
    for ( size_t i = 0; i < text.length(); i++ ) text[i] = tolower( text[i] );

    char reply[256];
    reply[0] = '\0';

    if ( text.find( "where" ) != std::string::npos || text.find( "location" ) != std::string::npos || text.find( "pos" ) != std::string::npos )
    {
        snprintf( reply, sizeof(reply), "I'm currently at (%.0f, %.0f) on map %d! Feel free to join me.",
                  m_player->Position->current.x, m_player->Position->current.y, m_player->Position->Map );
    }
    else if ( text.find( "party" ) != std::string::npos || text.find( "invite" ) != std::string::npos || text.find( "team" ) != std::string::npos )
    {
        if ( m_isBuffBot || m_isVendingBot )
        {
            snprintf( reply, sizeof(reply), "Sorry, static service bots cannot join parties." );
        }
        else if ( m_player->Party->party != NULL && m_player->Party->party != sender->Party->party )
        {
            snprintf( reply, sizeof(reply), "Sorry, I'm already in a party right now!" );
        }
        else
        {
            CParty* party = sender->Party->party;
            bool isNewParty = ( party == NULL );
            if ( party == NULL )
            {
                party = new CParty;
                party->AddPlayer( sender );
                GServer->AddParty( party );
                sender->Party->IsMaster = true;
            }

            if ( m_player->Party->party == NULL )
            {
                // Send accepted packet 0x7d1 to inviter
                BEGINPACKET( pak, 0x7d1 );
                ADDBYTE    ( pak, 0x02 );
                ADDWORD    ( pak, m_player->clientid );
                ADDBYTE    ( pak, 0x00 );
                if ( sender->client ) sender->client->SendPacket( &pak );

                // Send party level and exp
                RESETPACKET( pak, 0x7d4 );
                ADDBYTE    ( pak, party->PartyLevel );
                ADDDWORD   ( pak, party->Exp );
                if ( sender->client ) sender->client->SendPacket( &pak );

                m_player->Party->IsMaster = false;
                party->AddPlayer( m_player );

                // Send Party Member List (0x7d2) to party members
                RESETPACKET( pak, 0x7d2 );
                ADDBYTE    ( pak, party->Option );
                ADDBYTE    ( pak, 0x01 );
                ADDDWORD   ( pak, m_player->CharInfo->charid );
                ADDWORD    ( pak, m_player->clientid );
                ADDWORD    ( pak, (WORD)m_player->Stats->MaxHP );
                ADDWORD    ( pak, (WORD)m_player->Stats->HP );
                ADDDWORD   ( pak, GServer->BuildBuffs( m_player ) );
                ADDDWORD   ( pak, 0x1f40008c );
                ADDWORD    ( pak, 0x1388 );
                ADDSTRING  ( pak, m_player->CharInfo->charname );
                ADDBYTE    ( pak, 0x00 );
                party->SendToMembers( &pak );

                // Teleport bot to player's side
                if ( sender->Position != NULL && GServer->MapList.Index[sender->Position->Map] != NULL )
                {
                    CMap* pMap = GServer->MapList.Index[sender->Position->Map];
                    fPoint spawnCoord = sender->Position->current;
                    spawnCoord.x += ( ( rand() % 40 ) - 20 ) / 10.0f;
                    spawnCoord.y += ( ( rand() % 40 ) - 20 ) / 10.0f;

                    if ( m_player->Position->Map != sender->Position->Map )
                    {
                        pMap->TeleportPlayer( m_player, spawnCoord, false );
                    }
                    else
                    {
                        m_player->Position->current = spawnCoord;
                        m_player->Position->destiny = spawnCoord;
                    }

                    // Explicitly spawn 3D entity on leader's screen & add to VisiblePlayers
                    m_player->RespawnToPlayer( sender );
                    sender->RespawnToPlayer( m_player );

                    bool foundV = false;
                    for ( size_t v = 0; v < sender->VisiblePlayers.size( ); v++ )
                    {
                        if ( sender->VisiblePlayers[v] == m_player ) { foundV = true; break; }
                    }
                    if ( !foundV ) sender->VisiblePlayers.push_back( m_player );

                    bool foundB = false;
                    for ( size_t v = 0; v < m_player->VisiblePlayers.size( ); v++ )
                    {
                        if ( m_player->VisiblePlayers[v] == sender ) { foundB = true; break; }
                    }
                    if ( !foundB ) m_player->VisiblePlayers.push_back( sender );
                }

                SetFollowTarget( sender );
                SetState( BOT_STATE_FOLLOW );

                snprintf( reply, sizeof(reply), "I've joined your party! I'll follow you and fight by your side!" );
            }
            else
            {
                snprintf( reply, sizeof(reply), "I'm already in your party! Let me know if you need buffs or combat support." );
            }
        }
    }
    else if ( text.find( "join" ) != std::string::npos || text.find( "clan" ) != std::string::npos || text.find( "guild" ) != std::string::npos )
    {
        HandleClanInviteRequest( sender );
        return;
    }
    else if ( text.find( "trade" ) != std::string::npos )
    {
        HandleTradeRequest( sender );
        return;
    }
    else if ( text.find( "duel" ) != std::string::npos || text.find( "fight" ) != std::string::npos || text.find( "pvp" ) != std::string::npos )
    {
        snprintf( reply, sizeof(reply), "You want to fight? Type '/duel %s' and let's go!", m_player->CharInfo->charname );
    }
    else if ( text.find( "discount" ) != std::string::npos || text.find( "cheaper" ) != std::string::npos ||
              text.find( "haggle" ) != std::string::npos || text.find( "lower" ) != std::string::npos ||
              text.find( "price" ) != std::string::npos || text.find( "buy" ) != std::string::npos )
    {
        clock_t now = clock( );
        if ( ( now - m_lastHaggleTime ) >= ( 10 * CLOCKS_PER_SEC ) )
        {
            m_lastHaggleTime = now;
            int chaMod = ( m_player && m_player->Attr ) ? ( m_player->Attr->Cha / 10 ) : 0;
            int roll = RollD20( ) + chaMod;
            if ( roll >= 14 )
            {
                const char* successMsgs[] = {
                    "Alright %s, deal! Since you asked nicely, I can offer a 10%% discount!",
                    "Sure %s! I'll lower the price by 10%% for you.",
                    "Deal %s! 5%% to 10%% off just for you."
                };
                snprintf( reply, sizeof(reply), successMsgs[rand() % 3], sender->CharInfo->charname );
            }
            else
            {
                const char* failMsgs[] = {
                    "Sorry %s, prices are firm right now!",
                    "Can't go any lower, %s! Market prices are tough.",
                    "Haha, nice try %s! Price stays as listed."
                };
                snprintf( reply, sizeof(reply), failMsgs[rand() % 3], sender->CharInfo->charname );
            }
        }
    }
    else if ( text.find( "hi" ) != std::string::npos || text.find( "hello" ) != std::string::npos || text.find( "hey" ) != std::string::npos )
    {
        snprintf( reply, sizeof(reply), "Hey %s! Good to see you out here. Good luck grinding!", sender->CharInfo->charname );
    }
    else
    {
        const char* defaultReplies[] = {
            "Sounds good! Let's keep pushing ahead.",
            "Haha nice! See you around!",
            "Good luck with your quest!"
        };
        snprintf( reply, sizeof(reply), "%s", defaultReplies[rand() % 3] );
    }

    if ( reply[0] != '\0' )
    {
        WhisperPlayer( sender, reply );
    }
}

void CPlayerBot::TrackPvpResult( UINT playerCharId, bool won )
{
    if ( won )
        m_pvpRecord[playerCharId]++;
    else
        m_pvpRecord[playerCharId]--;
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

    // Collect candidates within 16 meters: both real players AND player bots needing buffs
    std::vector<CPlayer*> candidates;

    for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( !p || p == m_player || p->IsDead( ) ) continue;

        if ( p->bot_ai != NULL )
        {
            CPlayerBot* bAi = reinterpret_cast<CPlayerBot*>( p->bot_ai );
            if ( bAi && bAi->IsBuffBot( ) ) continue; // Don't buff other buff bots
        }
        else
        {
            if ( p->Session == NULL || !p->Session->inGame ) continue;
        }

        float dist = GServer->distance( m_player->Position->current, p->Position->current );
        if ( dist <= 16.0f )
        {
            bool needsHelp = false;
            if ( p->Stats->HP < ( p->Stats->MaxHP * 85 / 100 ) )
            {
                needsHelp = true;
            }
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
                candidates.push_back( p );
            }
        }
    }

    if ( candidates.empty( ) ) return;

    // Fair Round-Robin Queue: cycle through candidate list so every player & bot gets buffed equally
    if ( m_nextBuffCandidateIndex >= candidates.size( ) )
    {
        m_nextBuffCandidateIndex = 0;
    }

    CPlayer* bestTarget = candidates[m_nextBuffCandidateIndex];
    m_nextBuffCandidateIndex = ( m_nextBuffCandidateIndex + 1 ) % candidates.size( );
    bool targetIsPlayer = ( bestTarget->bot_ai == NULL );

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

void CPlayerBot::SetupVendingShop( const char* shopTitle, int category )
{
    if ( !m_player || !m_player->Shop ) return;

    m_vendingCategory = category;
    m_shopTitle = shopTitle ? shopTitle : "Shop";

    strncpy( m_player->Shop->name, shopTitle ? shopTitle : "Shop", sizeof(m_player->Shop->name) - 1 );
    m_player->Shop->name[sizeof(m_player->Shop->name) - 1] = '\0';
    m_player->Shop->ShopType = 0; // Standard shop

    PopulateVendingInventory( m_player, category );
    PopulateVendingBuyList( m_player, category );
    m_lastBuyListRefreshTime = clock( );

    m_player->Status->Stance = 1; // Sitting stance

    // Broadcast shop opening to nearby players
    BEGINPACKET( pak, 0x796 );
    ADDWORD    ( pak, m_player->clientid );
    ADDFLOAT   ( pak, m_player->Position->current.x );
    ADDFLOAT   ( pak, m_player->Position->current.y );
    ADDWORD    ( pak, 0x9057 );
    GServer->SendToVisible( &pak, m_player );

    RESETPACKET( pak, 0x7c2 );
    ADDWORD    ( pak, m_player->clientid );
    ADDWORD    ( pak, m_player->Shop->ShopType );
    ADDSTRING  ( pak, m_player->Shop->name );
    ADDBYTE    ( pak, 0x00 );
    GServer->SendToVisible( &pak, m_player );

    m_lastVendingSay = clock( );
}

void CPlayerBot::HandleVendingBot( )
{
    if ( !m_player || !m_player->Shop ) return;

    if ( m_player->IsDead( ) || m_player->Stats->HP <= 0 )
    {
        Respawn( );
        return;
    }

    // Keep shop active, max HP/MP, and shop sitting stance maintained
    m_player->Stats->HP = m_player->Stats->MaxHP;
    m_player->Stats->MP = m_player->Stats->MaxMP;
    m_player->Shop->open = true;
    m_player->Status->Stance = 1;

    // Periodic promotional shout
    clock_t now = clock( );
    if ( ( now - m_lastVendingSay ) > ( 60 * CLOCKS_PER_SEC ) )
    {
        m_lastVendingSay = now;
        const char* promo = GetVendingPromoMessage( m_vendingCategory );
        if ( promo && strlen( promo ) > 0 )
        {
            Say( promo );
        }
    }

    // Periodic Buy List Refresh & Rotation (Every 2 Hours = 7200 seconds)
    if ( m_lastBuyListRefreshTime == 0 || ( now - m_lastBuyListRefreshTime ) > ( (clock_t)7200 * CLOCKS_PER_SEC ) )
    {
        m_lastBuyListRefreshTime = now;
        PopulateVendingBuyList( m_player, m_vendingCategory );

        BEGINPACKET( pak, 0x7c2 );
        ADDWORD    ( pak, m_player->clientid );
        ADDWORD    ( pak, m_player->Shop->ShopType );
        ADDSTRING  ( pak, m_player->Shop->name );
        ADDBYTE    ( pak, 0x00 );
        GServer->SendToVisible( &pak, m_player );

        Log( MSG_INFO, "Vending Bot '%s' refreshed & randomized Buy List (Category %d)",
             m_player->CharInfo->charname, m_vendingCategory );
    }
}

void CPlayerBot::AssignClanTag( )
{
    if ( !m_player || !m_player->Clan ) return;
    if ( m_player->Clan->clanid == 0 )
    {
        m_player->Clan->clanid = 1 + ( rand( ) % 5 );
        m_player->Clan->clanrank = 2 + ( rand( ) % 4 );
    }
}

void CPlayerBot::CheckPlayerGreetings( )
{
    if ( !m_player || m_isVendingBot || m_state == BOT_STATE_DEAD || m_state == BOT_STATE_DUEL ) return;

    clock_t now = clock( );
    if ( ( now - m_lastGreetingTime ) < (clock_t)( 45 * CLOCKS_PER_SEC ) ) return;

    // Roll d20: 15 or higher required to greet a nearby player (~30% chance)
    if ( RollD20( ) < 15 ) return;

    CMap* map = GetMap( );
    if ( !map ) return;

    for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( p && !p->is_bot && p->Session && p->Session->inGame && p->Position )
        {
            float dist = GServer->distance( m_player->Position->current, p->Position->current );
            if ( dist <= 8.0f )
            {
                m_lastGreetingTime = now + ( ( rand( ) % 20 ) * CLOCKS_PER_SEC );
                DoEmote( 1 ); // Wave

                // 50% chance to also chat out loud on greeting
                if ( RollD20( ) >= 10 )
                {
                    char msg[80];
                    const char* greetings[] = {
                        "Hey there, %s!",
                        "Good luck hunting out here, %s!",
                        "Nice gear you got there, %s!",
                        "Stay safe out there, %s!"
                    };
                    snprintf( msg, sizeof(msg), greetings[rand() % 4], p->CharInfo->charname );
                    Say( msg );
                }
                return;
            }
        }
    }
}

void CPlayerBot::StartDuel( CPlayer* challenger )
{
    if ( !m_player || !challenger || m_state == BOT_STATE_DEAD ) return;

    m_duelTargetCid = challenger->clientid;
    m_duelStartTime = clock( );

    char msg[80];
    snprintf( msg, sizeof(msg), "Challenge accepted, %s! Prepare yourself!", challenger->CharInfo->charname );
    Say( msg );
    DoEmote( 2 ); // Joy/Cheer

    SetState( BOT_STATE_DUEL );
}

void CPlayerBot::HandleDuel( )
{
    if ( !m_player ) return;

    CMap* map = GetMap( );
    if ( !map )
    {
        SetState( BOT_STATE_IDLE );
        return;
    }

    CPlayer* challenger = map->GetPlayerInMap( m_duelTargetCid );
    if ( !challenger || challenger->IsDead( ) || challenger->Stats->HP <= ( challenger->Stats->MaxHP * 10 / 100 ) )
    {
        if ( challenger && challenger->Stats->HP <= ( challenger->Stats->MaxHP * 10 / 100 ) )
        {
            Say( "Good fight! You fought well." );
            DoEmote( 6 );
        }
        m_duelTargetCid = 0;
        ClearBattle( m_player->Battle );
        SetState( BOT_STATE_IDLE );
        return;
    }

    if ( m_player->Stats->HP <= ( m_player->Stats->MaxHP * 10 / 100 ) )
    {
        Say( "Yield! You win this time, nice duel!" );
        DoEmote( 6 );
        m_duelTargetCid = 0;
        ClearBattle( m_player->Battle );
        SetState( BOT_STATE_REST );
        return;
    }

    clock_t now = clock( );
    if ( ( now - m_duelStartTime ) > (clock_t)( 120 * CLOCKS_PER_SEC ) )
    {
        Say( "Time's up! Let me know if you want to duel again." );
        m_duelTargetCid = 0;
        ClearBattle( m_player->Battle );
        SetState( BOT_STATE_IDLE );
        return;
    }

    float dist = GServer->distance( m_player->Position->current, challenger->Position->current );
    if ( dist > 30.0f )
    {
        MoveTo( challenger->Position->current );
        return;
    }

    KiteTarget( challenger );

    if ( !m_player->IsOnBattle( ) || m_player->Battle->target != challenger->clientid )
    {
        AttackTarget( challenger );
        return;
    }

    if ( m_player->IsTargetReached( challenger ) )
    {
        CastCombatSkill( challenger );
    }
}

void CPlayerBot::AssignTitle( )
{
    if ( !m_player ) return;
    int lvl = m_player->Stats->Level;

    if ( lvl >= 100 ) m_title = "<Apex Legend>";
    else if ( lvl >= 80 ) m_title = "<Hero of Junon>";
    else if ( lvl >= 60 ) m_title = "<Master Crafter>";
    else if ( lvl >= 40 ) m_title = "<Shadow Raider>";
    else m_title = "<Novice Slayer>";
}

void CPlayerBot::WhisperPlayer( CPlayer* target, const char* msg )
{
    if ( !m_player || !target || !target->client || !msg ) return;

    BEGINPACKET( pak, 0x784 );
    ADDSTRING  ( pak, m_player->CharInfo->charname );
    ADDSTRING  ( pak, (char*)msg );
    ADDBYTE    ( pak, 0 );
    target->client->SendPacket( &pak );
}

void CPlayerBot::CheckPlayerShops( )
{
    if ( !m_player || m_isVendingBot || m_state == BOT_STATE_DEAD || m_state == BOT_STATE_DUEL ) return;

    clock_t now = clock( );
    if ( ( now - m_lastShopBrowseTime ) < (clock_t)( 40 * CLOCKS_PER_SEC ) ) return;

    CMap* map = GetMap( );
    if ( !map ) return;

    for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( p && !p->is_bot && p->Shop && p->Shop->open && p->Position )
        {
            float dist = GServer->distance( m_player->Position->current, p->Position->current );
            if ( dist <= 12.0f )
            {
                m_lastShopBrowseTime = now;
                MoveTo( p->Position->current );

                if ( dist <= 3.5f )
                {
                    for ( int idx = 0; idx < 30; idx++ )
                    {
                        if ( p->Shop->SellingList[idx].count > 0 && p->Shop->SellingList[idx].price > 0 )
                        {
                            UINT slot = p->Shop->SellingList[idx].slot;
                            UINT price = (UINT)p->Shop->SellingList[idx].price;
                            if ( slot < 140 && p->items[slot].itemnum != 0 && m_player->CharInfo->Zulies >= price )
                            {
                                m_player->CharInfo->Zulies -= price;
                                p->CharInfo->Zulies += price;

                                CItem boughtItem = p->items[slot];
                                p->Shop->SellingList[idx].count--;
                                if ( p->Shop->SellingList[idx].count == 0 )
                                {
                                    p->items[slot].itemnum = 0;
                                    p->items[slot].count = 0;
                                }

                                m_player->AddItem( boughtItem );

                                char whisperMsg[90];
                                snprintf( whisperMsg, sizeof(whisperMsg), "Thanks for the deal on your shop items!" );
                                WhisperPlayer( p, whisperMsg );

                                Say( "Bought some great supplies from your shop!" );
                                DoEmote( 2 ); // Cheer
                                return;
                            }
                        }
                    }
                }
                return;
            }
        }
    }
}

void CPlayerBot::CheckWhispers( )
{
    if ( !m_player || m_isVendingBot || m_state == BOT_STATE_DEAD ) return;

    clock_t now = clock( );
    if ( ( now - m_lastWhisperTime ) < (clock_t)( ( 300 + ( rand( ) % 300 ) ) * CLOCKS_PER_SEC ) ) return;
    m_lastWhisperTime = now;

    // 10% chance to whisper when cooldown expires
    if ( rand( ) % 100 >= 10 ) return;

    CMap* map = GetMap( );
    if ( !map || !m_player->Position ) return;

    for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( p && !p->is_bot && p->Session && p->Session->inGame && p->Position )
        {
            // Only whisper if player is nearby (within 25.0f units)
            float dist = GServer->distance( m_player->Position->current, p->Position->current );
            if ( dist > 25.0f ) continue;

            char msg[100];
            if ( m_personality == BOT_PERSONALITY_RIVAL )
            {
                snprintf( msg, sizeof(msg), "Hey %s! I'm level %d now. Trying to stay ahead of you!", p->CharInfo->charname, m_player->Stats->Level );
            }
            else if ( m_personality == BOT_PERSONALITY_HELPER )
            {
                snprintf( msg, sizeof(msg), "Greetings %s! Let me know if you need party buffs or support!", p->CharInfo->charname );
            }
            else
            {
                snprintf( msg, sizeof(msg), "Good luck hunting out here, %s!", p->CharInfo->charname );
            }
            WhisperPlayer( p, msg );
            return;
        }
    }
}

void CPlayerBot::CheckArenaQueue( )
{
    if ( !m_player || m_player->Stats->Level < 30 || m_state == BOT_STATE_DEAD || m_state == BOT_STATE_DUEL ) return;

    clock_t now = clock( );
    if ( ( now - m_lastArenaQueueTime ) < (clock_t)( 60 * CLOCKS_PER_SEC ) ) return;
    m_lastArenaQueueTime = now;

    eArenaState state = CArenaManager::GetInstance()->GetState( );
    if ( state == ARENA_STATE_COUNTDOWN )
    {
        CArenaManager::GetInstance()->JoinArena( m_player );
    }
}

void CPlayerBot::CheckDungeonRuns( )
{
    if ( !m_player || m_player->Stats->Level < 60 || m_state == BOT_STATE_DEAD || m_state == BOT_STATE_DUEL ) return;

    clock_t now = clock( );
    if ( ( now - m_lastDungeonCheckTime ) < (clock_t)( 120 * CLOCKS_PER_SEC ) ) return;
    m_lastDungeonCheckTime = now;

    if ( m_player->Position && m_player->Position->Map == 2 ) // Junon Polis
    {
        if ( rand( ) % 5 == 0 )
        {
            CMap* dungeonMap = GServer->MapList.Index[51]; // Barka Dungeon
            if ( dungeonMap && dungeonMap != GServer->MapList.nullzone )
            {
                fPoint enterPos = { 5200.0f, 5200.0f, 0.0f };
                dungeonMap->TeleportPlayer( m_player, enterPos, false );
                Say( "Entering Barka Dungeon to raid dungeon bosses!" );
            }
        }
    }
}

// -------------------------------------------------------------
// Advanced Feature 1: Multi-Party Boss Raid Assemblies
// -------------------------------------------------------------
void CPlayerBot::BroadcastRaidRally( CMonster* boss )
{
    if ( !m_player || !boss || !m_player->Position ) return;

    CMap* map = GetMap( );
    if ( !map ) return;

    fPoint bossPos = boss->Position->current;

    for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* other = map->PlayerList[i];
        if ( !other || other == m_player || !other->bot_ai || other->IsDead( ) ) continue;
        CPlayerBot* bot = other->bot_ai;
        if ( bot->IsBuffBot( ) || bot->IsVendingBot( ) ) continue;

        // Roll d20 >= 8 to join raid assembly (~65% rally response chance)
        if ( RollD20( ) >= 8 )
        {
            if ( bot->GetState( ) == BOT_STATE_IDLE || bot->GetState( ) == BOT_STATE_ROAM ||
                 bot->GetState( ) == BOT_STATE_REST || bot->GetState( ) == BOT_STATE_TOWN_STROLL )
            {
                bot->SetRaidTarget( boss, bossPos );
            }
        }
    }
}

void CPlayerBot::SetRaidTarget( CMonster* boss, fPoint rallyPos )
{
    if ( !boss ) return;
    if ( m_state == BOT_STATE_REST ) StandUp( );

    m_rallyBossCid = boss->clientid;
    m_rallyPos = rallyPos;
    SetState( BOT_STATE_RALLY_RAID );

    const char* rallyQuotes[] = {
        "On my way to the raid!",
        "Heading to boss coordinates now!",
        "Rallying up for the World Boss!",
        "Hold on, reinforcement arriving!"
    };
    Say( rallyQuotes[rand( ) % 4] );
}

void CPlayerBot::HandleRallyRaid( )
{
    if ( !m_player || m_player->IsDead( ) ) return;
    CMap* map = GetMap( );
    if ( !map ) { SetState( BOT_STATE_IDLE ); return; }

    CMonster* boss = map->GetMonsterInMap( m_rallyBossCid );
    if ( !boss || boss->IsDead( ) || boss->Stats->HP <= 0 )
    {
        m_rallyBossCid = 0;
        Say( "Boss defeated! Great raid everyone!" );
        DoEmote( 3 ); // Cheer
        SetState( BOT_STATE_IDLE );
        return;
    }

    float dist = GServer->distance( m_player->Position->current, boss->Position->current );

    // Path to raid assembly if far away
    if ( dist > 20.0f )
    {
        MoveTo( boss->Position->current );
        return;
    }

    // Role-Based Raid Combat Positioning & Tactics
    int wType = m_player->getWeaponType( );
    bool isRanged = ( wType == BOW || wType == GUN || wType == LAUNCHER || wType == CROSSBOW || wType == WAND || wType == STAFF );
    bool isHealer = ( m_player->CharInfo->Job == 211 || m_player->CharInfo->Job == 222 );
    bool isTank = ( m_player->CharInfo->Job == 111 || m_player->CharInfo->Job == 121 );

    // Boss Enrage / Heavy AOE Reaction (HP < 25%)
    if ( boss->Stats->HP < ( boss->Stats->MaxHP * 25 / 100 ) && !isTank )
    {
        clock_t now = clock( );
        if ( ( now - m_lastRaidKiteTime ) >= ( 8 * CLOCKS_PER_SEC ) )
        {
            if ( RollD20( ) < 10 ) // Squishy classes step back
            {
                m_lastRaidKiteTime = now;
                KiteTarget( boss );
                Say( "Boss enraging! Step back!" );
                return;
            }
        }
    }

    if ( isTank )
    {
        // Main Tank: Engages front, uses taunt, positions boss
        CheckPartyTaunt( );
        AttackTarget( boss );
    }
    else if ( isHealer )
    {
        // Dedicated Healer: Positions at 12m-15m, prioritizes group heals & buffs
        if ( dist < 10.0f ) KiteTarget( boss );
        if ( CheckPartyHeal( ) ) return;
        if ( CheckPartyBuffs( ) ) return;
        AttackTarget( boss );
    }
    else
    {
        // Ranged / Melee DPS: Flank boss & cast combat skills
        if ( isRanged && dist < 8.0f ) KiteTarget( boss );
        CastCombatSkill( boss );
        AttackTarget( boss );
    }
}

// -------------------------------------------------------------
// Advanced Feature 2: Interactive Economy & Vendor Runs
// -------------------------------------------------------------
void CPlayerBot::HandleVendorRun( )
{
    if ( !m_player ) return;

    // Path to town center coordinates for vendor dump
    fPoint townPos;
    if ( m_player->Position->Map == 2 ) { townPos.x = 5655.0f; townPos.y = 5238.0f; townPos.z = 0; }
    else if ( m_player->Position->Map == 1 ) { townPos.x = 5240.0f; townPos.y = 5189.0f; townPos.z = 0; }
    else { townPos.x = 5839.0f; townPos.y = 5332.0f; townPos.z = 0; }

    float dist = GServer->distance( m_player->Position->current, townPos );
    if ( dist > 8.0f )
    {
        MoveTo( townPos );
        return;
    }

    // Vendor dump completed: gain Zulies from sales
    long long zulyGain = 2500 + ( rand( ) % 10000 );
    m_player->CharInfo->Zulies += zulyGain;

    Say( "Sold off surplus loot to the merchant! Back to grinding." );
    SetState( BOT_STATE_ROAM );
}

void CPlayerBot::TriggerVendorRun( )
{
    if ( m_isBuffBot || m_isVendingBot || m_state == BOT_STATE_DEAD ) return;
    clock_t now = clock( );
    if ( ( now - m_lastVendorRunTime ) < ( 300 * CLOCKS_PER_SEC ) ) return;
    m_lastVendorRunTime = now;

    if ( m_state == BOT_STATE_REST ) StandUp( );
    SetState( BOT_STATE_VENDOR_RUN );
}

// -------------------------------------------------------------
// Advanced Feature 3: Clan System & Dynamic Rivalries
// -------------------------------------------------------------
void CPlayerBot::CheckClanRivalry( CPlayer* other )
{
    if ( !m_player || !other || !other->bot_ai || m_isBuffBot || m_isVendingBot ) return;
    if ( !m_player->Clan || !other->Clan ) return;
    if ( m_player->Clan->clanid == 0 || other->Clan->clanid == 0 ) return;
    if ( m_player->Clan->clanid == other->Clan->clanid ) return;

    clock_t now = clock( );
    if ( ( now - m_lastClanRivalryTime ) < ( 45 * CLOCKS_PER_SEC ) ) return;

    // Roll d20 >= 10 to initiate rivalry banter & skirmish (~55% chance)
    if ( RollD20( ) >= 10 )
    {
        m_lastClanRivalryTime = now;
        char msg[128];
        const char* clanQuotes[] = {
            "For Clan #%d! Prepare yourself, %s!",
            "Rival clan spotted! %s, face me!",
            "Our clan rules this zone, %s!"
        };
        snprintf( msg, sizeof(msg), clanQuotes[rand( ) % 3], m_player->Clan->clanid, other->CharInfo->charname );
        Say( msg );

        StartDuel( other );
    }
}

void CPlayerBot::HandleClanHangout( )
{
    if ( !m_player || !m_player->Clan ) return;

    // Clan hangout coordinates
    fPoint plazaPos;
    if ( m_player->Clan->clanid == 1 ) { plazaPos.x = 5240.0f; plazaPos.y = 5189.0f; plazaPos.z = 0; }
    else if ( m_player->Clan->clanid == 2 ) { plazaPos.x = 5655.0f; plazaPos.y = 5238.0f; plazaPos.z = 0; }
    else { plazaPos.x = 5102.0f; plazaPos.y = 5063.0f; plazaPos.z = 0; }

    float dist = GServer->distance( m_player->Position->current, plazaPos );
    if ( dist > 10.0f )
    {
        MoveTo( plazaPos );
        return;
    }

    if ( rand( ) % 3 == 0 ) SitDown( );
    else DoEmote( 1 ); // Wave
}

// -------------------------------------------------------------
// Advanced Feature 4: Dynamic Questing & Daily Bounties
// -------------------------------------------------------------
void CPlayerBot::AssignNewBounty( )
{
    int lvl = ( m_player && m_player->Stats ) ? m_player->Stats->Level : 1;

    if ( lvl >= 50 )
    {
        m_bounty.targetMobId = 781;
        m_bounty.mobName = "Moldie";
        m_bounty.requiredKills = 15;
    }
    else if ( lvl >= 30 )
    {
        m_bounty.targetMobId = 711;
        m_bounty.mobName = "Goblin Warrior";
        m_bounty.requiredKills = 15;
    }
    else if ( lvl >= 15 )
    {
        m_bounty.targetMobId = 511;
        m_bounty.mobName = "Woofy";
        m_bounty.requiredKills = 15;
    }
    else
    {
        m_bounty.targetMobId = 101;
        m_bounty.mobName = "Jellybean";
        m_bounty.requiredKills = 10;
    }

    m_bounty.currentKills = 0;
    m_bounty.isComplete = false;
    m_bounty.npcMapId = 1;
    m_bounty.npcPos.x = 5240.0f;
    m_bounty.npcPos.y = 5189.0f;
    m_bounty.npcPos.z = 0.0f;
}

void CPlayerBot::CheckBountyProgress( CMonster* mob )
{
    if ( !mob || m_bounty.isComplete ) return;

    m_bounty.currentKills++;
    if ( m_bounty.currentKills >= m_bounty.requiredKills )
    {
        m_bounty.isComplete = true;
        char msg[128];
        snprintf( msg, sizeof(msg), "Daily Bounty Complete (%d/%d %s killed)! Returning to town for reward!",
                  m_bounty.currentKills, m_bounty.requiredKills, m_bounty.mobName.c_str( ) );
        Say( msg );
        SetState( BOT_STATE_QUEST_RETURN );
    }
}

void CPlayerBot::HandleQuestReturn( )
{
    if ( !m_player ) return;

    float dist = GServer->distance( m_player->Position->current, m_bounty.npcPos );
    if ( dist > 6.0f )
    {
        MoveTo( m_bounty.npcPos );
        return;
    }

    // Turn in quest
    DoEmote( 3 ); // Cheer
    Say( "Turned in Daily Bounty to NPC! Received EXP & Zuly reward!" );

    long long zulyReward = 5000 + ( m_player->Stats->Level * 500 );
    m_player->CharInfo->Zulies += zulyReward;
    m_player->CharInfo->Exp += 2500 + ( m_player->Stats->Level * 300 );

    EquipTieredGear( true );
    AssignNewBounty( );
    SetState( BOT_STATE_ROAM );
}

// -------------------------------------------------------------
// Advanced Feature 1 & 2: Social Memory Engine (Gratitude & Nemesis)
// -------------------------------------------------------------
void CPlayerBot::RecordGratitude( CPlayer* player, int points )
{
    if ( !player || player->is_bot || !player->CharInfo ) return;
    m_friendMemory[player->CharInfo->charid] += points;

    if ( m_friendMemory[player->CharInfo->charid] == points )
    {
        char msg[128];
        snprintf( msg, sizeof(msg), "Thanks @%s! I won't forget your help!", player->CharInfo->charname );
        Say( msg );
    }
}

void CPlayerBot::RecordNemesis( CPlayer* player, int points )
{
    if ( !player || player->is_bot || !player->CharInfo ) return;
    m_nemesisMemory[player->CharInfo->charid] += points;
}

bool CPlayerBot::IsFriend( CPlayer* player ) const
{
    if ( !player || !player->CharInfo ) return false;
    std::map<UINT, int>::const_iterator it = m_friendMemory.find( player->CharInfo->charid );
    return ( it != m_friendMemory.end( ) && it->second > 0 );
}

bool CPlayerBot::IsNemesis( CPlayer* player ) const
{
    if ( !player || !player->CharInfo ) return false;
    std::map<UINT, int>::const_iterator it = m_nemesisMemory.find( player->CharInfo->charid );
    return ( it != m_nemesisMemory.end( ) && it->second > 0 );
}

void CPlayerBot::CheckNemesisRevenge( CPlayer* player )
{
    if ( !player || !player->CharInfo || m_state == BOT_STATE_DEAD || m_state == BOT_STATE_DUEL ) return;

    if ( RollD20( ) >= 7 ) // ~70% chance to react to nemesis
    {
        char msg[128];
        snprintf( msg, sizeof(msg), "You again, @%s! Time for paybacks!", player->CharInfo->charname );
        Say( msg );
        StartDuel( player );
    }
}

// -------------------------------------------------------------
// Advanced Feature 3: Player-Bot Guild Creation & Recruitment
// -------------------------------------------------------------
void CPlayerBot::CheckClanRecruitmentShout( )
{
    if ( !m_player || !m_player->Clan || m_player->Clan->clanid == 0 || m_isBuffBot || m_isVendingBot ) return;
    if ( m_player->Clan->clanrank > 2 ) return; // Only Clan Leaders & Officers recruit

    clock_t now = clock( );
    if ( ( now - m_lastRecruitmentShoutTime ) < ( 180 * CLOCKS_PER_SEC ) ) return;
    m_lastRecruitmentShoutTime = now;

    if ( m_player->Position && ( m_player->Position->Map == 1 || m_player->Position->Map == 2 ) )
    {
        char msg[128];
        snprintf( msg, sizeof(msg), "<<Clan #%d>> is recruiting active grinders LVL 30+! PST to join!", m_player->Clan->clanid );

        BEGINPACKET( pak, 0x0785 );
        ADDSTRING  ( pak, m_player->CharInfo->charname );
        ADDBYTE    ( pak, 0 );
        ADDSTRING  ( pak, msg );
        ADDBYTE    ( pak, 0 );
        GServer->SendToMap( &pak, m_player->Position->Map );
    }
}

void CPlayerBot::HandleClanInviteRequest( CPlayer* sender )
{
    if ( !m_player || !sender || !sender->CharInfo || !m_player->Clan ) return;

    if ( sender->Clan && sender->Clan->clanid != 0 )
    {
        WhisperPlayer( sender, "You are already in a clan! Leave your current clan first to join ours." );
        return;
    }

    int lvl = sender->Stats ? sender->Stats->Level : 1;
    if ( lvl < 20 )
    {
        WhisperPlayer( sender, "Sorry! We are currently looking for members level 20 or higher." );
        return;
    }

    if ( RollD20( ) >= 8 ) // ~65% acceptance rate
    {
        sender->Clan->clanid = m_player->Clan->clanid;
        sender->Clan->clanrank = 5; // Member rank

        char msg[128];
        snprintf( msg, sizeof(msg), "Welcome to Clan #%d, %s! Great to have you with us!", m_player->Clan->clanid, sender->CharInfo->charname );
        WhisperPlayer( sender, msg );
    }
    else
    {
        WhisperPlayer( sender, "Thanks for applying! We will review your application soon." );
    }
}

// -------------------------------------------------------------
// Advanced Feature 4: Direct Player-to-Bot Trading Engine
// -------------------------------------------------------------
void CPlayerBot::HandleTradeRequest( CPlayer* sender )
{
    if ( !m_player || !sender || !sender->CharInfo || m_state == BOT_STATE_DEAD || m_isVendingBot ) return;

    int chaMod = ( m_player && m_player->Attr ) ? ( m_player->Attr->Cha / 10 ) : 0;
    int roll = RollD20( ) + chaMod;

    if ( roll >= 10 )
    {
        long long offeredZulies = 10000 + ( rand( ) % 50000 );
        if ( IsFriend( sender ) ) offeredZulies = offeredZulies * 115 / 100; // 15% bonus for friends

        char msg[128];
        snprintf( msg, sizeof(msg), "Trade accepted! I can offer up to %lld Zulies for your surplus gear. Place your items!", offeredZulies );
        WhisperPlayer( sender, msg );
    }
    else
    {
        WhisperPlayer( sender, "Sorry, I'm not looking to trade right now!" );
    }
}

// -------------------------------------------------------------
// Advanced Feature 5: Clan War / Siege Unit AI
// -------------------------------------------------------------
void CPlayerBot::HandleSiegeUnit( )
{
    if ( !m_player || m_player->IsDead( ) ) return;

    fPoint gatePos = { 5200.0f, 5200.0f, 0.0f };
    float dist = GServer->distance( m_player->Position->current, gatePos );

    if ( dist > 15.0f )
    {
        MoveTo( gatePos );
        return;
    }

    CMonster* target = FindNearbyMonster( 25.0f );
    if ( target )
    {
        AttackTarget( target );
    }
}

// -------------------------------------------------------------
// Auto Companion Party System Implementation
// -------------------------------------------------------------

BotRole CPlayerBot::DetermineRoleFromJob( int job )
{
    if ( job == 121 ) return BOT_ROLE_TANK; // Knight
    if ( job == 111 ) return BOT_ROLE_TANK; // Soldier
    if ( job == 122 ) return BOT_ROLE_DPS_PHYS_MELEE; // Champion
    if ( job == 222 ) return BOT_ROLE_HEALER; // Cleric
    if ( job == 211 ) return BOT_ROLE_HEALER; // Muse
    if ( job == 221 ) return BOT_ROLE_DPS_MAGIC; // Mage
    if ( job == 322 || job == 311 ) return BOT_ROLE_DPS_PHYS_RANGED; // Scout / Hawker
    if ( job == 321 ) return BOT_ROLE_DPS_PHYS_MELEE; // Raider
    if ( job == 421 || job == 411 ) return BOT_ROLE_DPS_PHYS_RANGED; // Bourgeois / Dealer
    if ( job == 422 ) return BOT_ROLE_DPS_PHYS_MELEE; // Artisan
    return BOT_ROLE_GENERAL_VISITOR;
}

void CPlayerBot::SyncLevelToLeader( int targetLevel )
{
    if ( !m_player || !m_player->Stats ) return;
    if ( targetLevel < 1 ) targetLevel = 1;
    if ( targetLevel > GServer->Config.MaxLevel ) targetLevel = GServer->Config.MaxLevel;

    int oldLevel = m_player->Stats->Level;

    // Determine the expected Job/Class matching the target level tier and bot role
    int expectedJob = 0;
    if ( targetLevel < 10 )
    {
        expectedJob = 0; // Below level 10: strictly Visitor (no class yet)
    }
    else if ( targetLevel < 70 )
    {
        // 1st Job tier (levels 10-69)
        switch ( m_role )
        {
            case BOT_ROLE_TANK:
            case BOT_ROLE_DPS_PHYS_MELEE:
                expectedJob = 111; // Soldier
                break;
            case BOT_ROLE_HEALER:
            case BOT_ROLE_DPS_MAGIC:
                expectedJob = 211; // Muse
                break;
            case BOT_ROLE_DPS_PHYS_RANGED:
                expectedJob = 311; // Hawker
                break;
            default:
                expectedJob = 111;
                break;
        }
    }
    else
    {
        // 2nd Job tier (levels 70+)
        switch ( m_role )
        {
            case BOT_ROLE_TANK:
                expectedJob = 121; // Knight
                break;
            case BOT_ROLE_DPS_PHYS_MELEE:
                expectedJob = 122; // Champion
                break;
            case BOT_ROLE_HEALER:
                expectedJob = 222; // Cleric
                break;
            case BOT_ROLE_DPS_MAGIC:
                expectedJob = 221; // Mage
                break;
            case BOT_ROLE_DPS_PHYS_RANGED:
                expectedJob = 322; // Scout
                break;
            default:
                expectedJob = 121;
                break;
        }
    }

    if ( oldLevel == targetLevel && m_lastKnownLevel == targetLevel && m_player->CharInfo->Job == (UINT)expectedJob )
        return;

    m_player->CharInfo->Job = expectedJob;
    m_player->Stats->Level = targetLevel;
    m_lastKnownLevel = targetLevel;

    // Reset base attributes
    m_player->Attr->Str = 15;
    m_player->Attr->Dex = 15;
    m_player->Attr->Int = 15;
    m_player->Attr->Con = 15;
    m_player->Attr->Cha = 10;
    m_player->Attr->Sen = 10;

    int job = m_player->CharInfo->Job;
    for ( int lvl = 1; lvl < targetLevel; lvl++ )
    {
        if ( job == 111 || job == 121 || job == 122 ) // Soldier / Knight / Champion
        {
            m_player->Attr->Str += 3;
            m_player->Attr->Con += 2;
        }
        else if ( job == 211 || job == 221 || job == 222 ) // Muse / Mage / Cleric
        {
            m_player->Attr->Int += 3;
            m_player->Attr->Con += 1;
            m_player->Attr->Sen += 1;
        }
        else if ( job == 311 || job == 321 || job == 322 ) // Hawker / Raider / Scout
        {
            m_player->Attr->Dex += 3;
            m_player->Attr->Sen += 1;
            m_player->Attr->Str += 1;
        }
        else if ( job == 411 || job == 421 || job == 422 ) // Dealer / Bourgeois / Artisan
        {
            m_player->Attr->Con += 2;
            m_player->Attr->Sen += 2;
            m_player->Attr->Str += 1;
        }
        else // Visitor (Novice)
        {
            if ( m_role == BOT_ROLE_TANK )
            {
                m_player->Attr->Str += 3;
                m_player->Attr->Con += 2;
            }
            else if ( m_role == BOT_ROLE_HEALER || m_role == BOT_ROLE_DPS_MAGIC )
            {
                m_player->Attr->Int += 3;
                m_player->Attr->Con += 1;
                m_player->Attr->Sen += 1;
            }
            else if ( m_role == BOT_ROLE_DPS_PHYS_RANGED )
            {
                m_player->Attr->Dex += 3;
                m_player->Attr->Sen += 2;
            }
            else
            {
                m_player->Attr->Str += 2;
                m_player->Attr->Dex += 2;
                m_player->Attr->Con += 1;
            }
        }
    }

    // Re-equip gear and re-train skills to target level
    EquipTieredGear( true );
    UpdateSkills( );

    m_player->SetStats( );
    m_player->Stats->HP = m_player->Stats->MaxHP;
    m_player->Stats->MP = m_player->Stats->MaxMP;

    // Broadcast level up visual effect if leveling up
    if ( targetLevel > oldLevel )
    {
        BEGINPACKET( pak, 0x79e );
        ADDWORD( pak, m_player->clientid );
        ADDWORD( pak, m_player->Stats->Level );
        ADDDWORD( pak, 0 );
        ADDWORD( pak, 0 );
        ADDWORD( pak, 0 );
        GServer->SendToVisible( &pak, m_player );

        CelebrateLevelUp( );
    }

    // Refresh party HUD
    if ( m_player->Party && m_player->Party->party )
    {
        CBotManager::GetInstance()->SendPartyFullUpdate( m_player->Party->party );
    }
}

void CPlayerBot::AdjustSpecToRole( BotRole role, int level )
{
    if ( !m_player ) return;
    m_role = role;
    SyncLevelToLeader( level );
}

void CPlayerBot::AddAffinity( int amount )
{
    int oldAffinity = m_affinity;
    m_affinity += amount;
    if ( m_affinity > 100 ) m_affinity = 100;
    if ( m_affinity < 0 ) m_affinity = 0;

    // Check tier crossing & celebratory banter
    if ( m_isCompanion && m_followTarget )
    {
        clock_t now = clock( );
        if ( ( now - m_lastAffinityBanterTime ) >= ( 30 * CLOCKS_PER_SEC ) )
        {
            if ( oldAffinity < 70 && m_affinity >= 70 )
            {
                m_lastAffinityBanterTime = now;
                Say( "You have my absolute loyalty, leader! Together we are unstoppable!" );
                DoEmote( 3 ); // Cheer
            }
            else if ( oldAffinity < 90 && m_affinity >= 90 )
            {
                m_lastAffinityBanterTime = now;
                Say( "Soulbound bond achieved! I will fight to the last breath for this squad!" );
                DoEmote( 6 ); // Dance
            }
        }
    }
}

const char* CPlayerBot::GetAffinityTierString( ) const
{
    if ( m_affinity >= 90 ) return "Soulbound Champion";
    if ( m_affinity >= 70 ) return "Loyal";
    if ( m_affinity >= 30 ) return "Friendly";
    return "Hesitant";
}

const char* CPlayerBot::GetPersonalityString( ) const
{
    switch ( m_role )
    {
        case BOT_ROLE_TANK: return "Loyal Knight";
        case BOT_ROLE_HEALER: return "Caring Healer";
        case BOT_ROLE_DPS_MAGIC: return "Sassy Elementalist";
        case BOT_ROLE_DPS_PHYS_RANGED: return "Cocky Sharpshooter";
        case BOT_ROLE_DPS_PHYS_MELEE: return "Valiant Striker";
        default: return "Eager Adventurer";
    }
}

bool CPlayerBot::IsDungeonMap( UINT mapId ) const
{
    // Goblin Caves (51, 52), Marsh of Ghosts / Pyramids (53), Crystal Cave (61) or special dungeon maps
    return ( ( mapId >= 51 && mapId <= 65 ) || mapId == 24 || mapId == 25 );
}

void CPlayerBot::StartPullTarget( CMonster* mob, fPoint returnPos )
{
    if ( !mob || !m_player ) return;
    m_isPullingTarget = true;
    m_pullAnchorPos = returnPos;
    m_targetMobCid = mob->clientid;
    AttackTarget( mob );
    SetState( BOT_STATE_COMBAT );
    Say( "Pulling the target mob back to the squad! Stand by for ambush!" );
}

bool CPlayerBot::EquipGiftedItem( int slot, const CItem& itm )
{
    if ( !m_player || itm.itemnum == 0 ) return false;
    if ( slot < 1 || slot > 8 ) return false;

    m_player->items[slot] = itm;
    m_player->items[slot].count = 1;
    m_player->items[slot].appraised = true;
    m_player->SetStats( );

    // Broadcast visual change (0x7a5)
    BEGINPACKET( pak, 0x7a5 );
    ADDWORD    ( pak, m_player->clientid );
    ADDWORD    ( pak, slot );
    ADDWORD    ( pak, m_player->items[slot].itemnum );
    ADDWORD    ( pak, GServer->BuildItemRefine( &m_player->items[slot] ) );
    ADDWORD    ( pak, m_player->Stats->Move_Speed );
    GServer->SendToVisible( &pak, m_player );

    AddAffinity( 15 );

    char giftMsg[96];
    snprintf( giftMsg, sizeof(giftMsg), "Thank you so much! This gear feels amazing on me!" );
    Say( giftMsg );
    DoEmote( 4 ); // Bow
    return true;
}

bool CPlayerBot::CheckMinionPeel( )
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return false;
    // Only DPS roles peel off bosses to clear adds
    if ( m_role != BOT_ROLE_DPS_PHYS_MELEE && m_role != BOT_ROLE_DPS_PHYS_RANGED && m_role != BOT_ROLE_DPS_MAGIC )
        return false;

    CMap* map = GetMap( );
    if ( !map ) return false;

    CParty* party = m_player->Party->party;
    CPlayer* leader = GetPartyLeader( );
    CPlayer* healer = NULL;
    for ( size_t i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* mem = party->Members[i];
        if ( mem && mem->is_bot && mem->bot_ai && mem->bot_ai->GetRole( ) == BOT_ROLE_HEALER )
        {
            healer = mem;
            break;
        }
    }

    // Check if any minion/add is attacking healer or leader
    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* add = map->MonsterList[i];
        if ( !add || add->IsDead( ) || add->Stats->HP <= 0 || add->IsBonfire( ) ) continue;
        if ( add->clientid == m_targetMobCid ) continue;

        bool threat = false;
        if ( healer && add->Battle && add->Battle->target == healer->clientid ) threat = true;
        if ( leader && add->Battle && add->Battle->target == leader->clientid ) threat = true;

        if ( threat )
        {
            float d = GServer->distance( m_player->Position->current, add->Position->current );
            if ( d <= 25.0f )
            {
                AttackTarget( add );
                m_targetMobCid = add->clientid;
                SetState( BOT_STATE_COMBAT );
                Say( "Peeling minion off the squad! Clearing adds!" );
                return true;
            }
        }
    }
    return false;
}

bool CPlayerBot::CheckSpellInterrupt( CMonster* mob )
{
    if ( !mob || !m_player || mob->IsDead( ) || mob->Stats->HP <= 0 ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 20 ) return false;

    // Check if monster is casting a skill
    if ( !mob->Battle || mob->Battle->skillid == 0 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastInterruptTime ) < (clock_t)( 8.0f * CLOCKS_PER_SEC ) ) return false;

    UINT interruptSkill = 0;
    if ( m_role == BOT_ROLE_TANK || m_player->CharInfo->Job == 111 || m_player->CharInfo->Job == 121 )
    {
        interruptSkill = HasSkill( 351 ) ? 351 : ( HasSkill( 261 ) ? 261 : 0 );
    }
    else if ( m_role == BOT_ROLE_DPS_PHYS_RANGED || m_player->CharInfo->Job == 311 || m_player->CharInfo->Job == 322 )
    {
        interruptSkill = HasSkill( 1501 ) ? 1501 : 0;
    }
    else if ( m_role == BOT_ROLE_DPS_MAGIC || m_player->CharInfo->Job == 211 || m_player->CharInfo->Job == 221 )
    {
        interruptSkill = HasSkill( 821 ) ? 821 : 0;
    }

    if ( interruptSkill != 0 )
    {
        m_lastInterruptTime = now;
        m_lastSkillCastTime = now;
        m_player->StartAction( mob, SKILL_ATTACK, interruptSkill );
        // Cancel monster channel
        mob->Battle->skillid = 0;
        mob->Battle->skilltarget = 0;

        const char* shout = ( m_role == BOT_ROLE_TANK ) ? "Shield Bash! Spell channel cancelled!" :
                            ( ( m_role == BOT_ROLE_DPS_PHYS_RANGED ) ? "Stun Arrow! Interrupted mob cast!" : "Counter-spell cast! Interrupted!" );
        Say( shout );
        AddSynergy( 15 );
        return true;
    }
    return false;
}

void CPlayerBot::HandleBossCleaveAndTactics( CMonster* mob )
{
    if ( !mob || !m_player || !m_followTarget || !m_followTarget->Position ) return;

    bool isBoss = ( mob->Stats->MaxHP >= 4000 || mob->montype >= 400 );
    if ( !isBoss ) return;

    // A. Tank Cleave Facing: Position Tank on opposite side of boss facing away from leader
    if ( m_role == BOT_ROLE_TANK )
    {
        float dx = mob->Position->current.x - m_followTarget->Position->current.x;
        float dy = mob->Position->current.y - m_followTarget->Position->current.y;
        float len = sqrt( dx * dx + dy * dy );
        if ( len > 0.5f )
        {
            dx /= len;
            dy /= len;
            fPoint tankPos;
            tankPos.x = mob->Position->current.x + dx * 2.2f;
            tankPos.y = mob->Position->current.y + dy * 2.2f;
            tankPos.z = mob->Position->current.z;

            float curDist = GServer->distance( m_player->Position->current, tankPos );
            if ( curDist > 1.8f && curDist < 15.0f )
            {
                MoveTo( tankPos );
            }
        }
    }
    // B. Ranged / Healer AOE Dodging: If too close to a melee boss (< 6.5m), backpedal
    else if ( m_role == BOT_ROLE_DPS_PHYS_RANGED || m_role == BOT_ROLE_DPS_MAGIC || m_role == BOT_ROLE_HEALER )
    {
        float distToBoss = GServer->distance( m_player->Position->current, mob->Position->current );
        if ( distToBoss < 6.5f )
        {
            KiteTarget( mob );
        }
    }
}

const char* CPlayerBot::GetTacticString( ) const
{
    switch ( m_tactic )
    {
        case BOT_TACTIC_SPEEDRUN: return "Speedrun";
        case BOT_TACTIC_SAFE:     return "Safe";
        case BOT_TACTIC_ECO:      return "Eco";
        default:                  return "Balanced";
    }
}

int CPlayerBot::DoFieldAppraisal( )
{
    if ( !m_followTarget || m_followTarget->is_bot ) return 0;
    int count = 0;
    for ( int i = 1; i < 140; i++ )
    {
        CItem& itm = m_followTarget->items[i];
        if ( itm.itemnum > 0 && !itm.appraised )
        {
            itm.appraised = true;
            if ( itm.stats == 0 )
            {
                itm.stats = ( rand( ) % 30 ) + 1;
            }
            m_followTarget->UpdateInventory( i, 0xffff, true );
            count++;
        }
    }
    if ( count > 0 )
    {
        char msg[128];
        snprintf( msg, sizeof(msg), "Appraised %d unidentified items in your bag, %s! Hidden powers unlocked!",
            count, m_followTarget->CharInfo->charname );
        Say( msg );
    }
    return count;
}

int CPlayerBot::DoFieldRepair( )
{
    if ( !m_followTarget || m_followTarget->is_bot ) return 0;
    int count = 0;
    for ( int i = 1; i < 140; i++ )
    {
        CItem& itm = m_followTarget->items[i];
        if ( itm.itemnum > 0 && itm.durability < 50 )
        {
            itm.durability = 50;
            m_followTarget->UpdateInventory( i, 0xffff, true );
            count++;
        }
    }
    if ( count > 0 )
    {
        char msg[128];
        snprintf( msg, sizeof(msg), "Repaired %d pieces of gear back to full durability (50/50), %s!",
            count, m_followTarget->CharInfo->charname );
        Say( msg );
    }
    return count;
}

bool CPlayerBot::DoFieldSalvage( int slot )
{
    if ( !m_followTarget || m_followTarget->is_bot ) return false;
    if ( slot < 9 || slot >= 140 ) return false;

    CItem& itm = m_followTarget->items[slot];
    if ( itm.itemnum == 0 ) return false;

    uint64_t scrapValue = (uint64_t)( itm.count > 0 ? itm.count : 1 ) * 650ULL;
    ClearItem( itm );
    m_followTarget->UpdateInventory( slot, 0xffff, true );
    m_followTarget->CharInfo->Zulies += scrapValue;

    BEGINPACKET( pak, 0x71d );
    ADDQWORD( pak, m_followTarget->CharInfo->Zulies );
    m_followTarget->client->SendPacket( &pak );

    char msg[128];
    snprintf( msg, sizeof(msg), "Salvaged item in bag slot %d into %llu Zulies for the squad fund!",
        slot, (unsigned long long)scrapValue );
    Say( msg );
    return true;
}

void CPlayerBot::CheckAmmunitionResupply( )
{
    if ( !m_isCompanion || !m_followTarget || m_followTarget->is_bot ) return;
    clock_t now = clock( );
    if ( ( now - m_lastResupplyTime ) < (clock_t)( 20.0f * CLOCKS_PER_SEC ) ) return;
    m_lastResupplyTime = now;

    int wType = m_followTarget->getWeaponType( );
    if ( wType == BOW || wType == CROSSBOW )
    {
        if ( m_followTarget->items[132].count < 100 )
        {
            m_followTarget->items[132].itemtype = NATURAL;
            m_followTarget->items[132].itemnum = 1;
            m_followTarget->items[132].count = 500;
            m_followTarget->items[132].appraised = true;
            m_followTarget->UpdateInventory( 132, 0xffff, true );
            Say( "Here is a fresh quiver of arrows, leader! Keep firing!" );
        }
    }
    else if ( wType == GUN || wType == LAUNCHER )
    {
        if ( m_followTarget->items[133].count < 100 )
        {
            m_followTarget->items[133].itemtype = NATURAL;
            m_followTarget->items[133].itemnum = 1;
            m_followTarget->items[133].count = 500;
            m_followTarget->items[133].appraised = true;
            m_followTarget->UpdateInventory( 133, 0xffff, true );
            Say( "Fresh ammo magazine loaded for you, leader! Full auto!" );
        }
    }
}

void CPlayerBot::CheckClanSync( )
{
    if ( !m_isCompanion || !m_player || !m_followTarget || m_followTarget->is_bot ) return;
    clock_t now = clock( );
    if ( ( now - m_lastClanSyncTime ) < (clock_t)( 15.0f * CLOCKS_PER_SEC ) ) return;
    m_lastClanSyncTime = now;

    if ( m_followTarget->Clan && m_followTarget->Clan->clanid != 0 )
    {
        if ( m_player->Clan->clanid != m_followTarget->Clan->clanid )
        {
            m_player->Clan->clanid = m_followTarget->Clan->clanid;
            m_player->Clan->clanrank = 1;
            m_player->Clan->grade = m_followTarget->Clan->grade;
            m_player->Clan->logo = m_followTarget->Clan->logo;
            m_player->Clan->back = m_followTarget->Clan->back;
            strncpy( m_player->Clan->clanname, m_followTarget->Clan->clanname, sizeof(m_player->Clan->clanname) - 1 );

            BEGINPACKET( pak, 0x7e0 );
            ADDBYTE    ( pak, 0x35 );
            ADDWORD    ( pak, m_player->clientid );
            ADDWORD    ( pak, m_player->Clan->clanid );
            ADDWORD    ( pak, 0x0000 );
            ADDWORD    ( pak, m_player->Clan->back );
            ADDWORD    ( pak, m_player->Clan->logo );
            ADDBYTE    ( pak, m_player->Clan->grade );
            ADDBYTE    ( pak, m_player->Clan->clanrank );
            ADDSTRING  ( pak, m_player->Clan->clanname );
            ADDBYTE    ( pak, 0x00 );
            GServer->SendToVisible( &pak, m_player );
        }
    }
    else if ( m_player->Clan && m_player->Clan->clanid != 0 && ( !m_followTarget->Clan || m_followTarget->Clan->clanid == 0 ) )
    {
        m_player->Clan->clanid = 0;
        m_player->Clan->clanname[0] = '\0';
    }
}

bool CPlayerBot::HandleOffTargetCrowdControl( CMonster* primaryMob )
{
    if ( !m_player || !m_followTarget || !primaryMob ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 25 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastCcTime ) < (clock_t)( 10.0f * CLOCKS_PER_SEC ) ) return false;

    CMap* map = GetMap( );
    if ( !map ) return false;

    CMonster* secondaryTarget = NULL;
    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || mob == primaryMob || mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) ) continue;
        if ( !mob->Battle || mob->Battle->target == 0 ) continue;

        if ( mob->Battle->target == m_followTarget->clientid || mob->Battle->target == m_player->clientid )
        {
            float d = GServer->distance( m_player->Position->current, mob->Position->current );
            if ( d <= 20.0f )
            {
                secondaryTarget = mob;
                break;
            }
        }
    }

    if ( !secondaryTarget ) return false;

    UINT ccSkill = 0;
    const char* ccPhrase = NULL;

    if ( m_role == BOT_ROLE_DPS_MAGIC || m_player->CharInfo->Job == 211 || m_player->CharInfo->Job == 221 )
    {
        ccSkill = HasSkill( 821 ) ? 821 : 0;
        ccPhrase = "Ice Bolt crowd control! Freezing off-target add!";
    }
    else if ( m_role == BOT_ROLE_DPS_PHYS_RANGED || m_player->CharInfo->Job == 311 || m_player->CharInfo->Job == 322 )
    {
        ccSkill = HasSkill( 1501 ) ? 1501 : 0;
        ccPhrase = "Pinning down off-target add with Stun Shot!";
    }
    else if ( m_role == BOT_ROLE_TANK || m_player->CharInfo->Job == 111 || m_player->CharInfo->Job == 121 )
    {
        ccSkill = HasSkill( 351 ) ? 351 : ( HasSkill( 261 ) ? 261 : 0 );
        ccPhrase = "Secondary add peeled and stunned! Keep focus on the boss!";
    }

    if ( ccSkill != 0 )
    {
        m_lastCcTime = now;
        m_lastSkillCastTime = now;
        m_player->StartAction( secondaryTarget, SKILL_ATTACK, ccSkill );
        if ( ccPhrase ) Say( ccPhrase );
        AddSynergy( 10 );
        return true;
    }
    return false;
}

bool CPlayerBot::CheckPuzzleInteraction( )
{
    if ( !m_isCompanion || !m_player || !m_player->Position ) return false;
    if ( !IsDungeonMap( m_player->Position->Map ) ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastPuzzleCheck ) < (clock_t)( 15.0f * CLOCKS_PER_SEC ) ) return false;
    m_lastPuzzleCheck = now;

    CMap* map = GetMap( );
    if ( !map ) return false;

    for ( UINT i = 0; i < map->NPCList.size( ); i++ )
    {
        CNPC* npc = map->NPCList[i];
        if ( !npc ) continue;
        float d = GServer->distance( m_player->Position->current, npc->pos );
        if ( d <= 10.0f )
        {
            DoEmote( 4 );
            char puzzleMsg[96];
            snprintf( puzzleMsg, sizeof(puzzleMsg), "Dungeon mechanism spotted! Ready to activate it on your mark, %s!",
                m_followTarget ? m_followTarget->CharInfo->charname : "leader" );
            Say( puzzleMsg );
            return true;
        }
    }
    return false;
}

void CPlayerBot::AddSynergy( int amount )
{
    if ( !m_isCompanion || !m_followTarget ) return;
    m_synergy += amount;
    if ( m_synergy > 100 ) m_synergy = 100;
    if ( m_synergy < 0 ) m_synergy = 0;

    if ( m_player->Party && m_player->Party->party )
    {
        CParty* party = m_player->Party->party;
        for ( size_t i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* mem = party->Members[i];
            if ( mem && mem->is_bot && mem->bot_ai && mem != m_player )
            {
                reinterpret_cast<CPlayerBot*>( mem->bot_ai )->m_synergy = m_synergy;
            }
        }
    }
}

bool CPlayerBot::ExecuteUnisonUltimate( )
{
    if ( !m_isCompanion || !m_player || !m_followTarget ) return false;
    if ( m_synergy < 100 ) return false;

    m_synergy = 0;
    if ( m_player->Party && m_player->Party->party )
    {
        CParty* party = m_player->Party->party;
        for ( size_t i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* mem = party->Members[i];
            if ( mem && mem->is_bot && mem->bot_ai )
            {
                reinterpret_cast<CPlayerBot*>( mem->bot_ai )->m_synergy = 0;
            }
        }
    }

    CMap* map = GetMap( );
    if ( !map ) return false;

    char shoutMsg[128];
    snprintf( shoutMsg, sizeof(shoutMsg), "[SQUAD UNISON ATTACK] The party unleashes their ultimate Limit Break synergy!" );
    Say( shoutMsg );

    CParty* party = m_player->Party->party;
    if ( party )
    {
        for ( size_t i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* mem = party->Members[i];
            if ( !mem || !mem->is_bot || !mem->bot_ai ) continue;
            CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( mem->bot_ai );
            BotRole role = botAi->GetRole( );

            if ( role == BOT_ROLE_TANK )
            {
                botAi->Say( "Limit Break: AEGIS BASTION! Frontline bulwark activated! Taunting all foes!" );
                botAi->DoEmote( 4 );
                for ( UINT m = 0; m < map->MonsterList.size( ); m++ )
                {
                    CMonster* mob = map->MonsterList[m];
                    if ( mob && !mob->IsDead( ) && mob->Stats->HP > 0 && !mob->IsBonfire( ) )
                    {
                        if ( GServer->distance( mem->Position->current, mob->Position->current ) <= 25.0f )
                        {
                            mob->Battle->target = mem->clientid;
                        }
                    }
                }
            }
            else if ( role == BOT_ROLE_DPS_MAGIC )
            {
                botAi->Say( "Limit Break: GRAND CATACLYSM! Arcane heavens rain celestial fire!" );
                botAi->DoEmote( 3 );
                for ( UINT m = 0; m < map->MonsterList.size( ); m++ )
                {
                    CMonster* mob = map->MonsterList[m];
                    if ( mob && !mob->IsDead( ) && mob->Stats->HP > 0 && !mob->IsBonfire( ) )
                    {
                        if ( GServer->distance( mem->Position->current, mob->Position->current ) <= 20.0f )
                        {
                            long long dmg = (long long)mem->Stats->Attack_Power * 4 + 800;
                            if ( mob->Stats->HP > dmg ) mob->Stats->HP -= dmg;
                            else mob->Stats->HP = 0;
                        }
                    }
                }
            }
            else if ( role == BOT_ROLE_DPS_PHYS_RANGED || role == BOT_ROLE_DPS_PHYS_MELEE )
            {
                botAi->Say( "Limit Break: ARROW STORM! Piercing volleys shredding enemy defenses!" );
                botAi->DoEmote( 6 );
                for ( UINT m = 0; m < map->MonsterList.size( ); m++ )
                {
                    CMonster* mob = map->MonsterList[m];
                    if ( mob && !mob->IsDead( ) && mob->Stats->HP > 0 && !mob->IsBonfire( ) )
                    {
                        if ( GServer->distance( mem->Position->current, mob->Position->current ) <= 20.0f )
                        {
                            long long dmg = (long long)mem->Stats->Attack_Power * 3 + 400;
                            if ( mob->Stats->HP > dmg ) mob->Stats->HP -= dmg;
                            else mob->Stats->HP = 0;
                            mob->Stats->Move_Speed = ( mob->Stats->Move_Speed > 200 ) ? ( mob->Stats->Move_Speed / 2 ) : 150;
                        }
                    }
                }
            }
            else if ( role == BOT_ROLE_HEALER )
            {
                botAi->Say( "Limit Break: DIVINE RADIANCE! Full squad celestial restoration!" );
                botAi->DoEmote( 4 );
                for ( size_t p = 0; p < party->Members.size( ); p++ )
                {
                    CPlayer* ally = party->Members[p];
                    if ( ally && !ally->IsDead( ) )
                    {
                        ally->Stats->HP = ally->Stats->MaxHP;
                        ally->Stats->MP = ally->Stats->MaxMP;
                        ally->RefreshHPMP( );
                        ally->Status->Poisoned = 0xff;
                        ally->Status->Muted = 0xff;
                        ally->Status->Stuned = 0xff;
                        ally->Status->Curse = 0xff;
                        ally->Status->Attack_down = 0xff;
                        ally->Status->Defense_down = 0xff;
                        ally->Status->Haste_down = 0xff;
                        ally->Status->Dash_down = 0xff;
                    }
                }
            }
        }
    }

    if ( m_followTarget )
    {
        GServer->SendPM( m_followTarget, "[Squad Unison] All class Limit Breaks executed! Full squad restored, massive AoE damage inflicted!" );
    }
    return true;
}

bool CPlayerBot::DoFieldRefine( int slot )
{
    if ( !m_followTarget || m_followTarget->is_bot ) return false;
    if ( slot < 1 || slot >= 140 ) return false;

    CItem& itm = m_followTarget->items[slot];
    if ( itm.itemnum == 0 ) return false;

    if ( itm.refine >= 9 )
    {
        Say( "This equipment is already refined to the maximum of +9! Its power cannot be increased further!" );
        return true;
    }

    itm.refine++;
    m_followTarget->UpdateInventory( slot, 0xffff, true );
    AddAffinity( 5 );

    char msg[128];
    snprintf( msg, sizeof(msg), "The forge hammer strikes true! Your equipment in slot %d has been refined to +%d!",
        slot, itm.refine );
    Say( msg );
    return true;
}

bool CPlayerBot::DoFieldSocket( int slot, int gemSlot )
{
    if ( !m_followTarget || m_followTarget->is_bot ) return false;
    if ( slot < 1 || slot >= 140 || gemSlot < 9 || gemSlot >= 140 ) return false;

    CItem& equip = m_followTarget->items[slot];
    CItem& gem = m_followTarget->items[gemSlot];

    if ( equip.itemnum == 0 || gem.itemnum == 0 ) return false;

    if ( !equip.socketed )
    {
        Say( "This piece of gear does not have a socket slot! Socketing cannot be performed." );
        return true;
    }

    equip.gem = gem.itemnum;
    equip.stats = 301 + ( gem.itemnum % 50 );

    if ( gem.count > 1 ) gem.count--;
    else ClearItem( gem );

    m_followTarget->UpdateInventory( slot, gemSlot, true );
    AddAffinity( 10 );

    char msg[128];
    snprintf( msg, sizeof(msg), "Masterwork jewelcrafting complete! Socketed gem into slot %d, awakening its latent power!", slot );
    Say( msg );
    return true;
}

void CPlayerBot::HarvestReagents( CMonster* mob )
{
    if ( !m_isCompanion || !m_followTarget || !mob ) return;

    bool isArtisan = ( m_player->CharInfo->Job == 422 || m_player->CharInfo->Job == 411 || m_player->CharInfo->Job == 421 || strcasecmp( m_player->CharInfo->charname, "Barnaby" ) == 0 );
    if ( !isArtisan ) return;

    if ( ( rand( ) % 100 ) < 35 )
    {
        long long extraZulies = 300 + ( ( rand( ) % 8 ) * 150 );
        m_followTarget->CharInfo->Zulies += extraZulies;

        BEGINPACKET( pak, 0x71d );
        ADDQWORD( pak, m_followTarget->CharInfo->Zulies );
        m_followTarget->client->SendPacket( &pak );

        char harvestMsg[128];
        snprintf( harvestMsg, sizeof(harvestMsg), "Harvested rare monster essence & hides! Deposited %lld Zulies into squad pouch!", extraZulies );
        Say( harvestMsg );
    }
}

void CPlayerBot::CheckUnionSync( )
{
    if ( !m_isCompanion || !m_player || !m_followTarget || m_followTarget->is_bot ) return;
    clock_t now = clock( );
    if ( ( now - m_lastUnionSyncTime ) < (clock_t)( 20.0f * CLOCKS_PER_SEC ) ) return;
    m_lastUnionSyncTime = now;

    if ( m_followTarget->CharInfo && m_followTarget->CharInfo->unionid != 0 )
    {
        if ( m_player->CharInfo->unionid != m_followTarget->CharInfo->unionid )
        {
            m_player->CharInfo->unionid = m_followTarget->CharInfo->unionid;
            m_player->CharInfo->unionfame = m_followTarget->CharInfo->unionfame;
        }
    }
}

int CPlayerBot::GetStoryChapter( ) const
{
    if ( m_affinity >= 90 ) return 3;
    if ( m_affinity >= 60 ) return 2;
    if ( m_affinity >= 30 ) return 1;
    return 0;
}

const char* CPlayerBot::GetLoyaltyAuraName( ) const
{
    if ( m_affinity < 60 ) return "None";
    const char* name = m_player->CharInfo->charname;
    if ( strcasecmp( name, "Brom" ) == 0 || m_role == BOT_ROLE_TANK ) return "Bastion Aura (+5% DEF)";
    if ( strcasecmp( name, "Lyra" ) == 0 || m_role == BOT_ROLE_HEALER ) return "Grace Aura (+5% HP Recovery)";
    if ( strcasecmp( name, "Ignis" ) == 0 || m_role == BOT_ROLE_DPS_MAGIC ) return "Pyre Aura (+5% Magic Power)";
    if ( strcasecmp( name, "Sylvia" ) == 0 || m_role == BOT_ROLE_DPS_PHYS_RANGED ) return "Gale Aura (+5% Crit & Speed)";
    if ( strcasecmp( name, "Barnaby" ) == 0 ) return "Fortune Aura (+10% Zulies & Salvage)";
    return "Valiant Aura (+5% Attack)";
}

const char* CPlayerBot::GetStoryDialogue( ) const
{
    const char* name = m_player->CharInfo->charname;
    int ch = GetStoryChapter( );

    if ( strcasecmp( name, "Brom" ) == 0 || m_role == BOT_ROLE_TANK )
    {
        if ( ch == 3 ) return "Chapter 3: Soulbound Aegis - 'You are more than a commander, leader; you are family. My shield will never waver!'";
        if ( ch == 2 ) return "Chapter 2: The Shattered Shield - 'We vanquished our foes in the valley. Today, my vows are restored. Bastion Aura awakened!'";
        if ( ch == 1 ) return "Chapter 1: The Iron Vow - 'In the high halls of Junon, I swore never to let an ally fall on my watch.'";
        return "Prologue: 'I am Brom. My shield defends this party with iron will.'";
    }
    if ( strcasecmp( name, "Lyra" ) == 0 || m_role == BOT_ROLE_HEALER )
    {
        if ( ch == 3 ) return "Chapter 3: Soulbound Dawn - 'Where there is shadow, our unity shall bring the morning star!'";
        if ( ch == 2 ) return "Chapter 2: Celestial Dew - 'The sacred herbs of Luna blossom in harmony with your courage. Grace Aura awakened!'";
        if ( ch == 1 ) return "Chapter 1: The Sacred Well - 'The temple elders spoke of a leader with a pure spirit. They guided me to you.'";
        return "Prologue: 'I am Lyra. May the light of the seven planets keep us safe.'";
    }
    if ( strcasecmp( name, "Ignis" ) == 0 || m_role == BOT_ROLE_DPS_MAGIC )
    {
        if ( ch == 3 ) return "Chapter 3: Soulbound Supernova - 'Together, we can pierce the celestial abyss itself!'";
        if ( ch == 2 ) return "Chapter 2: The Inferno Core - 'I have harnessed starstone embers into this catalyst. Pyre Aura awakened!'";
        if ( ch == 1 ) return "Chapter 1: Spark of Arcana - 'Magic is not mere power; it is an untamed hymn of the stars.'";
        return "Prologue: 'I am Ignis. Point me to the enemy and watch them burn.'";
    }
    if ( strcasecmp( name, "Sylvia" ) == 0 || m_role == BOT_ROLE_DPS_PHYS_RANGED )
    {
        if ( ch == 3 ) return "Chapter 3: Soulbound Horizon - 'You never need to look over your shoulder, leader. My arrow flies for you.'";
        if ( ch == 2 ) return "Chapter 2: The Hunter's Focus - 'The wind whispers the quarry's every breath. Gale Aura awakened!'";
        if ( ch == 1 ) return "Chapter 1: Windrunner's Code - 'Keep your bow strung, your eyes sharp, and your footsteps silent.'";
        return "Prologue: 'I am Sylvia. One shot, one clean strike.'";
    }
    if ( strcasecmp( name, "Barnaby" ) == 0 )
    {
        if ( ch == 3 ) return "Chapter 3: Soulbound Ingenuity - 'With you at the helm, there's no machine or dungeon we can't conquer!'";
        if ( ch == 2 ) return "Chapter 2: Masterwork Blueprint - 'Reinforced metallurgy and precision tuning! Fortune Aura awakened!'";
        if ( ch == 1 ) return "Chapter 1: Gears and Zulies - 'Everything in this world has great value if you know how to shape it!'";
        return "Prologue: 'I am Barnaby. Need repairs, ammo, or firepower? I've got the tools!'";
    }

    if ( ch == 3 ) return "Chapter 3: Soulbound Companion - 'Our squad bond is unbreakable! Ready for anything!'";
    if ( ch == 2 ) return "Chapter 2: Proven Ally - 'We have weathered many storms together. Combat Aura awakened!'";
    if ( ch == 1 ) return "Chapter 1: Fresh Adventurer - 'I'm glad to travel by your side, leader!'";
    return "Prologue: 'Ready for the road ahead!'";
}

bool CPlayerBot::PrepareCampfireFeast( )
{
    if ( !m_isCompanion || !m_player || !m_followTarget || m_followTarget->is_bot ) return false;
    clock_t now = clock( );
    if ( ( now - m_lastFeastTime ) < (clock_t)( 60.0f * CLOCKS_PER_SEC ) ) return false;

    m_lastFeastTime = now;
    DoEmote( 3 );

    char msg[128];
    snprintf( msg, sizeof(msg), "Campfire feast prepared! Roasted meat and savory stew served for %s (+15%% EXP & +10%% Combat Stats)!",
        m_followTarget->CharInfo->charname );
    Say( msg );

    if ( m_player->Party && m_player->Party->party )
    {
        CParty* party = m_player->Party->party;
        for ( size_t i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* mem = party->Members[i];
            if ( mem && !mem->IsDead( ) && mem->Position->Map == m_player->Position->Map )
            {
                mem->Stats->Attack_Power += ( mem->Stats->Attack_Power * 10 / 100 );
                mem->Stats->Defense += ( mem->Stats->Defense * 10 / 100 );
                mem->Stats->HP = mem->Stats->MaxHP;
                mem->Stats->MP = mem->Stats->MaxMP;
                mem->RefreshHPMP( );
            }
        }
    }
    return true;
}

void CPlayerBot::ExecuteSquadDance( )
{
    if ( !m_isCompanion || !m_player ) return;
    DoEmote( 6 );
    Say( "Squad dance in sync! Look at these moves, leader!" );
}

void CPlayerBot::ExecuteVictoryCelebration( )
{
    if ( !m_isCompanion || !m_player || !m_followTarget ) return;
    DoEmote( 3 );
    char victoryShout[128];
    snprintf( victoryShout, sizeof(victoryShout), "Victory is ours! Another glorious triumph with leader %s!",
        m_followTarget->CharInfo->charname );
    Say( victoryShout );
}

BotRole CBotManager::GetPlayerRole( CPlayer* player )
{
    if ( !player || !player->CharInfo ) return BOT_ROLE_GENERAL_VISITOR;
    int job = player->CharInfo->Job;

    if ( job == 121 ) return BOT_ROLE_TANK; // Knight
    if ( job == 111 ) // Soldier
    {
        // If holding a shield, treat as Tank
        if ( player->items[8].itemtype == SUBWEAPON && player->items[8].itemnum > 0 )
        {
            return BOT_ROLE_TANK;
        }
        return BOT_ROLE_DPS_PHYS_MELEE;
    }
    if ( job == 122 ) return BOT_ROLE_DPS_PHYS_MELEE; // Champion
    if ( job == 222 ) return BOT_ROLE_HEALER; // Cleric
    if ( job == 211 ) return BOT_ROLE_HEALER; // Muse
    if ( job == 221 ) return BOT_ROLE_DPS_MAGIC; // Mage
    if ( job == 322 || job == 311 ) return BOT_ROLE_DPS_PHYS_RANGED; // Scout / Hawker
    if ( job == 321 ) return BOT_ROLE_DPS_PHYS_MELEE; // Raider
    if ( job == 421 || job == 411 ) return BOT_ROLE_DPS_PHYS_RANGED; // Bourgeois / Dealer
    if ( job == 422 ) return BOT_ROLE_DPS_PHYS_MELEE; // Artisan
    return BOT_ROLE_GENERAL_VISITOR;
}

void CBotManager::SendPartyFullUpdate( CParty* party )
{
    if ( !party ) return;

    for ( size_t i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* member = party->Members[i];
        if ( member && !member->is_bot && member->client )
        {
            BEGINPACKET( pak, 0x7d4 );
            ADDBYTE    ( pak, party->PartyLevel );
            ADDDWORD   ( pak, party->Exp );
            member->client->SendPacket( &pak );

            RESETPACKET( pak, 0x7d2 );
            ADDBYTE    ( pak, party->Option );
            ADDBYTE    ( pak, (BYTE)party->Members.size( ) );
            for ( size_t m = 0; m < party->Members.size( ); m++ )
            {
                CPlayer* pm = party->Members[m];
                if ( !pm || !pm->CharInfo || !pm->Stats ) continue;
                ADDDWORD   ( pak, pm->CharInfo->charid );
                ADDWORD    ( pak, pm->clientid );
                ADDWORD    ( pak, (WORD)pm->Stats->MaxHP );
                ADDWORD    ( pak, (WORD)pm->Stats->HP );
                ADDDWORD   ( pak, GServer->BuildBuffs( pm ) );
                ADDDWORD   ( pak, 0x1f40008c );
                ADDWORD    ( pak, 0x1388 );
                ADDSTRING  ( pak, pm->CharInfo->charname );
                ADDBYTE    ( pak, 0x00 );
            }
            member->client->SendPacket( &pak );
        }
    }
}

CPlayer* CBotManager::SpawnCompanionBot( const char* name, BotRole role, int level, int mapId, fPoint pos, CPlayer* leader )
{
    int job = 0;
    if ( level < 10 )
    {
        job = 0; // Visitor
    }
    else
    {
        switch ( role )
        {
            case BOT_ROLE_TANK:
                job = ( level >= 70 ) ? 121 : 111;
                break;
            case BOT_ROLE_HEALER:
                job = ( level >= 70 ) ? 222 : 211;
                break;
            case BOT_ROLE_DPS_MAGIC:
                job = ( level >= 70 ) ? 221 : 211;
                break;
            case BOT_ROLE_DPS_PHYS_RANGED:
                job = ( level >= 70 ) ? 322 : 311;
                break;
            case BOT_ROLE_DPS_PHYS_MELEE:
                job = ( level >= 70 ) ? 122 : 111;
                break;
            default:
                job = 0;
                break;
        }
    }

    CPlayer* bot = SpawnBot( name, job, level, mapId, pos, false );
    if ( bot && bot->bot_ai )
    {
        CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( bot->bot_ai );
        botAi->SetCompanion( true, leader );
        botAi->SetRole( role );
        botAi->SetFollowTarget( leader );
        botAi->SetState( BOT_STATE_FOLLOW );
        botAi->SyncLevelToLeader( level );
    }
    return bot;
}

void CBotManager::EnsurePlayerCompanionParty( CPlayer* realPlayer )
{
    if ( !realPlayer || realPlayer->is_bot || !realPlayer->Session || !realPlayer->Session->inGame ) return;

    // 1. Ensure realPlayer has an active party
    CParty* party = realPlayer->Party->party;
    if ( party == NULL )
    {
        party = new CParty;
        party->AddPlayer( realPlayer );
        GServer->AddParty( party );
        realPlayer->Party->IsMaster = true;
    }

    // Party capacity check (Target: 4 members total = 1 player + 3 companion bots)
    party->RefreshMax( );
    size_t targetMembers = ( party->Capacity > 4 ) ? 4 : party->Capacity;
    if ( targetMembers < 4 ) targetMembers = 4;

    // Count existing members
    size_t currentMembers = party->Members.size( );
    if ( currentMembers >= targetMembers )
    {
        // Party already full: ensure all companion bots are level-synced with the player
        for ( size_t m = 0; m < party->Members.size( ); m++ )
        {
            CPlayer* mem = party->Members[m];
            if ( mem && mem->is_bot && mem->bot_ai )
            {
                CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( mem->bot_ai );
                botAi->SetCompanion( true, realPlayer );
                botAi->SyncLevelToLeader( realPlayer->Stats->Level );
            }
        }
        SendPartyFullUpdate( party );
        return;
    }

    // Identify roles currently filled in the party
    bool hasTank = false;
    bool hasHealer = false;
    int dpsCount = 0;

    for ( size_t m = 0; m < party->Members.size( ); m++ )
    {
        CPlayer* member = party->Members[m];
        if ( !member ) continue;
        BotRole r = GetPlayerRole( member );
        if ( r == BOT_ROLE_TANK ) hasTank = true;
        else if ( r == BOT_ROLE_HEALER ) hasHealer = true;
        else if ( r == BOT_ROLE_DPS_PHYS_MELEE || r == BOT_ROLE_DPS_PHYS_RANGED || r == BOT_ROLE_DPS_MAGIC ) dpsCount++;
    }

    BotRole playerRole = GetPlayerRole( realPlayer );
    int playerLevel = realPlayer->Stats->Level;
    UINT mapId = ( realPlayer->Position != NULL ) ? realPlayer->Position->Map : 1;
    fPoint basePos = ( realPlayer->Position != NULL ) ? realPlayer->Position->current : fPoint{ 5200.0f, 5200.0f, 0.0f };

    // Select complementary companion roles needed to reach target members
    std::vector<BotRole> neededRoles;
    while ( ( party->Members.size( ) + neededRoles.size( ) ) < targetMembers )
    {
        if ( !hasTank )
        {
            neededRoles.push_back( BOT_ROLE_TANK );
            hasTank = true;
        }
        else if ( !hasHealer )
        {
            neededRoles.push_back( BOT_ROLE_HEALER );
            hasHealer = true;
        }
        else
        {
            if ( dpsCount % 2 == 0 )
            {
                neededRoles.push_back( ( playerRole == BOT_ROLE_DPS_MAGIC ) ? BOT_ROLE_DPS_PHYS_RANGED : BOT_ROLE_DPS_MAGIC );
            }
            else
            {
                neededRoles.push_back( BOT_ROLE_DPS_PHYS_MELEE );
            }
            dpsCount++;
        }
    }

    // Spawn companion bots for each needed role
    for ( size_t r = 0; r < neededRoles.size( ); r++ )
    {
        BotRole role = neededRoles[r];
        std::string botName = GenerateUniqueName( );

        fPoint spawnPos = basePos;
        spawnPos.x += ( ( rand( ) % 40 ) - 20 ) / 10.0f;
        spawnPos.y += ( ( rand( ) % 40 ) - 20 ) / 10.0f;

        CPlayer* companionBot = SpawnCompanionBot( botName.c_str( ), role, playerLevel, mapId, spawnPos, realPlayer );
        if ( companionBot )
        {
            party->AddPlayer( companionBot );
            companionBot->Party->IsMaster = false;

            // Spawn 3D entity models on leader's screen & add to VisiblePlayers
            companionBot->RespawnToPlayer( realPlayer );
            realPlayer->RespawnToPlayer( companionBot );

            bool foundV = false;
            for ( size_t v = 0; v < realPlayer->VisiblePlayers.size(); v++ )
            {
                if ( realPlayer->VisiblePlayers[v] == companionBot ) { foundV = true; break; }
            }
            if ( !foundV ) realPlayer->VisiblePlayers.push_back( companionBot );

            if ( companionBot->bot_ai )
            {
                CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( companionBot->bot_ai );
                botAi->SetFollowTarget( realPlayer );
                botAi->SetState( BOT_STATE_FOLLOW );

                const char* roleGreeting = "I'm with you, leader! Let's hunt!";
                if ( playerLevel < 10 )
                {
                    if ( role == BOT_ROLE_TANK ) roleGreeting = "Ready with sword and shield! Let's level up together, leader!";
                    else if ( role == BOT_ROLE_HEALER ) roleGreeting = "I'll support and watch your back! Let's train to level 10!";
                    else roleGreeting = "Ready to hunt monsters with you, leader!";
                }
                else
                {
                    if ( role == BOT_ROLE_TANK ) roleGreeting = "Shield up! I will protect you and hold aggro, leader!";
                    else if ( role == BOT_ROLE_HEALER ) roleGreeting = "Divine blessings upon you! Your health is my priority!";
                    else if ( role == BOT_ROLE_DPS_MAGIC ) roleGreeting = "Arcane energies ready! I will vaporize our enemies!";
                    else if ( role == BOT_ROLE_DPS_PHYS_RANGED ) roleGreeting = "Quiver full, bow strung! Focusing on your targets!";
                    else if ( role == BOT_ROLE_DPS_PHYS_MELEE ) roleGreeting = "Blades drawn! Ready for combat!";
                }

                botAi->Say( roleGreeting );
                if ( playerLevel >= 20 )
                {
                    botAi->ForcePartyBuff( );
                }
            }
        }
    }

    SendPartyFullUpdate( party );
    Log( MSG_INFO, "[PlayerBot] Ensured companion party for player '%s' (Total Members: %lu)",
         realPlayer->CharInfo->charname, party->Members.size( ) );
}

void CBotManager::CheckPartyRoleBalance( CPlayer* realPlayer )
{
    if ( !realPlayer || realPlayer->is_bot || !realPlayer->Party || !realPlayer->Party->party ) return;
    CParty* party = realPlayer->Party->party;

    BotRole playerRole = GetPlayerRole( realPlayer );
    if ( playerRole == BOT_ROLE_GENERAL_VISITOR ) return;

    int tankCount = ( playerRole == BOT_ROLE_TANK ) ? 1 : 0;
    int healerCount = ( playerRole == BOT_ROLE_HEALER ) ? 1 : 0;
    int dpsCount = ( playerRole != BOT_ROLE_TANK && playerRole != BOT_ROLE_HEALER ) ? 1 : 0;

    for ( size_t m = 0; m < party->Members.size( ); m++ )
    {
        CPlayer* member = party->Members[m];
        if ( !member || !member->is_bot || !member->bot_ai ) continue;
        CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( member->bot_ai );
        if ( !botAi->IsCompanion( ) ) continue;

        BotRole r = botAi->GetRole( );

        // If player is a Tank and bot is also a Tank -> respec bot to Healer or DPS
        if ( playerRole == BOT_ROLE_TANK && r == BOT_ROLE_TANK )
        {
            BotRole newRole = ( healerCount == 0 ) ? BOT_ROLE_HEALER : BOT_ROLE_DPS_MAGIC;
            botAi->AdjustSpecToRole( newRole, realPlayer->Stats->Level );
            botAi->Say( "I see you're tanking now, leader! I've switched my spec to support you!" );
            if ( newRole == BOT_ROLE_HEALER ) healerCount++; else dpsCount++;
        }
        // If player is a Healer and bot is also a Healer -> respec bot to Tank or DPS
        else if ( playerRole == BOT_ROLE_HEALER && r == BOT_ROLE_HEALER )
        {
            BotRole newRole = ( tankCount == 0 ) ? BOT_ROLE_TANK : BOT_ROLE_DPS_MAGIC;
            botAi->AdjustSpecToRole( newRole, realPlayer->Stats->Level );
            botAi->Say( "I see you're healing now, leader! I've switched my spec to frontline defense!" );
            if ( newRole == BOT_ROLE_TANK ) tankCount++; else dpsCount++;
        }
    }
}

void CBotManager::RefillPartyCompanions( CPlayer* realPlayer )
{
    EnsurePlayerCompanionParty( realPlayer );
}

void CBotManager::CheckPlayerCompanions( )
{
    clock_t now = clock( );
    if ( ( now - m_lastCompanionCheck ) < ( 5 * CLOCKS_PER_SEC ) ) return;
    m_lastCompanionCheck = now;

    // Check all active real players on all maps
    for ( int m = 0; m < GServer->MapList.max; m++ )
    {
        CMap* map = GServer->MapList.Index[m];
        if ( !map || map == GServer->MapList.nullzone ) continue;

        for ( UINT p = 0; p < map->PlayerList.size( ); p++ )
        {
            CPlayer* player = map->PlayerList[p];
            if ( !player || player->is_bot || !player->Session || !player->Session->inGame ) continue;

            // Check if player's party needs role rebalancing (e.g. after job change)
            CheckPartyRoleBalance( player );

            // If player is in party and has companions, ensure all companions are on the same map and level
            if ( player->Party && player->Party->party )
            {
                CParty* party = player->Party->party;
                for ( size_t i = 0; i < party->Members.size( ); i++ )
                {
                    CPlayer* mem = party->Members[i];
                    if ( mem && mem->is_bot && mem->bot_ai )
                    {
                        CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( mem->bot_ai );
                        botAi->SyncLevelToLeader( player->Stats->Level );
                    }
                }
            }
        }
    }
}

bool CBotManager::RecruitMercenary( CPlayer* leader, const char* mercName )
{
    if ( !leader || leader->is_bot || !mercName ) return false;

    BotRole role = BOT_ROLE_DPS_PHYS_MELEE;
    int job = 121;
    if ( strcasecmp( mercName, "Brom" ) == 0 )
    {
        role = BOT_ROLE_TANK;
        job = 121; // Knight
    }
    else if ( strcasecmp( mercName, "Lyra" ) == 0 )
    {
        role = BOT_ROLE_HEALER;
        job = 222; // Cleric
    }
    else if ( strcasecmp( mercName, "Ignis" ) == 0 )
    {
        role = BOT_ROLE_DPS_MAGIC;
        job = 221; // Mage
    }
    else if ( strcasecmp( mercName, "Sylvia" ) == 0 )
    {
        role = BOT_ROLE_DPS_PHYS_RANGED;
        job = 322; // Scout
    }
    else if ( strcasecmp( mercName, "Barnaby" ) == 0 )
    {
        role = BOT_ROLE_DPS_PHYS_RANGED;
        job = 422; // Bourgeois / Artisan
    }
    else
    {
        return false;
    }

    if ( !leader->Party || !leader->Party->party )
    {
        EnsurePlayerCompanionParty( leader );
    }
    CParty* party = leader->Party->party;
    if ( !party ) return false;

    // Check if mercenary is already in party
    for ( size_t i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* mem = party->Members[i];
        if ( mem && strcasecmp( mem->CharInfo->charname, mercName ) == 0 )
        {
            return false;
        }
    }

    // If party is full (4 members), dismiss companion matching role or non-leader
    if ( party->Members.size( ) >= 4 )
    {
        CPlayer* toDismiss = NULL;
        for ( size_t i = 0; i < party->Members.size( ); i++ )
        {
            CPlayer* mem = party->Members[i];
            if ( mem && mem->is_bot && mem->bot_ai )
            {
                CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( mem->bot_ai );
                if ( botAi->GetRole( ) == role )
                {
                    toDismiss = mem;
                    break;
                }
                if ( !toDismiss ) toDismiss = mem;
            }
        }

        if ( toDismiss )
        {
            party->RemovePlayer( toDismiss );
            toDismiss->Party->party = NULL;
            RemoveBot( toDismiss->CharInfo->charname );
        }
    }

    // Spawn the mercenary
    fPoint spawnPos = leader->Position->current;
    spawnPos.x += ( ( rand( ) % 6 ) - 3 );
    spawnPos.y += ( ( rand( ) % 6 ) - 3 );

    CPlayer* bot = SpawnBot( mercName, job, leader->Stats->Level, leader->Position->Map, spawnPos, false );
    if ( bot && bot->bot_ai )
    {
        CPlayerBot* botAi = reinterpret_cast<CPlayerBot*>( bot->bot_ai );
        botAi->SetCompanion( true, leader );
        botAi->SetRole( role );
        botAi->SetFollowTarget( leader );
        botAi->SetState( BOT_STATE_FOLLOW );
        botAi->SyncLevelToLeader( leader->Stats->Level );
        botAi->SetAffinity( 80 ); // Seasoned veteran initial trust

        party->AddPlayer( bot );
        bot->Party->party = party;
        SendPartyFullUpdate( party );

        char intro[128];
        snprintf( intro, sizeof(intro), "Veteran Mercenary %s reporting for duty! Lead the way, %s!",
            mercName, leader->CharInfo->charname );
        botAi->Say( intro );
        return true;
    }
    return false;
}

