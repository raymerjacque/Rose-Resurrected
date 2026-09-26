#ifndef __DUNGEON_MANAGER_H__
#define __DUNGEON_MANAGER_H__

#include <map>
#include <vector>
#include <pthread.h>
#include "worldserver.h"
#include "worldmap.h"
#include "player.h"

class CDungeonManager
{
public:
    static CDungeonManager* GetInstance();

    // Check if map ID is a registered dungeon cave or instanced dungeon
    bool IsDungeonZone(UINT mapId);
    UINT GetBaseDungeonZone(UINT mapId);
    bool GetDungeonExit(UINT baseZone, UINT& exitMap, fPoint& exitPos);

    // Retrieve active instance or create a new one for player/party
    CMap* GetOrCreatePartyInstance(CPlayer* player, UINT baseMapId);
    CMap* CreateInstance(UINT baseMapId, UINT partyId);
    void DestroyInstance(CMap* map);

    // Main tick loop
    void Update();

    // Event handlers
    void HandlePartyWipe(CMap* map);
    void HandleTimeout(CMap* map);
    UINT GetRemainingTime(CMap* map);

    // Instance messaging & teleport helpers
    void BroadcastToInstance(CMap* map, const char* message);
    void TeleportPartyOutOfDungeon(CMap* map, const char* reasonMessage);

private:
    CDungeonManager();
    ~CDungeonManager();

    static CDungeonManager* m_instance;
    pthread_mutex_t m_dungeonMutex;

    // Active dynamic instances keyed by instance_id
    std::map<UINT, CMap*> m_activeInstances;
    // Map of (baseMapId, partyId) -> instance_id
    std::map<std::pair<UINT, UINT>, UINT> m_partyInstances;
};

#endif
