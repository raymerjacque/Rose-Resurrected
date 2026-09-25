#include "worldserver.h"
#include "PlayerBot.h"

bool CWorldServer::handleCommand( char* buffer )
{
	char* command = strtok( buffer , " ");
	char* tmp;
	if (command==NULL) return true;
	if(strcmp( command, "closeserver" )==0)
	{
        Log( MSG_CONSOLE, "Closing server..." );
        isActive = false;
        return false;
    }
    if(strcmp( command, "ann" )==0)
    {
        char* from = strtok( NULL , " ");
        if(from==NULL)
        {
            Log (MSG_CONSOLE, "'ann' command: ann <name> <message>" );
            return true;
        }
        unsigned msgoffset = strlen(from)+5;
        return pakConsoleAnn( from, (char*)&buffer[msgoffset] );
    }
    if(strcmp( command, "kick" )==0)
    {
        char* name = strtok( NULL , " ");
        return pakConsoleKick( name );
    }
    if(strcmp( command, "hhOn" )==0)
    {
        if ((tmp = strtok(NULL, " ")) == NULL) tmp = 0;
        int hhOn=atoi(tmp);

        if(hhOn == 1)
        {
            doubleExpMapActivate();
            Log(MSG_CONSOLE, "HH activated");
			Log(MSG_GMACTION, "Server :: /hhOn active");
        }
		if(hhOn == 0)
		{
			doubleExpMapDeactivate();
			GServer->Config.doubleExpActive = 0;
			Log(MSG_CONSOLE, "HH deactivated");
			Log(MSG_GMACTION, "Server :: /hhOn deactive");
		}
    }
    else
    if(strcmp( command, "adver" )==0)
    {
        if ((tmp = strtok(NULL, " ")) == NULL) tmp = 0;
        int adver=atoi(tmp);

        if(adver == 1)
        {
            Config.AdvertiseActive = 1;
            DB->QExecute("UPDATE  list_config SET advertise_active = 1");
            Config.AdvertiseTime = clock();
            Advertise();

            Log (MSG_CONSOLE, "Advertise system active" );
        }else
        {
            Config.AdvertiseActive = 0;
            DB->QExecute("UPDATE list_config SET advertise_active = 0");
            Log (MSG_CONSOLE, "Advertise system deactivated" );
        }
        return true;
    }
    else
    if(strcmp( command, "loadads" )==0)
    {
            LoadAdverds();
            Log (MSG_CONSOLE, "Adverds loaded");
            return true;
    }
    else
    if(strcmp( command, "help" )==0)
    {
            Log (MSG_CONSOLE, "Console Commands Available:" );
            Log (MSG_CONSOLE, "'ann' command: ann <name> <message>" );
			Log (MSG_CONSOLE, "'kick' command: kick <char name> kicks a player." );
			Log (MSG_CONSOLE, "'hhon' command: hhon <0/1> turns HH on or off." );
            Log (MSG_CONSOLE, "'bot' command: bot <spawn|remove|list|clear>." );
            Log (MSG_CONSOLE, "'help' command: returns this list" );
            return true;
    }
    else
    if(strcmp( command, "bot" )==0)
    {
        char* sub = strtok(NULL, " ");
        if (!sub || strcmp(sub, "help") == 0)
        {
            Log(MSG_CONSOLE, "bot spawn <name> [job] [lvl] [map] [x] [y]");
            Log(MSG_CONSOLE, "bot buffbot <name> [map] [x] [y]");
            Log(MSG_CONSOLE, "bot remove <name>");
            Log(MSG_CONSOLE, "bot list");
            Log(MSG_CONSOLE, "bot clear");
            return true;
        }
        if (strcmp(sub, "buffbot") == 0)
        {
            char* name = strtok(NULL, " ");
            char* mapStr = strtok(NULL, " ");
            char* xStr = strtok(NULL, " ");
            char* yStr = strtok(NULL, " ");

            if (!name) { Log(MSG_CONSOLE, "Usage: bot buffbot <name> [map] [x] [y]"); return true; }
            int mapId = mapStr ? atoi(mapStr) : 2;
            fPoint pos;
            pos.x = xStr ? (float)atof(xStr) : 5509.0f;
            pos.y = yStr ? (float)atof(yStr) : 5485.0f;
            pos.z = 0;

            CPlayer* b = CBotManager::GetInstance()->SpawnBuffBot(name, mapId, pos);
            if (b) Log(MSG_CONSOLE, "Buff Bot '%s' spawned on map %d at (%.1f, %.1f)", name, mapId, pos.x, pos.y);
            else Log(MSG_CONSOLE, "Failed to spawn Buff Bot '%s'", name);
            return true;
        }
        if (strcmp(sub, "spawn") == 0)
        {
            char* name = strtok(NULL, " ");
            char* jobStr = strtok(NULL, " ");
            char* lvlStr = strtok(NULL, " ");
            char* mapStr = strtok(NULL, " ");
            char* xStr = strtok(NULL, " ");
            char* yStr = strtok(NULL, " ");

            if (!name) { Log(MSG_CONSOLE, "Usage: bot spawn <name> [job] [lvl] [map] [x] [y]"); return true; }
            int job = jobStr ? atoi(jobStr) : 111;
            if (job == 0 && jobStr && strcmp(jobStr, "0") != 0) job = 111;
            int lvl = lvlStr ? atoi(lvlStr) : 1;
            int mapId = mapStr ? atoi(mapStr) : 2;
            fPoint pos;
            pos.x = xStr ? (float)atof(xStr) : 5200.0f;
            pos.y = yStr ? (float)atof(yStr) : 5200.0f;
            pos.z = 0;

            CPlayer* b = CBotManager::GetInstance()->SpawnBot(name, job, lvl, mapId, pos);
            if (b) Log(MSG_CONSOLE, "Bot '%s' spawned on map %d at (%.1f, %.1f)", name, mapId, pos.x, pos.y);
            else Log(MSG_CONSOLE, "Failed to spawn bot '%s'", name);
            return true;
        }
        if (strcmp(sub, "remove") == 0)
        {
            char* name = strtok(NULL, " ");
            if (!name) { Log(MSG_CONSOLE, "Usage: bot remove <name>"); return true; }
            if (CBotManager::GetInstance()->RemoveBot(name)) Log(MSG_CONSOLE, "Bot '%s' removed.", name);
            else Log(MSG_CONSOLE, "Bot '%s' not found.", name);
            return true;
        }
        if (strcmp(sub, "list") == 0)
        {
            std::vector<CPlayerBot*> bots = CBotManager::GetInstance()->GetBots();
            Log(MSG_CONSOLE, "Active bots count: %zu", bots.size());
            for (size_t i = 0; i < bots.size(); i++)
            {
                CPlayerBot* pb = bots[i];
                if (pb && pb->GetPlayer())
                {
                    CPlayer* p = pb->GetPlayer();
                    Log(MSG_CONSOLE, "  #%zu: %s (Lvl %d, Job %d, Map %d, HP %I64i/%d, State: %s)",
                        i + 1, p->CharInfo->charname, p->Stats->Level, p->CharInfo->Job,
                        p->Position->Map, p->Stats->HP, p->Stats->MaxHP, pb->GetStateString());
                }
            }
            return true;
        }
        if (strcmp(sub, "clear") == 0)
        {
            CBotManager::GetInstance()->RemoveAllBots();
            Log(MSG_CONSOLE, "All bots removed.");
            return true;
        }
        return true;
    }
    else

        Log( MSG_CONSOLE, "Command not handled" );
    return true;
}

// CONSOLE: Announcment
bool CWorldServer::pakConsoleAnn( char* from, char* message )
{
	BEGINPACKET( pak, 0x702 );
	ADDSTRING( pak, from );
	ADDSTRING( pak, "> " );
	ADDSTRING( pak, message );
	ADDBYTE( pak, 0x00);
	SendToAll( &pak );
	Log( MSG_CONSOLE, "Announcment sent" );
	return true;
}

bool CWorldServer::pakConsoleKick(char* name)
{
    Log( MSG_GMACTION, " Server : /kick %s" , name);
    CPlayer* otherclient = GetClientByCharName( name );
    if(otherclient==NULL)
        return true;
    BEGINPACKET( pak, 0x702 );
    ADDSTRING( pak, "You were disconnected from the server !" );
    ADDBYTE( pak, 0 );
    otherclient->client->SendPacket( &pak );

    RESETPACKET( pak, 0x707 );
    ADDWORD( pak, 0 );
    otherclient->client->SendPacket( &pak );

    otherclient->client->isActive = false;

    return true;
}
