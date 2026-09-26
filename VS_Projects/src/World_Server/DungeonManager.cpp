#include "DungeonManager.h"
#include <cstdio>
#include <cstring>

CDungeonManager* CDungeonManager::m_instance = NULL;

CDungeonManager* CDungeonManager::GetInstance()
{
    if (!m_instance)
    {
        m_instance = new CDungeonManager();
    }
    return m_instance;
}

CDungeonManager::CDungeonManager()
{
    pthread_mutex_init(&m_dungeonMutex, NULL);
    m_activeInstances.clear();
    m_partyInstances.clear();
}

CDungeonManager::~CDungeonManager()
{
    pthread_mutex_destroy(&m_dungeonMutex);
}

bool CDungeonManager::IsDungeonZone(UINT mapId)
{
    if (mapId < MAX_ZONE_INSTANCES && GServer->MapList.Index[mapId] && GServer->MapList.Index[mapId]->is_instance)
    {
        mapId = GServer->MapList.Index[mapId]->base_zone;
    }

    // Goblin Cave: 24, 25, 26, 27, 28
    if (mapId >= 24 && mapId <= 28) return true;
    // George's Cave: 31, 32, 33
    if (mapId >= 31 && mapId <= 33) return true;
    // Oblivion Temple / Sea of Seeds: 41, 42, 43, 44
    if (mapId >= 41 && mapId <= 44) return true;
    // Forgotten Temple: 56, 57
    if (mapId >= 56 && mapId <= 57) return true;

    return false;
}

UINT CDungeonManager::GetBaseDungeonZone(UINT mapId)
{
    if (mapId < MAX_ZONE_INSTANCES && GServer->MapList.Index[mapId] && GServer->MapList.Index[mapId]->is_instance)
    {
        return GServer->MapList.Index[mapId]->base_zone;
    }
    return mapId;
}

bool CDungeonManager::GetDungeonExit(UINT baseZone, UINT& exitMap, fPoint& exitPos)
{
    if (baseZone >= 31 && baseZone <= 33)
    {
        // George's Cave -> Valley of Lux (map 22)
        exitMap = 22;
        exitPos.x = 5174.16f;
        exitPos.y = 5132.11f;
        return true;
    }
    else if (baseZone >= 24 && baseZone <= 28)
    {
        // Goblin Cave -> Adventurer's Plains (map 1)
        exitMap = 1;
        exitPos.x = 5279.88f;
        exitPos.y = 5493.08f;
        return true;
    }
    else if (baseZone >= 41 && baseZone <= 44)
    {
        // Oblivion Temple -> Goblin Cave B1 (map 24)
        exitMap = 24;
        exitPos.x = 5488.20f;
        exitPos.y = 5223.60f;
        return true;
    }
    else if (baseZone >= 56 && baseZone <= 57)
    {
        // Forgotten Temple -> Xita Refuge (map 54)
        exitMap = 54;
        exitPos.x = 5114.37f;
        exitPos.y = 4250.87f;
        return true;
    }

    // Default fallback to Zant (map 2)
    exitMap = 2;
    exitPos.x = 5654.0f;
    exitPos.y = 5216.0f;
    return false;
}

CMap* CDungeonManager::GetOrCreatePartyInstance(CPlayer* player, UINT baseMapId)
{
    if (!player || !IsDungeonZone(baseMapId)) return NULL;

    baseMapId = GetBaseDungeonZone(baseMapId);

    UINT partyId = 0;
    if (player->Party && player->Party->party && player->Party->party->PartyId > 0)
    {
        partyId = player->Party->party->PartyId;
    }
    else
    {
        // Solo player assigned a private partition using high-bit tagged charid
        partyId = 0x80000000 | player->CharInfo->charid;
    }

    pthread_mutex_lock(&m_dungeonMutex);
    auto it = m_partyInstances.find(std::make_pair(baseMapId, partyId));
    if (it != m_partyInstances.end())
    {
        UINT instId = it->second;
        if (instId < MAX_ZONE_INSTANCES && GServer->MapList.Index[instId] && GServer->MapList.Index[instId]->is_instance)
        {
            CMap* existing = GServer->MapList.Index[instId];
            pthread_mutex_unlock(&m_dungeonMutex);
            return existing;
        }
    }
    pthread_mutex_unlock(&m_dungeonMutex);

    return CreateInstance(baseMapId, partyId);
}

