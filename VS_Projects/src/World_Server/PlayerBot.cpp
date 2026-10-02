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
      m_roamRadius( 100.0f ),
      m_isBuffBot( false ),
      m_bonfireCid( 0 ),
      m_lastBuffSay( 0 ),
      m_lastBuffCastTime( 0 ),
      m_lastBuffSeekTime( 0 ),
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
      m_personality( (BotPersonality)( rand( ) % 4 ) ),
      m_lastShopBrowseTime( 0 ),
      m_lastWhisperTime( 0 ),
      m_lastArenaQueueTime( 0 ),
      m_lastDungeonCheckTime( 0 )
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
        m_strollDest = m_roamCenter;
        AssignClanTag( );
        AssignTitle( );
        ApplyRandomCosmetics( );
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
        default:                   return "UNKNOWN";
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

            SayChatter( "low_hp" );
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
    if ( !m_player || m_isBuffBot || m_isVendingBot || m_followTarget ) return;
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
            m_roamRadius = 100.0f;
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

bool CPlayerBot::CheckPartyBuffs( )
{
    if ( !m_player || !m_player->Party || !m_player->Party->party ) return false;
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 30 ) return false;

    int job = m_player->CharInfo->Job;
    if ( job != 211 && job != 221 && job != 222 ) return false; // Muse / Mage / Cleric only

    clock_t now = clock( );
    if ( ( now - m_lastPartyBuffTime ) < (clock_t)( 3.5f * CLOCKS_PER_SEC ) ) return false;

    CParty* party = m_player->Party->party;
    CPlayer* targetToBuff = NULL;
    UINT skillToCast = 0;
    const char* buffName = NULL;

    for ( UINT i = 0; i < party->Members.size( ); i++ )
    {
        CPlayer* member = party->Members[i];
        if ( !member || member->IsDead( ) || member->Position->Map != m_player->Position->Map ) continue;
        if ( GServer->distance( m_player->Position->current, member->Position->current ) > 22.0f ) continue;

        // Check for missing buffs in priority order
        if ( member->Status->Dash_up == 0xff )
        {
            targetToBuff = member;
            skillToCast = 930; // Hustle Charm Lv 5 (Move speed)
            buffName = "Hustle";
            break;
        }
        else if ( member->Status->Attack_up == 0xff )
        {
            targetToBuff = member;
            skillToCast = 1270; // Clobber Charm Lv 5 (Attack power)
            buffName = "Clobber Charm";
            break;
        }
        else if ( member->Status->Defense_up == 0xff )
        {
            targetToBuff = member;
            skillToCast = 1004; // Resilience Charm Lv 9 (Defense)
            buffName = "Resilience";
            break;
        }
        else if ( member->Status->Haste_up == 0xff )
        {
            targetToBuff = member;
            skillToCast = 1254; // Battle Charm Lv 9 (Attack speed)
            buffName = "Battle Charm";
            break;
        }
        else if ( member->Status->Accuracy_up == 0xff )
        {
            targetToBuff = member;
            skillToCast = 1019; // Precision Charm Lv 9 (Accuracy)
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

    // Only Muse (211), Mage (221), Cleric (222) can perform proximity buffing
    int job = m_player->CharInfo->Job;
    if ( job != 211 && job != 221 && job != 222 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastPartyBuffTime ) < (clock_t)( 5.0f * CLOCKS_PER_SEC ) ) return false;

    // Roll d20: 14 or higher required (~35% chance to buff a passing stranger)
    if ( RollD20( ) < 14 ) return false;

    CMap* map = GetMap( );
    if ( !map ) return false;

    CPlayer* targetToBuff = NULL;
    UINT skillToCast = 0;
    const char* buffName = NULL;

    for ( size_t i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( !p || p == m_player || p->IsDead( ) ) continue;
        if ( p->Session == NULL || !p->Session->inGame ) continue;
        if ( p->bot_ai != NULL ) continue; // Target real players

        // 1. MUST BE STANDING STILL (do not run after moving players)
        if ( p->IsMoving( ) ) continue;

        // 2. MUST BE WITHIN SPELL CAST RANGE (max 15.0m - no chasing!)
        float dist = GServer->distance( m_player->Position->current, p->Position->current );
        if ( dist > 15.0f ) continue;

        // 3. Priority checks for missing buffs or healing
        if ( p->Stats->HP < ( p->Stats->MaxHP * 85 / 100 ) )
        {
            targetToBuff = p;
            skillToCast = 985; // Recovery
            buffName = "Heal";
            break;
        }
        else if ( p->Status->Dash_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 930; // Hustle Charm (Move Speed)
            buffName = "Hustle";
            break;
        }
        else if ( p->Status->Haste_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 1254; // Battle Charm (Attack Speed)
            buffName = "Battle Charm";
            break;
        }
        else if ( p->Status->Attack_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 1270; // Clobber Charm (Attack Power)
            buffName = "Clobber Charm";
            break;
        }
        else if ( p->Status->Defense_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 1004; // Resilience Charm (Defense)
            buffName = "Resilience";
            break;
        }
        else if ( p->Status->Accuracy_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 1019; // Precision Charm (Accuracy)
            buffName = "Precision";
            break;
        }
        else if ( p->Status->Critical_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 1284; // Critical Charm (Crit Rate)
            buffName = "Critical Charm";
            break;
        }
        else if ( p->Status->ExtraDamage_up == 0xff )
        {
            targetToBuff = p;
            skillToCast = 1294; // Valkyrie Charm (Extra Damage)
            buffName = "Valkyrie Charm";
            break;
        }
    }

    if ( targetToBuff && skillToCast > 0 )
    {
        CSkills* sk = GServer->GetSkillByID( skillToCast );
        if ( sk && m_player->IsTargetReached( targetToBuff, sk ) )
        {
            // Cast buff from current position without chasing or moving towards the player
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
    if ( !m_player->Status->CanCastSkill || m_player->Stats->MP < 60 ) return false;

    int job = m_player->CharInfo->Job;
    if ( job != 211 && job != 221 && job != 222 ) return false; // Muse / Mage / Cleric
    if ( m_player->Stats->Level < 30 ) return false;

    clock_t now = clock( );
    if ( ( now - m_lastResurrectTime ) < (clock_t)( 6.0f * CLOCKS_PER_SEC ) ) return false;

    CParty* party = m_player->Party->party;
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
            member->RefreshBuff( );
            member->SetStats( );

            // Broadcast HP packet (0x79f)
            BEGINPACKET( pak, 0x79f );
            ADDWORD    ( pak, member->clientid );
            ADDDWORD   ( pak, (DWORD)member->Stats->HP );
            GServer->SendToVisible( &pak, member );

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
    if ( job != 111 && job != 121 && job != 122 ) return false; // Soldier / Knight / Champion only

    clock_t now = clock( );
    if ( ( now - m_lastTauntTime ) < (clock_t)( 4.0f * CLOCKS_PER_SEC ) ) return false;

    CMap* map = GetMap( );
    if ( !map ) return false;

    CParty* party = m_player->Party->party;
    for ( UINT i = 0; i < map->MonsterList.size( ); i++ )
    {
        CMonster* mob = map->MonsterList[i];
        if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) ) continue;
        if ( !mob->Battle || mob->Battle->target == 0 || mob->Battle->target == m_player->clientid ) continue;

        // Check if mob is targeting a party member
        for ( UINT p = 0; p < party->Members.size( ); p++ )
        {
            CPlayer* member = party->Members[p];
            if ( member && member != m_player && member->clientid == mob->Battle->target )
            {
                float dist = GServer->distance( m_player->Position->current, mob->Position->current );
                if ( dist <= 20.0f )
                {
                    m_lastTauntTime = now;
                    AttackTarget( mob );
                    SetState( BOT_STATE_COMBAT );

                    if ( m_player->Stats->Level >= 20 && m_player->Stats->MP >= 20 )
                    {
                        m_player->StartAction( mob, SKILL_ATTACK, 261 );
                    }

                    char tauntMsg[64];
                    snprintf( tauntMsg, sizeof(tauntMsg), "Back off! Focus on me, monster!" );
                    Say( tauntMsg );
                    return true;
                }
            }
        }
    }
    return false;
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

    float relAngle = 3.14159265f;
    float dist = 3.2f;

    switch ( slotIndex % 6 )
    {
        case 0: relAngle = 3.14159265f + 0.65f; dist = 3.0f; break; // Behind-Right
        case 1: relAngle = 3.14159265f - 0.65f; dist = 3.0f; break; // Behind-Left
        case 2: relAngle = 3.14159265f;         dist = 4.5f; break; // Behind-Center
        case 3: relAngle = 1.5707963f;          dist = 2.8f; break; // Right-Flank
        case 4: relAngle = -1.5707963f;         dist = 2.8f; break; // Left-Flank
        default: relAngle = 3.14159265f + 0.35f; dist = 3.5f; break;
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

    // Phase 2: Autonomous party invitations check
    CheckPartyInvitations( );

    // Phase 3: Zone migration check
    CheckZoneMigration( );

    // Player Greetings: Wave and greet nearby real players
    CheckPlayerGreetings( );

    // Autonomous Player Shop Browsing & Economy Purchases
    CheckPlayerShops( );

    // Private Message Whispers & Social Interaction
    CheckWhispers( );

    // Akram Arena PvP Battleground Participation
    CheckArenaQueue( );

    // Dungeon Crawling & Raids
    CheckDungeonRuns( );

    // Phase 4 Realism: LFG Shouts & World Boss Raids
    CheckLfgShouts( );
    CheckWorldBossRaids( );

    // Proximity buffing: buff nearby unbuffed stationary real players (without chasing/moving toward them)
    if ( CheckProximityBuffs( ) ) return;

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
    CDrop* drop = FindNearbyDrop( 60.0f );
    if ( drop )
    {
        m_targetDropCid = drop->clientid;
        MoveTo( drop->pos );
        SetState( BOT_STATE_LOOT );
        return;
    }

    // 3. Look for nearby monsters to attack (greatly expanded aggro detection radius)
    CMonster* mob = FindNearbyMonster( 90.0f );
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
    CDrop* drop = FindNearbyDrop( 50.0f );
    if ( drop )
    {
        m_targetDropCid = drop->clientid;
        MoveTo( drop->pos );
        SetState( BOT_STATE_LOOT );
        return;
    }

    // Check for monsters while roaming (expanded detection radius)
    CMonster* mob = FindNearbyMonster( 85.0f );
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

    // Check party support during combat
    CheckPartyHeal( );
    CheckPartyResurrect( );
    CheckPartyBuffs( );

    CMonster* mob = map->GetMonsterInMap( m_targetMobCid );
    if ( !mob || mob->IsDead( ) || mob->Stats->HP <= 0 || mob->IsBonfire( ) )
    {
        ClearBattle( m_player->Battle );
        m_targetMobCid = 0;

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

    // Target is valid: check attack range
    float dist = GServer->distance( m_player->Position->current, mob->Position->current );

    // Ranged kiting AI check
    KiteTarget( mob );

    // If target is too far away, drop combat
    if ( dist > 100.0f )
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

    // 2. Check if party members need resurrecting
    if ( CheckPartyResurrect( ) ) return;

    // 3. Check if party members need buffing
    if ( CheckPartyBuffs( ) ) return;

    // 4. Tank aggro defense
    if ( CheckPartyTaunt( ) ) return;

    // 5. Assist leader: check if leader is attacking a monster
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

    // 6. Defend leader: check if any monster is attacking leader
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

    // 7. Tactical formation follow (and cross-map teleportation)
    if ( m_player->Position->Map != m_followTarget->Position->Map )
    {
        CMap* targetMap = GServer->MapList.Index[m_followTarget->Position->Map];
        if ( targetMap != NULL )
        {
            targetMap->TeleportPlayer( m_player, m_followTarget->Position->current, false );
            return;
        }
    }

    int slot = GetPartySlotIndex( );
    fPoint destPos = GetFormationOffset( slot, m_followTarget->Position->current, m_followTarget->Position->destiny );

    float dist = GServer->distance( m_player->Position->current, destPos );
    if ( dist > 55.0f )
    {
        map->TeleportPlayer( m_player, destPos, false );
    }
    else if ( dist > 2.0f )
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
    if ( ( now - m_stateTimer ) >= ( 5 * CLOCKS_PER_SEC ) )
    {
        Respawn( );
    }
}

void CPlayerBot::HandleTownStroll( )
{
    if ( !m_player ) return;

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

            if ( rand( ) % 2 == 0 )
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
    if ( ( now - m_lastLfgShoutTime ) < ( 90 * CLOCKS_PER_SEC ) ) return;
    m_lastLfgShoutTime = now;

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
        if ( m_player->Party->party == NULL || m_player->Party->party->Members.size() < 4 )
        {
            snprintf( reply, sizeof(reply), "Sure! Target me and click twice to invite me to your party!" );
        }
        else
        {
            snprintf( reply, sizeof(reply), "Sorry, my party is full right now! Catch you on the next run." );
        }
    }
    else if ( text.find( "duel" ) != std::string::npos || text.find( "fight" ) != std::string::npos || text.find( "pvp" ) != std::string::npos )
    {
        snprintf( reply, sizeof(reply), "You want to fight? Type '/duel %s' and let's go!", m_player->CharInfo->charname );
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
            if ( p->IsMoving( ) ) continue; // Require player to stand still
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
                if ( p->IsMoving( ) ) continue; // Require bot to stand still
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

void CPlayerBot::SetupVendingShop( const char* shopTitle, int category )
{
    if ( !m_player || !m_player->Shop ) return;

    m_vendingCategory = category;
    m_shopTitle = shopTitle ? shopTitle : "Shop";

    strncpy( m_player->Shop->name, shopTitle ? shopTitle : "Shop", sizeof(m_player->Shop->name) - 1 );
    m_player->Shop->name[sizeof(m_player->Shop->name) - 1] = '\0';
    m_player->Shop->ShopType = 0; // Selling shop

    PopulateVendingInventory( m_player, category );

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
    if ( ( now - m_lastWhisperTime ) < (clock_t)( 180 * CLOCKS_PER_SEC ) ) return;

    CMap* map = GetMap( );
    if ( !map ) return;

    for ( UINT i = 0; i < map->PlayerList.size( ); i++ )
    {
        CPlayer* p = map->PlayerList[i];
        if ( p && !p->is_bot && p->Session && p->Session->inGame )
        {
            m_lastWhisperTime = now;
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