CMap* CDungeonManager::CreateInstance(UINT baseMapId, UINT partyId)
{
    if (baseMapId >= (UINT)GServer->maxZone) return NULL;
    CMap* baseMap = GServer->MapList.Index[baseMapId];
    if (!baseMap || baseMap == GServer->MapList.nullzone) return NULL;

    pthread_mutex_lock(&m_dungeonMutex);

    // Find free dynamic slot in range [200, MAX_ZONE_INSTANCES)
    UINT newId = 0;
    for (UINT id = 200; id < MAX_ZONE_INSTANCES; id++)
    {
        if (GServer->MapList.Index[id] == GServer->MapList.nullzone && m_activeInstances.find(id) == m_activeInstances.end())
        {
            newId = id;
            break;
        }
    }

    if (newId == 0)
    {
        Log(MSG_ERROR, "CDungeonManager::CreateInstance: No free instance slots available!");
        pthread_mutex_unlock(&m_dungeonMutex);
        return NULL;
    }

    CMap* newMap = new CMap();
    newMap->id = newId;
    newMap->base_zone = baseMapId;
    newMap->instance_id = newId;
    newMap->party_id = partyId;
    newMap->creation_time = clock();
    newMap->time_limit_sec = 1800; // 30 minutes
    newMap->is_instance = true;
    newMap->is_wiped = false;
    newMap->empty_since = 0;
    newMap->warn_10m_sent = false;
    newMap->warn_5m_sent = false;
    newMap->warn_1m_sent = false;

    // Inherit base map environmental parameters
    newMap->dayperiod = baseMap->dayperiod;
    newMap->morningtime = baseMap->morningtime;
    newMap->daytime = baseMap->daytime;
    newMap->eveningtime = baseMap->eveningtime;
    newMap->nighttime = baseMap->nighttime;
    newMap->allowpvp = baseMap->allowpvp;
    newMap->pvp_mode = baseMap->pvp_mode;
    newMap->allowpat = baseMap->allowpat;
    newMap->STLID = baseMap->STLID;
    newMap->ZoneTime = baseMap->ZoneTime;
    newMap->MapTime = baseMap->MapTime;
    newMap->CurrentTime = baseMap->CurrentTime;

    // Shallow copy shared navigation structures (they are not modified by instances)
    newMap->RespawnList = baseMap->RespawnList;
    newMap->TeleGateList = baseMap->TeleGateList;
    newMap->NPCList = baseMap->NPCList;

    // Deep copy IFO monster spawn areas so this instance has its own isolated spawns
    for (UINT i = 0; i < baseMap->MonsterSpawnList.size(); i++)
    {
        CSpawnArea* baseSpawn = baseMap->MonsterSpawnList.at(i);
        if (!baseSpawn) continue;

        CSpawnArea* instSpawn = new CSpawnArea(*baseSpawn);
        instSpawn->map = newId;
        instSpawn->MobPointer = 1;
        instSpawn->MCount = 0;
        instSpawn->ResetSpawn = false;
        instSpawn->lastRespawnTime = clock();
        newMap->MonsterSpawnList.push_back(instSpawn);

        // Spawn initial mob group
        for (UINT k = 0; k < instSpawn->bcount[1]; k++)
        {
            fPoint pos = GServer->RandInCircle(instSpawn->points, instSpawn->radius);
            newMap->AddMonster(instSpawn->bmob[1], pos, 0, NULL, NULL, instSpawn->id);
            instSpawn->MCount++;
        }
    }

    // Register instance in server map tables under MapMutex
    pthread_mutex_lock(&GServer->MapMutex);
    GServer->MapList.Index[newId] = newMap;
    GServer->MapList.Map.push_back(newMap);
    pthread_mutex_unlock(&GServer->MapMutex);

    m_activeInstances[newId] = newMap;
    m_partyInstances[std::make_pair(baseMapId, partyId)] = newId;

    pthread_mutex_unlock(&m_dungeonMutex);

    Log(MSG_INFO, "CDungeonManager: Created dynamic instance %u for base map %u (party/owner %u, monsters %zu)",
        newId, baseMapId, partyId, newMap->MonsterList.size());

    return newMap;
}

void CDungeonManager::DestroyInstance(CMap* map)
{
    if (!map || !map->is_instance) return;

    UINT instId = map->instance_id;
    UINT baseZone = map->base_zone;
    UINT partyId = map->party_id;

    pthread_mutex_lock(&m_dungeonMutex);
    m_activeInstances.erase(instId);
    m_partyInstances.erase(std::make_pair(baseZone, partyId));
    pthread_mutex_unlock(&m_dungeonMutex);

    // Unregister from server map tables under MapMutex
    pthread_mutex_lock(&GServer->MapMutex);
    if (instId < MAX_ZONE_INSTANCES && GServer->MapList.Index[instId] == map)
    {
        GServer->MapList.Index[instId] = GServer->MapList.nullzone;
    }
    for (auto it = GServer->MapList.Map.begin(); it != GServer->MapList.Map.end(); ++it)
    {
        if (*it == map)
        {
            GServer->MapList.Map.erase(it);
            break;
        }
    }
    pthread_mutex_unlock(&GServer->MapMutex);

    delete map;

    Log(MSG_INFO, "CDungeonManager: Destroyed dynamic instance %u (base %u, party %u)", instId, baseZone, partyId);
}

void CDungeonManager::BroadcastToInstance(CMap* map, const char* message)
{
    if (!map || !message) return;
    for (UINT i = 0; i < map->PlayerList.size(); i++)
    {
        CPlayer* p = map->PlayerList.at(i);
        if (p)
        {
            GServer->SendPM(p, "%s", message);
        }
    }
}

UINT CDungeonManager::GetRemainingTime(CMap* map)
{
    if (!map || !map->is_instance) return 0;
    clock_t elapsed = (clock() - map->creation_time) / CLOCKS_PER_SEC;
    if (elapsed >= map->time_limit_sec) return 0;
    return map->time_limit_sec - elapsed;
}

void CDungeonManager::HandlePartyWipe(CMap* map)
{
    if (!map || !map->is_instance || map->is_wiped) return;
    map->is_wiped = true;

    BroadcastToInstance(map, "[Dungeon] Party wiped! All party members have fallen. Teleporting to entrance...");

    UINT exitMap = 0;
    fPoint exitPos;
    GetDungeonExit(map->base_zone, exitMap, exitPos);

    std::vector<CPlayer*> playersToTeleport = map->PlayerList;
    for (UINT i = 0; i < playersToTeleport.size(); i++)
    {
        CPlayer* player = playersToTeleport.at(i);
        if (!player) continue;

        player->Stats->HP = (player->Stats->MaxHP * 20) / 100;
        if (player->Stats->HP < 1) player->Stats->HP = 1;
        player->Stats->MP = (player->Stats->MaxMP * 20) / 100;

        for (int j = 0; j < 30; j++)
        {
            player->MagicStatus[j].Duration = 0;
            player->MagicStatus[j].BuffTime = 0;
        }
        player->RefreshBuff();
        player->SetStats();

        GServer->MapList.Index[exitMap]->TeleportPlayer(player, exitPos, false);
    }
    map->empty_since = clock();
}

void CDungeonManager::HandleTimeout(CMap* map)
{
    if (!map || !map->is_instance) return;

    BroadcastToInstance(map, "[Dungeon] Time limit reached! The dungeon has collapsed. Teleporting to entrance...");

    UINT exitMap = 0;
    fPoint exitPos;
    GetDungeonExit(map->base_zone, exitMap, exitPos);

    std::vector<CPlayer*> playersToTeleport = map->PlayerList;
    for (UINT i = 0; i < playersToTeleport.size(); i++)
    {
        CPlayer* player = playersToTeleport.at(i);
        if (!player) continue;
        GServer->MapList.Index[exitMap]->TeleportPlayer(player, exitPos, false);
    }
    map->empty_since = clock();
}

void CDungeonManager::TeleportPartyOutOfDungeon(CMap* map, const char* reasonMessage)
{
    if (!map) return;
    if (reasonMessage)
    {
        BroadcastToInstance(map, reasonMessage);
    }

    UINT exitMap = 0;
    fPoint exitPos;
    GetDungeonExit(map->base_zone, exitMap, exitPos);

    std::vector<CPlayer*> playersToTeleport = map->PlayerList;
    for (UINT i = 0; i < playersToTeleport.size(); i++)
    {
        CPlayer* player = playersToTeleport.at(i);
        if (!player) continue;
        GServer->MapList.Index[exitMap]->TeleportPlayer(player, exitPos, false);
    }
    map->empty_since = clock();
}

void CDungeonManager::Update()
{
    std::vector<CMap*> instances;
    pthread_mutex_lock(&m_dungeonMutex);
    for (auto& pair : m_activeInstances)
    {
        if (pair.second) instances.push_back(pair.second);
    }
    pthread_mutex_unlock(&m_dungeonMutex);

    std::vector<CMap*> toDestroy;

    for (UINT i = 0; i < instances.size(); i++)
    {
        CMap* map = instances.at(i);
        if (!map || !map->is_instance) continue;

        // 1. Check if empty
        if (map->PlayerList.empty())
        {
            if (map->empty_since == 0)
            {
                map->empty_since = clock();
            }
            else
            {
                clock_t emptySec = (clock() - map->empty_since) / CLOCKS_PER_SEC;
                if (emptySec >= 30)
                {
                    toDestroy.push_back(map);
                    continue;
                }
            }
        }
        else
        {
            map->empty_since = 0;
        }

        // 2. Party wipe check (when players are in map and wipe hasn't been handled yet)
        if (!map->is_wiped && !map->PlayerList.empty())
        {
            bool allDead = true;
            for (UINT j = 0; j < map->PlayerList.size(); j++)
            {
                CPlayer* p = map->PlayerList.at(j);
                if (p && !p->IsDead() && p->Stats->HP > 0)
                {
                    allDead = false;
                    break;
                }
            }
            if (allDead)
            {
                HandlePartyWipe(map);
                toDestroy.push_back(map);
                continue;
            }
        }

        // 3. Countdown timer check
        clock_t elapsed = (clock() - map->creation_time) / CLOCKS_PER_SEC;
        if (elapsed >= map->time_limit_sec)
        {
            HandleTimeout(map);
            toDestroy.push_back(map);
            continue;
        }

        UINT rem = map->time_limit_sec - elapsed;
        if (rem <= 60 && !map->warn_1m_sent)
        {
            map->warn_1m_sent = true;
            BroadcastToInstance(map, "[Dungeon] Warning: 1 minute remaining before dungeon collapses!");
        }
        else if (rem <= 300 && !map->warn_5m_sent)
        {
            map->warn_5m_sent = true;
            BroadcastToInstance(map, "[Dungeon] Warning: 5 minutes remaining in this instance.");
        }
        else if (rem <= 600 && !map->warn_10m_sent)
        {
            map->warn_10m_sent = true;
            BroadcastToInstance(map, "[Dungeon] Warning: 10 minutes remaining in this instance.");
        }
    }

    for (UINT i = 0; i < toDestroy.size(); i++)
    {
        DestroyInstance(toDestroy.at(i));
    }
}
