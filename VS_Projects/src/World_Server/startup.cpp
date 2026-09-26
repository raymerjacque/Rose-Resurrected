/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007,2008 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/
#include "worldserver.h"
bool CWorldServer::LoadSTBData( )
{
    Log( MSG_LOAD, "STB Data             " );  // all of the stb loading was from osiRose and ospRose

    //LMA: loading Pegasus data.
    if(Config.is_pegasus==1)
    {
        //LMA: test for quest hack (stackable).
        #ifdef QHACK
        STBStoreDataNPC( "3DDataPeg\\STB\\LIST_NPC.STB", &STB_NPC );
        #else
        STBStoreData( "3DDataPeg\\STB\\LIST_NPC.STB", &STB_NPC );
        #endif

        STBStoreData( "3DDataPeg\\STB\\LIST_SKILL.STB", &STB_SKILL );
        STBStoreData( "3DDataPeg\\STB\\LIST_STATUS.STB", &STB_STATUS );
        STBStoreData( "3DDataPeg\\STB\\LIST_FACEITEM.STB", &STB_ITEM[0] );
        STBStoreData( "3DDataPeg\\STB\\LIST_CAP.STB", &STB_ITEM[1] );
        STBStoreData( "3DDataPeg\\STB\\LIST_BODY.STB", &STB_ITEM[2] );
        STBStoreData( "3DDataPeg\\STB\\LIST_ARMS.STB", &STB_ITEM[3] );
        STBStoreData( "3DDataPeg\\STB\\LIST_FOOT.STB", &STB_ITEM[4] );
        STBStoreData( "3DDataPeg\\STB\\LIST_BACK.STB", &STB_ITEM[5] );
        STBStoreData( "3DDataPeg\\STB\\LIST_JEWEL.STB", &STB_ITEM[6] );
        STBStoreData( "3DDataPeg\\STB\\LIST_WEAPON.STB", &STB_ITEM[7] );
        STBStoreData( "3DDataPeg\\STB\\LIST_SUBWPN.STB", &STB_ITEM[8] );
        STBStoreData( "3DDataPeg\\STB\\LIST_USEITEM.STB", &STB_ITEM[9] );
        STBStoreData( "3DDataPeg\\STB\\LIST_JEMITEM.STB", &STB_ITEM[10] );
        STBStoreData( "3DDataPeg\\STB\\LIST_NATURAL.STB", &STB_ITEM[11] );
        STBStoreData( "3DDataPeg\\STB\\LIST_QUESTITEM.STB", &STB_ITEM[12] );
        STBStoreData( "3DDataPeg\\STB\\LIST_PAT.STB", &STB_ITEM[13] );
        STBStoreData( "3DDataPeg\\STB\\LIST_PRODUCT.STB", &STB_PRODUCT );
        STBStoreData( "3DDataPeg\\STB\\LIST_SELL.STB", &STB_SELL );
        //STBStoreData( "3DDataPeg\\STB\\LIST_ZONE.STB", &STB_ZONE );
        STBStoreData( "3DDataPeg\\STB\\ITEM_DROP.STB", &STB_DROP );
        //STBStoreData("3DDataPeg\\STB\\LIST_UPGRADE.STB", &upgradeData);
        STBStoreData("3DDataPeg\\STB\\LIST_BREAK.STB", &BreakData);    //LMA: for break and chest and blue break.
        STBStoreDataChar("3DDataPeg\\STB\\LIST_ZONE.STB", &ZoneData);  //LMA: Getting Zone Data.
        STBStoreData("3DDataPeg\\STB\\LIST_CLASS.STB", &ListClass);    //LMA: used to store the class list, actually to check equip requirements.
        STBStoreData("3DDataPeg\\STB\\LIST_GRADE.STB", &ListGrade);    //LMA: used to store the refine bonuses (% and +).
    }
    else
    {
        #ifdef QHACK
        STBStoreDataNPC( "3DData\\STB\\LIST_NPC.STB", &STB_NPC );
        #else
        STBStoreData( "3DData\\STB\\LIST_NPC.STB", &STB_NPC );
        #endif

        STBStoreData( "3DData\\STB\\LIST_SKILL.STB", &STB_SKILL );
        STBStoreData( "3DData\\STB\\LIST_STATUS.STB", &STB_STATUS );
        STBStoreData( "3DData\\STB\\LIST_FACEITEM.STB", &STB_ITEM[0] );
        STBStoreData( "3DData\\STB\\LIST_CAP.STB", &STB_ITEM[1] );
        STBStoreData( "3DData\\STB\\LIST_BODY.STB", &STB_ITEM[2] );
        STBStoreData( "3DData\\STB\\LIST_ARMS.STB", &STB_ITEM[3] );
        STBStoreData( "3DData\\STB\\LIST_FOOT.STB", &STB_ITEM[4] );
        STBStoreData( "3DData\\STB\\LIST_BACK.STB", &STB_ITEM[5] );
        STBStoreData( "3DData\\STB\\LIST_JEWEL.STB", &STB_ITEM[6] );
        STBStoreData( "3DData\\STB\\LIST_WEAPON.STB", &STB_ITEM[7] );
        STBStoreData( "3DData\\STB\\LIST_SUBWPN.STB", &STB_ITEM[8] );
        STBStoreData( "3DData\\STB\\LIST_USEITEM.STB", &STB_ITEM[9] );
        STBStoreData( "3DData\\STB\\LIST_JEMITEM.STB", &STB_ITEM[10] );
        STBStoreData( "3DData\\STB\\LIST_NATURAL.STB", &STB_ITEM[11] );
        STBStoreData( "3DData\\STB\\LIST_QUESTITEM.STB", &STB_ITEM[12] );
        STBStoreData( "3DData\\STB\\LIST_PAT.STB", &STB_ITEM[13] );
        STBStoreData( "3DData\\STB\\LIST_PRODUCT.STB", &STB_PRODUCT );
        STBStoreData( "3DData\\STB\\LIST_SELL.STB", &STB_SELL );
        //STBStoreData( "3DData\\STB\\LIST_ZONE.STB", &STB_ZONE );
        STBStoreData( "3DData\\STB\\ITEM_DROP.STB", &STB_DROP );
        STBStoreData("3DData\\STB\\LIST_BREAK.STB", &BreakData);			//LMA: for break and chest and blue break.
        STBStoreDataChar("3DData\\STB\\LIST_ZONE.STB", &ZoneData);			//LMA: Getting Zone Data.
        STBStoreData("3DData\\STB\\LIST_CLASS.STB", &ListClass);			//LMA: used to store the class list, actually to check equip requirements.
        STBStoreData("3DData\\STB\\LIST_GRADE.STB", &ListGrade);			//LMA: used to store the refine bonuses (% and +).
		STBStoreData ("3DData\\STB\\LIST_UPGRADE.STB", &ListUpGrade);		//PY: Used to store the refine chances for equip upgrades
    }

    //LMA: Loading STL too :)
    Log( MSG_LOAD, "STL Data             " );
    if(Config.is_pegasus==1)
    {
        STLStoreData( "3DDataPeg\\STB\\LIST_FACEITEM_S.STL", 1 );
        STLStoreData( "3DDataPeg\\STB\\LIST_CAP_S.STL", 2 );
        STLStoreData( "3DDataPeg\\STB\\LIST_BODY_S.STL", 3 );
        STLStoreData( "3DDataPeg\\STB\\LIST_ARMS_S.STL", 4 );
        STLStoreData( "3DDataPeg\\STB\\LIST_FOOT_S.STL", 5 );
        STLStoreData( "3DDataPeg\\STB\\LIST_BACK_S.STL", 6 );
        STLStoreData( "3DDataPeg\\STB\\LIST_JEWEL_S.STL", 7 );
        STLStoreData( "3DDataPeg\\STB\\LIST_WEAPON_S.STL", 8 );
        STLStoreData( "3DDataPeg\\STB\\LIST_SUBWPN_S.STL", 9 );
        STLStoreData( "3DDataPeg\\STB\\LIST_USEITEM_S.STL", 10 );
        STLStoreData( "3DDataPeg\\STB\\LIST_JEMITEM_S.STL", 11 );
        STLStoreData( "3DDataPeg\\STB\\LIST_NATURAL_S.STL", 12 );
        STLStoreData( "3DDataPeg\\STB\\LIST_QUESTITEM_S.STL", 13 );
        STLStoreData( "3DDataPeg\\STB\\LIST_PAT_S.STL", 14 );
        STLStoreData( "3DDataPeg\\STB\\LIST_NPC_S.STL", 15 );
        STLStoreData( "3DDataPeg\\STB\\STR_ITEMPREFIX.STL", 16 );
        STLStoreData( "3DDataPeg\\STB\\LIST_SKILL_S.STL", 17 ); //Skills take a lot
        STLStoreData( "3DDataPeg\\STB\\LIST_QUEST_S.STL",18);
        STLStoreData( "3DDataPeg\\STB\\LIST_ZONE_S.STL",19);
    }
    else
    {
        STLStoreData( "3DData\\STB\\LIST_FACEITEM_S.STL", 1 );
        STLStoreData( "3DData\\STB\\LIST_CAP_S.STL", 2 );
        STLStoreData( "3DData\\STB\\LIST_BODY_S.STL", 3 );
        STLStoreData( "3DData\\STB\\LIST_ARMS_S.STL", 4 );
        STLStoreData( "3DData\\STB\\LIST_FOOT_S.STL", 5 );
        STLStoreData( "3DData\\STB\\LIST_BACK_S.STL", 6 );
        STLStoreData( "3DData\\STB\\LIST_JEWEL_S.STL", 7 );
        STLStoreData( "3DData\\STB\\LIST_WEAPON_S.STL", 8 );
        STLStoreData( "3DData\\STB\\LIST_SUBWPN_S.STL", 9 );
        STLStoreData( "3DData\\STB\\LIST_USEITEM_S.STL", 10 );
        STLStoreData( "3DData\\STB\\LIST_JEMITEM_S.STL", 11 );
        STLStoreData( "3DData\\STB\\LIST_NATURAL_S.STL", 12 );
        STLStoreData( "3DData\\STB\\LIST_QUESTITEM_S.STL", 13 );
        STLStoreData( "3DData\\STB\\LIST_PAT_S.STL", 14 );
        STLStoreData( "3DData\\STB\\LIST_NPC_S.STL", 15 );
        STLStoreData( "3DData\\STB\\STR_ITEMPREFIX.STL", 16 );
        STLStoreData( "3DData\\STB\\LIST_SKILL_S.STL", 17 );
        STLStoreData( "3DData\\STB\\LIST_QUEST_S.STL",18);
        STLStoreData( "3DData\\STB\\LIST_ZONE_S.STL",19);
    }

    //adding some special values :)
    STLNameList[0]=(char*) "No STL";
    STLNameList[1]=(char*) "STL Error";

    //LMA: exporting to test ;)
    /*
    map <UINT, char*>::iterator itr;
    for(itr = STLNameList.begin(); itr != STLNameList.end(); ++itr)
    {
        //cout << "Key: " << (*itr).first << " Value: " << (*itr).second;
        Log(MSG_INFO,"STL Id %u, text: %s",itr->first,itr->second);
    }
    */


    return true;
}

//LMA: loading IFO Objects (so far only the warp gate)
bool CWorldServer::LoadIfoObjects( )
{
    //LMA: Warp Gate Union :)
    WarpGate.hidden=true;
    WarpGate.id=1;
    WarpGate.virtualNpctypeID=10001;
    for (int k=0;k<20;k++)
        WarpGate.IfoObjVar[k]=0;
    WarpGate.IfoPosition.x=5470.72;
    WarpGate.IfoPosition.y=5248.40;
    WarpGate.IfoPosition.z=0;
    WarpGate.direction=0;
    WarpGate.mapid=2;
    WarpGate.IfoX=34;
    WarpGate.IfoY=32;
    WarpGate.clientID=0;
    WarpGate.Npc=NULL;

   CNPCData* newnpc = new (nothrow) CNPCData;
    if(newnpc==NULL)
    {
        Log( MSG_ERROR, "Error allocing memory" );
        WarpGate.NPCData=NULL;
        return true;
    }

    newnpc->id = 1;
    newnpc->life = 0;
    newnpc->stance =0;
    newnpc->wspeed =0;
    newnpc->rspeed =0;
    //newnpc->dspeed =0; //dspeed is monster size at startup.cpp...
    newnpc->weapon =0;
    newnpc->subweapon =0;
    newnpc->level =0;
    newnpc->hp =100;
    newnpc->atkpower =0;
    newnpc->hitrate =0;
    newnpc->defense =100;
    newnpc->magicdefense =0;
    newnpc->dodge =0;
    newnpc->atkspeed =0;
    newnpc->AI =0;
    newnpc->AiTimer=0;
    newnpc->exp =0;
    newnpc->dropid =0;
    newnpc->money =0;
    newnpc->item =0;
    newnpc->tab1 =0;
    newnpc->tab2 =0;
    newnpc->tab3 =0;
    newnpc->specialtab =0;
    newnpc->atkdistance =0;
    //newnpc->aggresive =0;				/// No... just no
    //newnpc->helpless=0;
    //newnpc->shp =0;
    newnpc->dialogid = 0;
    newnpc->eventid = 0;
    newnpc->side=0;
    newnpc->sidechance=0;
    newnpc->STLId=0;

    //LMA: Various skills for monsters (won't be used anymore, will be done by AIP, for now left for compatibility).
    for(int i=0;i<4;i++)
    {
      newnpc->askills[i]=0;
      newnpc->bskills[i]=0;
      newnpc->dskills[i]=0;
    }

    newnpc->lastskill=0;
    newnpc->sigskill=0;
    newnpc->delayskill=0;

    WarpGate.NPCData=newnpc;


    return true;
}


//LMA: loading LTB (for AIP)
bool CWorldServer::LoadLTB( )
{
    maxltbaip=0;
    maxltbqsd=0;
    Log(MSG_INFO, "Loading LTB strings");

    //LMA: loading Pegasus data.
    if(Config.is_pegasus==1)
    {
        LTBStoreData("3DDataPeg\\AI\\ULNGTB_AI.LTB",&MyLTB);
        LTBStoreData("3DDataPeg\\QUESTDATA\\ULNGTB_QST.LTB",&MyLTBQSD);
    }
    else
    {
        LTBStoreData("3DData\\AI\\ULNGTB_AI.LTB",&MyLTB);
        LTBStoreData("3DData\\QUESTDATA\\ULNGTB_QST.LTB",&MyLTBQSD);
    }

    if (MyLTB.record.size()==0||MyLTBQSD.record.size()==0)
    {
        return true;
    }

    //LTB for AIP
    maxltbaip=MyLTB.record.size();
    Ltbstring = new CLTBstring*[MyLTB.record.size()];
    for (unsigned int k=0;k<MyLTB.record.size();k++)
    {
        //cout << "IndexMyLTB " << k << " NPC: " << MyLTB.record.at(k).name.c_str() << ", sentence: " << MyLTB.record.at(k).sentence.c_str() << endl;
        CLTBstring* tempLTB = new CLTBstring;
        tempLTB->NPCname=new char[MyLTB.record.at(k).name.size()+1];
        strcpy (tempLTB->NPCname, MyLTB.record.at(k).name.c_str());
        tempLTB->LTBstring=new char[MyLTB.record.at(k).sentence.size()+1];
        strcpy (tempLTB->LTBstring, MyLTB.record.at(k).sentence.c_str());
        GServer->Ltbstring[k]=tempLTB;
    }

    //LTB for QSD
    maxltbqsd=MyLTBQSD.record.size();
    LtbstringQSD = new CLTBstring*[MyLTBQSD.record.size()];
    for (unsigned int k=0;k<MyLTBQSD.record.size();k++)
    {
        //cout << "IndexMyLTBQSD " << k << " NPC: " << MyLTBQSD.record.at(k).name.c_str() << ", sentence: " << MyLTBQSD.record.at(k).sentence.c_str() << endl;
        CLTBstring* tempLTB = new CLTBstring;
        tempLTB->NPCname=new char[2];
        strcpy (tempLTB->NPCname, " ");
        tempLTB->LTBstring=new char[MyLTBQSD.record.at(k).sentence.size()+1];
        strcpy (tempLTB->LTBstring, MyLTBQSD.record.at(k).sentence.c_str());
        GServer->LtbstringQSD[k]=tempLTB;
    }

    Log( MSG_INFO, "LTB Data Loaded" );

    return true;
}

//LMA: we init default values for some lists.
bool CWorldServer::InitDefaultValues()
{
    //Creating indexes
	//EquipList [ 0 ].Index = 0;                                            //PY not necessary or even relevent. Uses a vector now
	EquipList [ 0 ].max = 0;
    for(UINT j=0; j<9; j++)
    {
        //EquipList[j + 1].Index = new CEquip*[STB_ITEM[j].rowcount];       //PY not necessary
        EquipList[j + 1].max = STB_ITEM[j].rowcount;
		UINT p = j;
    }

    //JemList.Index		= new CJemData*[STB_ITEM[10].rowcount];             //PY not necessary
    JemList.max			= STB_ITEM[10].rowcount;
    NaturalList.Index	= new CNaturalData*[STB_ITEM[11].rowcount];
    NaturalList.max		= STB_ITEM[11].rowcount;
    PatList.Index		= new CPatData*[STB_ITEM[13].rowcount];
    PatList.max			= STB_ITEM[13].rowcount;
    ProductList.Index	= new CProductData*[STB_PRODUCT.rowcount];
    ProductList.max		= STB_PRODUCT.rowcount;
    SellList.Index		= new CCSellData*[STB_SELL.rowcount];
    SellList.max		= STB_SELL.rowcount;
    UseList.Index		= new CUseData*[STB_ITEM[9].rowcount];
    UseList.max			= STB_ITEM[9].rowcount;

    //Real amount.
    MapList.Index		= new CMap*[ZoneData.rowcount];
    MapList.max			= ZoneData.rowcount;
    maxZone				= ZoneData.rowcount;

    //LMA: adding questItems too
    QuestItemData		= new CQuestItemData*[STB_ITEM[12].rowcount];
    maxQuestItems		= STB_ITEM[12].rowcount;

    //stats.
    StatusList			= new CStatus*[STB_STATUS.rowcount];
    maxStatus			= STB_STATUS.rowcount;

    //breaklist.
    BreakList			= new CBreakList*[BreakData.rowcount];
    maxBreak			= BreakData.rowcount;

    //Skills and NPC as static array too, but no init for this one.
    SkillList			= new CSkills*[STB_SKILL.rowcount];
    maxSkills			= STB_SKILL.rowcount;
    NPCData				= new CNPCData*[STB_NPC.rowcount];
    maxNPC				= STB_NPC.rowcount;

    //Item stats.
    StatsList			= new CItemStas*[STB_ITEM[10].rowcount];
    maxStats			= STB_ITEM[10].rowcount;

    //LMA: job requirements.
    ClassList			= new vector<UINT>*[ListClass.rowcount];
    maxClass			= ListClass.rowcount;

    //LMA: Refine bonuses (+ and %)
    GradeList			= new CGrade*[ListGrade.rowcount];
    maxGrades			= ListGrade.rowcount;

    //Null break
    CBreakList* nullBreak	= new CBreakList;
    nullBreak->itemnum		= 0;
    nullBreak->itemtype		= 0;
    //nullBreak->numToGive=0;
    //nullBreak->total=0;
    nullBreak->nb_reward	= 0;
    nullBreak->reward_min	= 0;
    nullBreak->reward_max	= 0;

    for(int i=0;i<20;i++)
    {
        nullBreak->amount_min[i]	= 0;
        nullBreak->amount_max[i]	= 0;
        nullBreak->product[i]		= 0;
        nullBreak->prob[i]			= 0;
    }

    for(UINT i=0;i<maxBreak;i++)
    {
        BreakList[i]=nullBreak;
    }

    //Equip null init
    CEquip* nullequip = new CEquip;
    nullequip->id				= 0;
    nullequip->equiptype		= 0;
	nullequip->restriction		= 0;
    nullequip->type				= 0;
    nullequip->price			= 0;
    nullequip->pricerate		= 0;
    nullequip->weight			= 0;
    nullequip->quality			= 0;
    nullequip->level			= 0;
    nullequip->craft_skill		= 0;
    nullequip->craft_level		= 0;
    nullequip->craft_recipe		= 0;
    nullequip->craft_difficult	= 0;
    nullequip->defense			= 0;
    nullequip->magicresistence	= 0;
    nullequip->attackdistance	= 0;
    nullequip->movespeed		= 0;
    nullequip->attackpower		= 0;
    nullequip->attackspeed		= 0;
    nullequip->itemgrade		= 0;
    nullequip->refineitem		= 0;
    nullequip->breakid			= 0;
	nullequip->is_mileage		= 0;
	nullequip->BlockRate		= 0;
	nullequip->magicattack		= 0;
	nullequip->STLId			= 0;
	nullequip->STLPrefix		= 0;
	nullequip->acc              = 0;
    nullequip->durability       = 0;
    nullequip->dodge            = 0;
    nullequip->rare_type        = 0;

    for(int i=0;i<3;i++)
    {
        nullequip->occupation[i] = 0;
        nullequip->condition1[i] = 0;
        nullequip->condition2[i] = 0;
    }
    for(int i=0;i<2;i++)
    {
        nullequip->stat1[i] = 0;
        nullequip->stat2[i] = 0;
    }
    ///*for(int i=0;i<10;i++)                           // PY not necessary. Uses a vector to hold equiplist stuff
    //{
    //    EquipList[i].nullequip = nullequip;
    //    for(UINT j=0;j<EquipList[i].max;j++)
    //        EquipList[i].Index[j] = nullequip;
    //}*/

    //natural null init
    CNaturalData* nullnatural = new CNaturalData;
    nullnatural->id					= 0;
	nullnatural->restriction		= 0;
    nullnatural->type				= 0;
    nullnatural->price				= 0;
    nullnatural->pricerate			= 0;
    nullnatural->weight				= 0;
    nullnatural->quality			= 0;
    nullnatural->pricevalue			= 0;
    nullnatural->craft_difficult	= 0;
    NaturalList.nullnatural			= nullnatural;

    for(UINT i=0;i<NaturalList.max;i++)
    {
        NaturalList.Index[i] = nullnatural;
    }

    //pat null init
    CPatData* nullpat = new CPatData;
    nullpat->id					= 0;
	nullpat->restriction		= 0;
    nullpat->type				= 0;
    nullpat->price				= 0;
    nullpat->pricerate			= 0;
    nullpat->weight				= 0;
    nullpat->quality			= 0;
    nullpat->craft_skill		= 0;
    nullpat->craft_level		= 0;
    nullpat->craft_recipe		= 0;
    nullpat->craft_difficult	= 0;
    nullpat->partversion		= 0;
    nullpat->maxfuel			= 0;
    nullpat->fuelcons			= 0;
    nullpat->speed				= 0;
    nullpat->attackdistance		= 0;
    nullpat->attackpower		= 0;
    nullpat->attackspeed		= 0;
    PatList.nullpat				= nullpat;
    for(UINT i=0;i<PatList.max;i++)
    {
        PatList.Index[i] = nullpat;
    }

    //sell null init
    CCSellData* nullsell = new CCSellData;
    nullsell->id = 0;
    for(UINT i=0;i<48;i++)
        nullsell->item[i] = 0;
    SellList.nullsell = nullsell ;

    for(UINT i=0;i<SellList.max;i++)
    {
        SellList.Index[i] = nullsell;
    }

    //map null init
    CMap* nullzone = new CMap( );
    nullzone->id			= 0;
    nullzone->dayperiod		= 1;
    nullzone->morningtime	= 0;
    nullzone->daytime		= 0;
    nullzone->eveningtime	= 0;
    nullzone->nighttime		= 0;
    nullzone->allowpvp		= 0;
    nullzone->allowpat		= 0;
    nullzone->MapTime		= 0;
    nullzone->LastUpdate	= 0;
    nullzone->CurrentTime	= 0;
    nullzone->nb_summons	= 0;
    for(UINT i=0;i<MapList.max;i++)
    {
        MapList.Index[i] = nullzone;
    }

    MapList.nullzone = nullzone;

    //Use null init
    CUseData* nulluse = new CUseData;
    nulluse->id					= 0;
    nulluse->restriction		= 0;
    nulluse->type				= 0;
    nulluse->price				= 0;
    nulluse->pricerate			= 0;
    nulluse->weight				= 0;
    nulluse->quality			= 0;
    nulluse->pricevalue			= 0;
    nulluse->craft_skill		= 0;
    nulluse->craft_level		= 0;
    nulluse->craft_difficult	= 0;
    nulluse->craft_recipe		= 0;
    nulluse->cooldown_type		= 0;
    nulluse->cooldown			= 0;
    nulluse->affected_Stat		= 0;
    nulluse->affected_Amount	= 0;
	nulluse->required_Stat		= 0;
	nulluse->required_Amount	= 0;
    UseList.nulluse				= nulluse;
    for(UINT i=0;i<UseList.max;i++)
    {
        UseList.Index[i] = nulluse;
    }

    //product null init
    CProductData* nullproduct = new CProductData;
    nullproduct->id = 0;
    //for(UINT i=0;i<50;i++)
    for(UINT i=0;i<4;i++)
    {
        nullproduct->item[i];
        nullproduct->amount[i];
    }

    nullproduct->item_0_family=0;

    ProductList.nullproduct = nullproduct;
    for(UINT i=0;i<ProductList.max;i++)
    {
        ProductList.Index[i] = nullproduct;
    }

    //jem null init
    CJemData* nulljem = new CJemData;
    nulljem->id					= 0;
	nulljem->restriction		= 0;
    nulljem->type				= 0;
    nulljem->price				= 0;
    nulljem->pricerate			= 0;
    nulljem->weight				= 0;
    nulljem->quality			= 0;
    nulljem->craft_skill		= 0;
    nulljem->craft_level		= 0;
    nulljem->craft_recipe		= 0;
    nulljem->craft_difficult	= 0;
    for(int i=0;i<2;i++)
    {
        nulljem->stat1[i]		= 0;
        nulljem->stat2[i]		= 0;
    }
    JemList.nulljem				= nulljem ;
    /*for(UINT i=0;i<JemList.max;i++)
    {
        JemList.Index[i]		= nulljem;
    }*/

    //LMA: Quest Items.
    CQuestItemData* nullQuestItem = new CQuestItemData;
    nullQuestItem->id			= 0;
    nullQuestItem->STLId		= 0;

    for (int k=0;k<maxQuestItems;k++)
    {
        QuestItemData[k] = nullQuestItem;
    }


    return true;
}

//LMA: loading Quest item Data.
bool CWorldServer::LoadQuestItemData( )
{
	Log( MSG_LOAD, "Quest Item Data - STB" );
    for (UINT i = 0; i<STB_ITEM[12].rowcount; i++)
    {

        CQuestItemData* newQuestItem = new (nothrow) CQuestItemData;
        if(newQuestItem==NULL)
        {
            Log( MSG_ERROR, "Error allocing memory" );
            continue;
        }

        newQuestItem->id=i;
        newQuestItem->STLId=STB_ITEM[12].rows[i][32];
        QuestItemData[newQuestItem->id]=newQuestItem;
    }

    STBFreeData(&STB_ITEM[12]);
    Log( MSG_LOAD, "Quest Item Data Loaded - STB" );


    return true;
}

//LMA: npc_data, STB version.
bool CWorldServer::LoadNPCData( )
{
    //LMA: Mass Exporter.
    if (Config.massexport==1)
    {
        NPC_AIP.clear();
    }

	Log( MSG_LOAD, "NPC Data - STB               " );
    for (UINT i = 0; i<STB_NPC.rowcount; i++)
    {
        CNPCData* newnpc = new (nothrow) CNPCData;
        if(newnpc==NULL)
        {
            Log( MSG_ERROR, "Error allocing memory" );
            continue;
        }

        newnpc->id				= i;
        newnpc->life			= 0;    //LMA: non sense, it's the name lol
        newnpc->stance			= mRUNNING;  //AIP
        //newnpc->name            = STB_NPC.rows[i][0];
        newnpc->wspeed			= STB_NPC.rows[i][2];
        newnpc->rspeed			= STB_NPC.rows[i][3];
        //newnpc->dspeed		= STB_NPC.rows[i][4]; //row 4 is monster size ...
        newnpc->weapon			= STB_NPC.rows[i][5];
        newnpc->subweapon		= STB_NPC.rows[i][6];
        newnpc->level			= STB_NPC.rows[i][7];
        newnpc->hp				= STB_NPC.rows[i][8];
        newnpc->atkpower		= STB_NPC.rows[i][9];
        newnpc->hitrate			= STB_NPC.rows[i][10];
        newnpc->defense			= STB_NPC.rows[i][11];
        newnpc->magicdefense	= STB_NPC.rows[i][12];
        newnpc->dodge			= STB_NPC.rows[i][13];
        newnpc->atkspeed		= STB_NPC.rows[i][14];
		newnpc->magicattack		= STB_NPC.rows[i][15];
        newnpc->AI				= STB_NPC.rows[i][16];
        newnpc->exp				= STB_NPC.rows[i][17];

        newnpc->dropid			= STB_NPC.rows[i][18];
        newnpc->money			= STB_NPC.rows[i][19];
        newnpc->item			= STB_NPC.rows[i][20];
        newnpc->tab1			= STB_NPC.rows[i][21];
        newnpc->tab2			= STB_NPC.rows[i][22];
        newnpc->tab3			= STB_NPC.rows[i][23];
        newnpc->specialtab		= STB_NPC.rows[i][24];

        newnpc->atkdistance		= (float)STB_NPC.rows[i][26] / 100.0f;
        if (newnpc->atkdistance < 1.5f) newnpc->atkdistance = 1.5f;
        //newnpc->aggresive		= STB_NPC.rows[i][27];				///No it isn't. column 27 is character type
        //newnpc->helpless=0;
        //newnpc->shp				= STB_NPC.rows[i][42];			// Column 42 is the Z offset. It has nothing to do with HP
        newnpc->dialogid		= 0;   //handled in list_npc now
        newnpc->eventid			= 0;   //handled in list_npc now
        newnpc->side			= 0;   //hidden
        newnpc->sidechance		= 0;   //hidden
        newnpc->STLId			= STB_NPC.rows[i][40];

        //LMA: test for quest hack (stackable).
        #ifdef QHACK
        newnpc->die_quest		= STB_NPC.rows[i][41];

        //Adding the quest to the list.
        if(newnpc->die_quest != 0 && (MapStackQuest.find(newnpc->die_quest) != MapStackQuest.end()))
        {
            MapStackQuest.insert ( pair<dword,int>(newnpc->die_quest,1) );
        }
        else if(newnpc->die_quest != 0)
        {
            MapStackQuest[newnpc->die_quest]++;
        }
        #endif

        //LMA: Various skills for monsters (won't be used anymore, will be done by AIP, for now left for compatibility).
        for(int m=0;m<4;m++)
        {
          newnpc->askills[m] = 0;
          newnpc->bskills[m] = 0;
          newnpc->dskills[m] = 0;
        }

        newnpc->lastskill = 0;
        newnpc->sigskill = 0;
        newnpc->delayskill = 0;

        NPCData[newnpc->id] = newnpc;

        //LMA: Mass Exporter.
        /*if (Config.massexport == 1)
        {
            NPC_AIP[newnpc->AI].push_back(newnpc->id);
        }*/

    }

    //LMA: test for quest hack (stackable).
    #ifdef QHACK
    STBFreeDataChar(&STB_NPC);
    #else
    STBFreeData(&STB_NPC);
    #endif

    Log( MSG_LOAD, "NPC Data loaded          STB" );


    return true;
}

//LMA: We give a NPC his timer.
bool CWorldServer::TimerForNPC( )
{
    for (int k=0;k<maxNPC;k++)
    {
        if (NPCData[k]->AI == 0)
        {
            continue;
        }
        NPCData[k]->AiTimer = GetTimerFromAIP(NPCData[k]->AI, false);
    }
    return true;
}

//old version (sql).
//bool CWorldServer::LoadSkillDataOld( )															//No function calls so commented
//{
//    //test stuff.
//
//    return true;
//}

//LMA: Stuff... Used for tests :)
//bool CWorldServer::LMACheckStuff()																//No function calls so commented
//{
//    //test test :)
//
//    return true;
//}


//LMA: We compare STB and SQL "skills" to check differences (for debug purpose only!!!).			//No function calls so commented
//bool CWorldServer::LMACheckSkills()
//{
//    //test.
//
//    return true;
//}

//loading status.
bool CWorldServer::LoadStatusData( )
{
  Log( MSG_LOAD, "Stats Data             " );
  for (unsigned int i = 0; i < STB_STATUS.rowcount; i++)
  {
    CStatus* newstatus = new (nothrow) CStatus;
    if (newstatus == NULL)
    {
      Log(MSG_WARNING, "\nError allocing memory: status_data");
      return false;
    }
    newstatus->id = i;
    newstatus->type = STB_STATUS.rows[i][1];  // Unk
    newstatus->duplication = STB_STATUS.rows[i][2]; // Unk
    newstatus->dir = STB_STATUS.rows[i][3]; // 0: Up 1: Down 2: ?
    newstatus->repeat = STB_STATUS.rows[i][4]; // 1: Repeat (Heal) 2: Once (Buff Stat) 3: Status Effect (Poison, etc)
    newstatus->ability[0] = STB_STATUS.rows[i][5]; // Status # to call?
    newstatus->amount[0] = STB_STATUS.rows[i][6]; // Amount
    newstatus->ability[1] = STB_STATUS.rows[i][7];
    newstatus->amount[1] = STB_STATUS.rows[i][8];
    newstatus->decrease = STB_STATUS.rows[i][17]; // 0: Increase 1: Decrease
    StatusList[i]=newstatus;
  }

  Log( MSG_LOAD, "Stats Data loaded       " );
  return true;
}


//News version (STB)
bool CWorldServer::LoadSkillData( )
{
	Log( MSG_LOAD, "Skills data - STB        " );
	//Log (MSG_DEBUG, "Loading skills data");
    for (unsigned int i = 0; i<STB_SKILL.rowcount;i++)
    {
        CSkills* newskill = new (nothrow) CSkills;
        if(newskill == NULL)
        {
            Log(MSG_WARNING, "\nError allocing memory: skills_data" );
            return false;
        }
        char *tmp;
        newskill->id		= i;
		newskill->BaseID	= STB_SKILL.rows[i][1];		// The skill id of this skill at level 1
        newskill->level		= STB_SKILL.rows[i][2];		// Skills Level
        newskill->sp		= STB_SKILL.rows[i][3];     // Cost to get skill
        newskill->type		= STB_SKILL.rows[i][4];		// tab type.
        newskill->skill_tab = STB_SKILL.rows[i][4];		// tab type.		//Why do we load tab type into 2 places?
        newskill->skilltype = STB_SKILL.rows[i][5];		// Type of skill
        newskill->range		= STB_SKILL.rows[i][6]/100; // Range of skill
        newskill->target	= STB_SKILL.rows[i][7];		// Skill Target
        newskill->aoerange	= STB_SKILL.rows[i][8]/100; // AOE Range
        newskill->atkpower	= STB_SKILL.rows[i][9];     // Attack Power
        newskill->gm_aoe	= 0;						// LMA: GM AOE buff

        //LMA: Patch for osRose.
        //For us type is skilltype...
        newskill->type		= newskill->skilltype;

        //LMA: Not the same IDs in ospRose and osRose...
        /*
        newskill->status[0] = STB_SKILL.rows[i][11];  // status
        newskill->status[1] = STB_SKILL.rows[i][12];  // status
        */
        newskill->status[0] = STB_SKILL.rows[i][88];  // status
        newskill->status[1] = STB_SKILL.rows[i][92];  // status
        newskill->status[2] = STB_SKILL.rows[i][96];  // status

        newskill->success = STB_SKILL.rows[i][13];    // Success Rate
        if(newskill->success == 0)
        {
            newskill->success = 100;                //success rate is stored as NULL where it is 100%. eg dual scratch
        }

        newskill->duration		= STB_SKILL.rows[i][14];	// Duration
        newskill->formula		= STB_SKILL.rows[i][15];	//skill type magical or weapon
        newskill->costtype[0]	= STB_SKILL.rows[i][16];	//not all costs are in MP
        newskill->costamount[0] = STB_SKILL.rows[i][17];	//some are in HP or Stamina
        newskill->mp			= STB_SKILL.rows[i][17];
        newskill->costtype[1]	= STB_SKILL.rows[i][18];	// All driveby skills have 2 costs
        newskill->costamount[1] = STB_SKILL.rows[i][19];	// The second is normally cart gauge
        newskill->cooldown		= STB_SKILL.rows[i][20]/5;  // The time it takes to be able to use the skill again. (LMA: /5...)
        //newskill->success=STB_SKILL.rows[i][69];			//	Dunno what this field is really but it sure isn't success rate <_<	//PY it's relative speed of the animation. 100 = normal

		// Warp skill
		newskill->WarpData		= { float (STB_SKILL.rows[i][22] * 10), float (STB_SKILL.rows[i][23] * 10) };
		newskill->WarpZone		= STB_SKILL.rows[i][21];				//Map id where the skill takes you (see return scrolls)
		newskill->WarpX			= float (STB_SKILL.rows[i][22] * 10);
		newskill->WarpY			= float (STB_SKILL.rows[i][23] * 10);
		//end Warp skill

        newskill->weapon[0] = STB_SKILL.rows[i][30];    // Item Type must be equipped (Only 1)
        newskill->weapon[1] = STB_SKILL.rows[i][31];    // Item Type must be equipped
        newskill->weapon[2] = STB_SKILL.rows[i][32];    // Item Type must be equipped
        newskill->weapon[3] = STB_SKILL.rows[i][33];    // Item Type must be equipped
        newskill->weapon[4] = STB_SKILL.rows[i][34];    // Item Type must be equipped
        newskill->c_class = STB_SKILL.rows[i][35];   // Required Class (Only One)
        //newskill->c_class[1] = STB_SKILL.rows[i][36];   // Required Class						//PY NOPE!! only column 35 relates to required class. the other 3 are for Union skills which are not used in RosE Resurrected
        //newskill->c_class[2] = STB_SKILL.rows[i][37];   // Required Class
        //newskill->c_class[3] = STB_SKILL.rows[i][38];   // Required Class
        //newskill->c_class[4] = 0;
        newskill->rskill[0] = STB_SKILL.rows[i][39];    // Required Skill
        newskill->rskill[1] = STB_SKILL.rows[i][41];    // Required Skill
        newskill->rskill[2] = STB_SKILL.rows[i][43];    // Required Skill
        newskill->lskill[0] = STB_SKILL.rows[i][40];    // Required Skill Level
        newskill->lskill[1] = STB_SKILL.rows[i][42];    // Required Skill Level
        newskill->lskill[2] = STB_SKILL.rows[i][44];    // Required Skill Level

        newskill->buff[0] = STB_SKILL.rows[i][89];      // Stat
        newskill->value1[0] = STB_SKILL.rows[i][90];    // Int Value
        newskill->value2[0] = STB_SKILL.rows[i][91];    // % Value
        newskill->buff[1] = STB_SKILL.rows[i][93];      // Stat
        newskill->value1[1] = STB_SKILL.rows[i][94];    // Int Value
        newskill->value2[1] = STB_SKILL.rows[i][95];    // % Value
        newskill->buff[2] = STB_SKILL.rows[i][97];      // Stat
        newskill->value1[2] = STB_SKILL.rows[i][98];    // Int Value
        newskill->value2[2] = STB_SKILL.rows[i][99];    // % Value

        newskill->req[0] = STB_SKILL.rows[i][45];    //the requirement type (usually 31 = level)
        newskill->clevel = STB_SKILL.rows[i][46];    //requirement amount
        newskill->reqam[0] = STB_SKILL.rows[i][46];  //this need no always be level but is usually
        newskill->req[1] = STB_SKILL.rows[i][47];    //the requirement type (in most cases blank)
        newskill->reqam[1] = STB_SKILL.rows[i][48];  //requirement amount
        newskill->zuly = (STB_SKILL.rows[i][85] * 100);    // Required zuly

        //LMA: STL:
        newskill->STLId = STB_SKILL.rows[i][113];

        if ((STB_SKILL.rows[i][8])>0)
        {
            newskill->aoe = 1;
            newskill->aoeradius = STB_SKILL.rows[i][8]/100;
        }
        else
        {
            newskill->aoe = 0;
            newskill->aoeradius = 0;
        }
        if ((STB_SKILL.rows[i][28]) > 0)
        {
            newskill->script = 1;
            newskill->svalue1 = STB_SKILL.rows[i][28];
        }
        else
        {
            newskill->script = 0;
            newskill->svalue1 = 0;
        }

        //LMA: Patch for some skills (?)
		// Reflect attack, enhance damage, enhance damage (again)
        if(newskill->status[0] == 35 || newskill->status[0] == 54 || newskill->status[0] == 83)
        {
            if(newskill->buff[0] == 0 && newskill->value1[0] == 0)
            {
                newskill->buff[0] = newskill->status[0];
                newskill->value1[0] = newskill->atkpower;
                //Log(MSG_INFO,"Skill (%i) status0=%i, buff0=%i, value=%i",i,newskill->status[0],newskill->buff[0],newskill->value1[0]);
            }

        }
        //end of patch.

        newskill->nbuffs = 0;
        if(newskill->buff[0] != 0 || newskill->status[0] != 0)
        {
            newskill->nbuffs += 1;
        }

        if(newskill->buff[1]!= 0 || newskill->status[1] != 0)
        {
            newskill->nbuffs += 2;
        }

        //LMA: add the [2]?
        if(newskill->buff[2] != 0 || newskill->status[2] != 0)
        {
            newskill->nbuffs += 4;
        }

		//PY we need to update the max level value for all previous levels of this skill
		SkillList[i] = newskill;									// Add newskill to the array first. Then modify it
		if(newskill->level > 1 && newskill->id != newskill->BaseID)	// Not the minimum level so we need to scan back over the earlier entries and change the max value
		{ 
			int start = newskill->BaseID;							// set the startpoint to the first skill id of this skill
			for (int index = start; index < i; index++)			// start scanning
			{
				if(SkillList[index]->BaseID == newskill->BaseID)	// Make sure it's the same skill. just an earlier version of it
				{ 
					SkillList[index]->MaxLevel = newskill->level;
				}
			}
			SkillList[i]->MaxLevel = newskill->level;
		}
		else
		{															// If the new skill is level 1 then set the max level to 1 as well. We will come back and change it a few times later
			newskill->MaxLevel = newskill->level;					// does nothing if the skill entry is blank
		}
		GServer->MaximumSkillID = i;								// track the number of rows in the skill STB
		//Log (MSG_DEBUG, "Loaded skill id %i baseID %i current level %i max level %i ref-Max %i", newskill->id, newskill->BaseID, newskill->level, SkillList[i]->MaxLevel, SkillList[newskill->BaseID]->MaxLevel);
	}
	
	Log( MSG_LOAD, "Skills Data - STB loaded" );


    return true;
}


bool CWorldServer::LoadTeleGateData( )
{
	Log( MSG_LOAD, "Telegates data              " );
	MYSQL_ROW row;

    //LMA: Pegasus
    MYSQL_RES *result=NULL;
    if(Config.is_pegasus==1)
    {
        result = DB->QStore("SELECT id, srcx, srcy, srcmap, destx, desty, destmap FROM list_telegates_p ORDER BY id ASC");
    }
    else
    {
        result = DB->QStore("SELECT id, srcx, srcy, srcmap, destx, desty, destmap FROM list_telegates ORDER BY id ASC");
    }

	if(result==NULL) return false;

	int last_id=0;
	int nb_offset=0;

	while( row = mysql_fetch_row(result) )
    {
		CTeleGate* thisgate = new (nothrow) CTeleGate;
        if(thisgate==NULL)
        {
            Log(MSG_ERROR, "Error allocing memory       " );
            DB->QFree( );
            return false;
        }

		thisgate->id = atoi(row[0]);
		thisgate->src.x = (float)atof(row[1]);
		thisgate->src.y = (float)atof(row[2]);
		thisgate->srcMap = atoi(row[3]);
		thisgate->dest.x = (float)atof(row[4]);
		thisgate->dest.y = (float)atof(row[5]);
		thisgate->destMap = atoi(row[6]);
        //thisgate->dest.x = (float)atof(row[1]);
        //thisgate->dest.y = (float)atof(row[2]);
        //thisgate->destMap = atoi(row[3]);

        if (last_id!=thisgate->id)
        {
            last_id=thisgate->id;
            nb_offset=0;
        }
        else
        {
            nb_offset++;
        }

        thisgate->offset=nb_offset;

		TeleGateList.push_back( thisgate );
	}

	DB->QFree( );
	Log( MSG_LOAD, "Telegates Data loaded" );


	return true;
}

//Loading Quests
bool CWorldServer::LoadQuestSTB()
{
    STBStoreData( "3ddata/stb/LIST_QUEST.STB", &STB_QUEST );
    Log(MSG_INFO,"STB Quest data loaded");
    return true;
}


bool CWorldServer::LoadRespawnData( )
{
	Log( MSG_LOAD, "RespawnZones data           " );
	MYSQL_ROW row;
	MYSQL_RES *result = DB->QStore("SELECT id,x,y,map,radius,type FROM list_respawnzones");
    if(result==NULL) return false;
	while( row = mysql_fetch_row(result) )
    {
		CRespawnPoint* thisrespawnpoint = new (nothrow) CRespawnPoint;
        if(thisrespawnpoint==NULL)
        {
            Log(MSG_ERROR, "Error allocing memory" );
            DB->QFree( );
            return false;
        }
		thisrespawnpoint->id = atoi(row[0]);
		thisrespawnpoint->dest.x = (float)atof(row[1]);
		thisrespawnpoint->dest.y = (float)atof(row[2]);
		thisrespawnpoint->destMap = atoi(row[3]);
		thisrespawnpoint->radius = atoi(row[4]);
		thisrespawnpoint->masterdest = (atoi(row[5]) == 1);

        //LMA: check if out of memory.
        if (thisrespawnpoint->destMap>=MapList.max)
        {
           Log(MSG_WARNING,"RespawnZones, index overflow trapped %i>%i (should not happen)",thisrespawnpoint->destMap,MapList.max);
           delete thisrespawnpoint;
           continue;
        }

		MapList.Index[thisrespawnpoint->destMap]->RespawnList.push_back( thisrespawnpoint );
	}
	DB->QFree( );
	Log( MSG_LOAD, "RespawnZones Data loaded" );
	return true;
}

bool CWorldServer::LoadMobGroups()
{
  Log(MSG_LOAD, "MobGroups data    " );
  //vector<CMobGroup*> mobGroups;
  MYSQL_ROW row;
  bool flag = true;
  char* tmp = NULL;

  //LMA: Day and night (for Halloween)
  //For pegasus too
  MYSQL_RES *result=NULL;
    if(Config.is_pegasus==1)
    {
        result = DB->QStore("SELECT `id`, `map`, `x`, `y`, `range`, `respawntime`, `limit`, `tacticalpoints`, `moblist`,`daynight`,`isactive`  FROM `list_mobgroups_p`");
    }
    else
    {
        result = DB->QStore("SELECT `id`, `map`, `x`, `y`, `range`, `respawntime`, `limit`, `tacticalpoints`, `moblist`,`daynight`,`isactive`  FROM `list_mobgroups`");
    }

  if (result == NULL) return false;
  while (row = mysql_fetch_row(result))
  {
    //LMA: Active spawn?
    int is_active=atoi(row[10]);
    if(is_active==0)
        continue;

    CMobGroup* thisgroup = new (nothrow) CMobGroup;
    if (thisgroup == NULL)
    {
      Log(MSG_ERROR, "Error allocating memory");
      DB->QFree();
      return false;
    }

    thisgroup->id = atoi(row[0]);
    thisgroup->map = atoi(row[1]);
    thisgroup->point.x = atof(row[2]);
    thisgroup->point.y = atof(row[3]);
    thisgroup->range = atoi(row[4]);
    thisgroup->respawntime = atoi(row[5]);
    thisgroup->limit = atoi(row[6]);
    thisgroup->tacticalpoints = atoi(row[7]);

    char* mobList = row[8];

    //LMA: Day and night (for Halloween) 0=day and night, 1=day, 2=night
    thisgroup->daynight=atoi(row[9]);

    thisgroup->lastRespawnTime = clock();
    thisgroup->active = 0;
    thisgroup->basicKills = 0;
    thisgroup->lastKills=0;
    thisgroup->curTac = 0;
    thisgroup->curBasic = 0;
    thisgroup->group_ready=false;


    thisgroup->basicMobs.clear();
    thisgroup->tacMobs.clear();

    //LMA: resetting the error flag
    flag=true;

    // Fill in basic/tac mobs
    tmp = strtok(mobList, ",|");
    while (tmp != NULL)
    {

      int mobId = atoi(tmp);
      tmp = strtok(NULL, ",|");
      if (tmp == NULL) {
        Log(MSG_ERROR, "MobGroup %i is invalid", thisgroup->id);
        flag = false;
        break;
      }
      int amount = atoi(tmp);
      tmp = strtok(NULL, ",|");
      if (tmp == NULL) {
        Log(MSG_ERROR, "MobGroup %i is invalid", thisgroup->id);
        flag = false;
        break;
      }
      int tactical = atoi(tmp);
      CMob *thismob = new (nothrow) CMob;
      if (thismob == NULL) {
        Log(MSG_ERROR, "Error allocating memory");
        DB->QFree();
        return false;
      }
      thismob->amount = amount;
      thismob->real_amount=1;
      thismob->tactical = tactical;
      thismob->mobId = mobId;
      thismob->thisnpc = GetNPCDataByID( thismob->mobId );

      //LMA: check
      if(thisgroup->limit<thismob->amount)
      {
          //Log(MSG_WARNING,"spawn %i: limit %u < monster amount %u",thisgroup->id,thisgroup->limit,thismob->amount);
          //overwriting.
          thismob->amount=thisgroup->limit;
      }

      //LMA: We check here and delete the whole group.
      if (thismob->thisnpc == NULL)
      {
        Log(MSG_WARNING, "Group %i:: Invalid monster %i",thisgroup->id,thismob->mobId);
        flag=false;
        break;
      }

    //LMA: shouldn't be PY'drop used in those ones?
      thismob->mapdrop=NULL;
      thismob->mobdrop=NULL;

    /*
    //Org code
      thismob->mapdrop = GetDropData( thisgroup->map );
      thismob->mobdrop= GetDropData( thismob->thisnpc->dropid );
      */

      if (thismob->tactical)
        thisgroup->tacMobs.push_back(thismob);
      else
        thisgroup->basicMobs.push_back(thismob);
      tmp = strtok(NULL, ",|");
     }

        if (!flag)
        {
          delete thisgroup;
          continue;
        }

        MapList.Index[thisgroup->map]->MobGroupList.push_back(thisgroup);
        //mobGroups.push_back(thisgroup);
    }

    DB->QFree( );


    return true;
}

//LMA: Special Spawns (like Halloween or whatever...)
/*
bool CWorldServer::LoadMobGroupsSpecial()
{
  Log(MSG_LOAD, "MobGroupsSpecial data    " );
  //vector<CMobGroup*> mobGroups;
  MYSQL_ROW row;
  bool flag = true;
  char* tmp = NULL;
  //LMA: Day and night (for Halloween)
  MYSQL_RES *result = DB->QStore("SELECT `id`, `map`, `x`, `y`, `range`, `respawntime`, `limit`, `tacticalpoints`, `moblist`,`daynight`,`isactive`  FROM `list_mobgroups_special`");
  if (result == NULL) return false;
  while (row = mysql_fetch_row(result))
  {

    //LMA: Active spawn?
    int is_active=atoi(row[10]);
    if(is_active==0)
        continue;

    CMobGroup* thisgroup = new (nothrow) CMobGroup;
    if (thisgroup == NULL)
    {
      Log(MSG_ERROR, "Error allocating memory");
      DB->QFree();
      return false;
    }

    thisgroup->id = atoi(row[0]);
    thisgroup->map = atoi(row[1]);
    thisgroup->point.x = atof(row[2]);
    thisgroup->point.y = atof(row[3]);
    thisgroup->range = atoi(row[4]);
    thisgroup->respawntime = atoi(row[5]);
    thisgroup->limit = atoi(row[6]);
    thisgroup->tacticalpoints = atoi(row[7]);

    char* mobList = row[8];

    //LMA: Day and night (for Halloween) 0=day and night, 1=day, 2=night
    thisgroup->daynight=atoi(row[9]);

    thisgroup->lastRespawnTime = clock();
    thisgroup->active = 0;
    thisgroup->basicKills = 0;
    thisgroup->curTac = 0;
    thisgroup->curBasic = 0;


    thisgroup->basicMobs.clear();
    thisgroup->tacMobs.clear();

    //LMA: resetting the error flag
    flag=true;

    // Fill in basic/tac mobs
    tmp = strtok(mobList, ",|");
    while (tmp != NULL)
    {

      int mobId = atoi(tmp);
      tmp = strtok(NULL, ",|");
      if (tmp == NULL) {
        Log(MSG_ERROR, "MobGroup %i is invalid", thisgroup->id);
        flag = false;
        break;
      }
      int amount = atoi(tmp);
      tmp = strtok(NULL, ",|");
      if (tmp == NULL) {
        Log(MSG_ERROR, "MobGroup %i is invalid", thisgroup->id);
        flag = false;
        break;
      }
      int tactical = atoi(tmp);
      CMob *thismob = new (nothrow) CMob;
      if (thismob == NULL) {
        Log(MSG_ERROR, "Error allocating memory");
        DB->QFree();
        return false;
      }
      thismob->amount = amount;
      thismob->tactical = tactical;
      thismob->mobId = mobId;
      thismob->thisnpc = GetNPCDataByID( thismob->mobId );

      //LMA: We check here and delete the whole group.
      if (thismob->thisnpc == NULL)
      {
        Log(MSG_WARNING, "Group %i:: Invalid monster %i",thisgroup->id,thismob->mobId);
        flag=false;
        break;
      }

    //LMA: shouldn't be PY'drop used in those ones?
      thismob->mapdrop=NULL;
      thismob->mobdrop=NULL;


    //Org code
      //thismob->mapdrop = GetDropData( thisgroup->map );
      //thismob->mobdrop= GetDropData( thismob->thisnpc->dropid );


      if (thismob->tactical)
        thisgroup->tacMobs.push_back(thismob);
      else
        thisgroup->basicMobs.push_back(thismob);
      tmp = strtok(NULL, ",|");
     }

        if (!flag)
        {
          delete thisgroup;
          continue;
        }

        MapList.Index[thisgroup->map]->MobGroupList.push_back(thisgroup);
        //mobGroups.push_back(thisgroup);
    }

    DB->QFree( );


    return true;
}
*/


//LMA: outdated.
//PY Reworking code for a new spawn system based more accurately on IFO data
bool CWorldServer::LoadMonsterSpawn( )
{
	Log( MSG_INFO, "Loading IFO SpawnZones data" );
    //clear the respawns data first

    for(int i=0;i<MapList.Map.size();i++)
    {
        MapList.Index[i]->MonsterSpawnList.clear();
    }

	MYSQL_ROW row;
	//MYSQL_RES *result = DB->QStore("SELECT id,map,montype,min,max,respawntime,points,triggeramount,bossid FROM list_spawnareas");
	MYSQL_RES *result = DB->QStore("SELECT spawnid,mapid,radius,spawn_x,spawn_y,tactic,bmob1,bmob2,bmob3,bmob4,bmob5,bcount1,bcount2,bcount3,bcount4,bcount5,tmob1,tmob2,tcount1,tcount2,spawn_interval FROM spawns");
	if(result == NULL) return false;
	int count = 0;
	while(row = mysql_fetch_row(result))
    {
        if (atoi(row[11]) == 0) continue; //skip this entry if bcount1 = 0
        //if ( count >10782)
        //    continue;
        bool flag = true;
		char* tmp;
		CSpawnArea* thisspawn = new (nothrow) CSpawnArea;
        if(thisspawn == NULL)
        {
            Log(MSG_ERROR, "Error allocing memory" );
            DB->QFree( );
            return false;
        }
		thisspawn->id = atoi(row[0]);
		thisspawn->map = atoi(row[1]);
		thisspawn->radius = atoi(row[2]);
		thisspawn->points.x = atof(row[3]);
        thisspawn->points.y = atof(row[4]);
        thisspawn->tactic = atoi(row[5]);
        thisspawn->bmob[1] = atoi(row[6]);
        thisspawn->bmob[2] = atoi(row[7]);
        thisspawn->bmob[3] = atoi(row[8]);
        thisspawn->bmob[4] = atoi(row[9]);
        thisspawn->bmob[5] = atoi(row[10]);
        thisspawn->bcount[1] = atoi(row[11]);
        thisspawn->bcount[2] = atoi(row[12]);
        thisspawn->bcount[3] = atoi(row[13]);
        thisspawn->bcount[4] = atoi(row[14]);
        thisspawn->bcount[5] = atoi(row[15]);
        thisspawn->tmob[1] = atoi(row[16]);
        thisspawn->tmob[2] = atoi(row[17]);
        thisspawn->tcount[1] = atoi(row[18]);
        thisspawn->tcount[2] = atoi(row[19]);
        thisspawn->respawntime = atoi(row[20]);
        thisspawn->ResetSpawn = false;

		//check that all the monsters in the spawn are valid
		for(int i=1;i==5;i++)
        {
            if(thisspawn->bmob[i] == 0) continue; //skip the check if no monster defined
            CNPCData* tempnpc;
            tempnpc = GetNPCDataByID( thisspawn->bmob[i] );
            if(tempnpc == NULL)
            {
                Log( MSG_WARNING, "Invalid montype %i for bmob %i in spawn ID %i",thisspawn->bmob[i],i,thisspawn->id );
                flag = false;
            }
        }
        for(int i=1;i==2;i++)
        {
            if(thisspawn->tmob[i] == 0) continue; //skip the check if no monster defined
            CNPCData* tempnpc;
            tempnpc = GetNPCDataByID( thisspawn->tmob[i] );
            if(tempnpc == NULL)
            {
                Log( MSG_WARNING, "Invalid montype %i for bmob %i in spawn ID %i",thisspawn->tmob[i],i,thisspawn->id );
                flag = false;
            }
        }
        //if(count > 10780)

		thisspawn->amon = 0;

		thisspawn->lastRespawnTime = clock();

        if(!flag)
        {

            delete thisspawn;
            continue;
        }

        //LMA: check if out of memory.
        if (thisspawn->map >= MapList.max)
        {
           Log(MSG_WARNING,"Spawn, index overflow trapped %i>%i (should not happen)",thisspawn->map,MapList.max);
           delete thisspawn;
           continue;
        }
        count++;
        MapList.Index[thisspawn->map]->MonsterSpawnList.push_back( thisspawn );
	}
	Log( MSG_INFO, "Loaded %i entries from Spawns table", count );
	DB->QFree( );
	Log( MSG_LOAD, "SpawnZones Data loaded" );
	return true;
}


bool CWorldServer::LoadNPCs( )
{
	Log( MSG_LOAD, "NPC spawn                   " );
	MYSQL_ROW row;
	NpcNameList.clear();

    //LMA: Pegasus
    MYSQL_RES *result=NULL;
    if(Config.is_pegasus==1)
    {
        result = DB->QStore("SELECT type,map,dir,x,y,dialogid,eventid,tempdialogid,name FROM list_npcs_p ORDER BY id");
    }
    else
    {
        result = DB->QStore("SELECT type,map,dir,x,y,dialogid,eventid,tempdialogid,name,extra_param FROM list_npcs ORDER BY id");
    }

	if(result==NULL) return false;
	while(row = mysql_fetch_row(result))
    {
		CNPC* thisnpc = new (nothrow) CNPC;
        if(thisnpc==NULL)
        {
            Log(MSG_ERROR, "Error allocing memory" );
            DB->QFree( );
            return false;
        }

		thisnpc->clientid = GetNewClientID();
		thisnpc->npctype = atoi(row[0]);
		thisnpc->posMap = atoi(row[1]);
		thisnpc->dir = (float)atof(row[2]);
		thisnpc->pos.x = (float)atof(row[3]);
		thisnpc->pos.y = (float)atof(row[4]);
		thisnpc->thisnpc = GetNPCDataByID( thisnpc->npctype );
		thisnpc->name = row[8];
		if( thisnpc->thisnpc == NULL)
		{
           Log(MSG_LOAD,"The NPC %i has not been found!, it won't be displayed",thisnpc->npctype);
            delete thisnpc;
            continue;
        }

        NpcNameList[thisnpc->npctype]=_strdup(row[8]);    //LMA: NPC Name list.
		thisnpc->name = row[8];

        thisnpc->thisnpc->dialogid = atoi(row[5]); //This is global to NPC type (original dialog)
        //thisnpc->dialog=thisnpc->thisnpc->dialogid;
        //thisnpc->event=thisnpc->thisnpc->eventid; //LMA Event.
        thisnpc->event=atoi(row[6]);                //LMA Event.
        thisnpc->thisnpc->eventid=thisnpc->event;            //LMA Event (left for compatibility).
        ObjVar[thisnpc->npctype][0]=thisnpc->thisnpc->eventid;  //LMA: eventid for AIP / QSD.

        //LMA: Special step for Williams.
        if(thisnpc->npctype==1075)
        {
            ObjVar[thisnpc->npctype][1]=atoi(row[9]);
            Log(MSG_INFO,"Infos for Williams: %i, %i");
            LastTempleAccess[0]=ObjVar[thisnpc->npctype][0];
            LastTempleAccess[1]=ObjVar[thisnpc->npctype][1];
        }

        thisnpc->dialog=atoi(row[7]);               //LMA tempdialog ID, used for events for example

        //LMA: check if out of memory.
        if (thisnpc->posMap>=MapList.max)
        {
           Log(MSG_WARNING,"NPC, index overflow trapped %i>%i (should not happen)",thisnpc->posMap,MapList.max);
           delete thisnpc;
           continue;
        }

        thisnpc->lastAiUpdate=clock();
		MapList.Index[thisnpc->posMap]->AddNPC( thisnpc );
	}

	DB->QFree( );

	//LMA: Adding WarpGate as "Virtual NPC" :)
	CNPC* thisnpc = new (nothrow) CNPC;
    thisnpc->clientid = GetNewClientID();
    thisnpc->npctype = WarpGate.virtualNpctypeID;
    thisnpc->posMap = 2;
    thisnpc->dir = 0;
    thisnpc->pos.x = WarpGate.IfoPosition.x;
    thisnpc->pos.y = WarpGate.IfoPosition.y;
    thisnpc->dialog=0;
    thisnpc->event=0;
    thisnpc->thisnpc = GetNPCDataByID( thisnpc->npctype );
    thisnpc->thisnpc->eventid=0;

    //LMA: check if out of memory.
    if (thisnpc->posMap>=MapList.max)
    {
       Log(MSG_WARNING,"NPC, index overflow trapped %i>%i (should not happen) WarpGate",thisnpc->posMap,MapList.max);
       delete thisnpc;
    }
    else
    {
        MapList.Index[thisnpc->posMap]->AddNPC( thisnpc );
    }

	WarpGate.clientID=thisnpc->clientid;
	WarpGate.Npc=thisnpc;
	//End WarpGate.
	Log( MSG_LOAD, "NPC spawn Data loaded" );


	return true;
}


//LMA: Special NPCs (Events or whatever...)
bool CWorldServer::LoadNPCsSpecial( )
{
    Log( MSG_LOAD, "NPC Special           " );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT type,map,dir,x,y,dialogid,eventid,tempdialogid,name,isactive,buffbot,buffpower FROM list_npcs_special where isactive='1' ");
    if(result==NULL) return false;
    while(row = mysql_fetch_row(result))
    {
        //LMA: Active NPC?
        int is_active = atoi(row[9]);
        if(is_active == 0)
        {
            continue;
        }

        CNPC* thisnpc = new (nothrow) CNPC;
        if(thisnpc==NULL)
        {
            Log(MSG_ERROR, "Error allocing memory" );
            DB->QFree( );
            return false;
        }

        thisnpc->clientid = GetNewClientID();
        thisnpc->npctype = atoi(row[0]);
        thisnpc->posMap = atoi(row[1]);
        thisnpc->dir = (float)atof(row[2]);
        thisnpc->pos.x = (float)atof(row[3]);
        thisnpc->pos.y = (float)atof(row[4]);
        thisnpc->thisnpc = GetNPCDataByID( thisnpc->npctype );
		thisnpc->buffbot = atoi (row[10]);
		thisnpc->buffpower = atoi (row[11]);
        if( thisnpc->thisnpc == NULL)
        {
           Log(MSG_LOAD,"The NPC %i has not been found!, it won't be displayed",thisnpc->npctype);
            delete thisnpc;
            continue;
        }

        NpcNameList[thisnpc->npctype] = _strdup(row[8]);    //LMA: NPC Name list.

        thisnpc->thisnpc->dialogid = atoi(row[5]); //This is global to NPC type (original dialog)
        //thisnpc->dialog=thisnpc->thisnpc->dialogid;
        //thisnpc->event=thisnpc->thisnpc->eventid; //LMA Event.
        thisnpc->event=atoi(row[6]);                //LMA Event.
        thisnpc->thisnpc->eventid = thisnpc->event;            //LMA Event (left for compatibility).
        thisnpc->dialog=atoi(row[7]);               //LMA tempdialog ID, used for events for example

        //LMA: check if out of memory.
        if (thisnpc->posMap >= MapList.max)
        {
           Log(MSG_WARNING,"NPC, index overflow trapped %i>%i (should not happen)",thisnpc->posMap,MapList.max);
           delete thisnpc;
           continue;
        }

        thisnpc->lastAiUpdate = clock();
        MapList.Index[thisnpc->posMap]->AddNPC( thisnpc );
    }

    DB->QFree( );
    Log( MSG_LOAD, "NPC Special loaded" );
    return true;
}

//LMA: Special Events (original ones)
//We modify the original NPCs by the new ones...
bool CWorldServer::LoadNPCsEvents( )
{
    Log( MSG_LOAD, "NPC Special Events          " );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT type,dialogid,eventid,activekeyword FROM list_events WHERE Active='1' ");
    if(result==NULL) return false;
    while(row = mysql_fetch_row(result))
    {
        CNPC* thisnpc = GetNPCByType(atoi(row[0]));
        if( thisnpc->thisnpc == NULL)
        {
           Log(MSG_LOAD,"The NPC %i has not been found!, it won't be displayed",thisnpc->npctype);
            delete thisnpc;
            continue;
        }

        //NpcNameList[thisnpc->npctype]=strdup(row[3]);    //LMA: NPC Name list.
        //thisnpc->dialog=thisnpc->thisnpc->dialogid;
        //thisnpc->event=thisnpc->thisnpc->eventid; //LMA Event.
        //thisnpc->thisnpc->dialogid = atoi(row[1]); //This is global to NPC type (original dialog)
        thisnpc->event=atoi(row[2]);                //LMA Event.
        thisnpc->thisnpc->eventid=thisnpc->event;   //LMA Event (left for compatibility).
        thisnpc->dialog=atoi(row[1]);               //LMA tempdialog ID, used for events for example
        thisnpc->lastAiUpdate=clock();
        Log(MSG_INFO,"Loading Event %s:: NPC %u, dialog %u, event %u",row[3],atoi(row[0]),thisnpc->dialog,thisnpc->event);
    }

    DB->QFree( );
    Log( MSG_LOAD, "NPC Special Events" );
    return true;
}

//hidden
bool CWorldServer::LoadPYDropsData( )
{
   	Log( MSG_LOAD, "PYDrops Data                " );
    MDropList.clear();
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT id,type,min_level,max_level,prob,mob,map,alt,ref FROM item_drops");
    if(result == NULL)
    {
        DB->QFree( );
        return false;
    }

    //Creating indexes
    int nb_max_itemtype[15];
    nb_max_itemtype[0]=0;
    nb_max_itemtype[13]=0;

    for(int j=1;j<10;j++)
    {
        nb_max_itemtype[j]=EquipList[j].max;
    }

    nb_max_itemtype[10]=UseList.max;
    nb_max_itemtype[11]=JemList.max;
    nb_max_itemtype[12]=NaturalList.max;
    nb_max_itemtype[14]=PatList.max;


    while(row = mysql_fetch_row(result))
    {
        CMDrops* newdrop = new (nothrow) CMDrops;
        assert(newdrop);
        newdrop->itemnum = atoi(row[0]);
        newdrop->itemtype = atoi(row[1]);
        newdrop->level_min = atoi(row[2]);
        newdrop->level_max = atoi(row[3]);
        newdrop->prob = atoi(row[4]);
        newdrop->mob = atoi(row[5]);
        newdrop->map = atoi(row[6]);

        int record_id= atoi(row[8]);

        char *tmp;
        if((tmp = strtok( row[7] , "|"))==NULL)
        {
            newdrop->alt[0]=0;
        }
        else
        {
            newdrop->alt[0]=atoi(tmp);
        }

        //We only use 7 alt drops (why [8] for alt?).
        //for(unsigned int i=1;i<8; i++)
        newdrop->alt[7]=0;
        for(unsigned int i=1;i<7; i++)
        {
            if((tmp = strtok( NULL , "|"))==NULL)
            {
                newdrop->alt[i]=0;
            }
            else
            {
                newdrop->alt[i]=atoi(tmp);
            }

        }

        //LMA: checking stupid drops...
        if(newdrop->itemtype<=0||newdrop->itemtype==13||newdrop->itemtype>14)
        {
            Log(MSG_WARNING,"Incorrect drop itemtype detected (%i) for record %i",newdrop->itemtype,record_id);
            delete newdrop;
            continue;
        }

        //checking items.
        if (newdrop->itemnum<=0||newdrop->itemnum>=nb_max_itemtype[newdrop->itemtype])
        {
            Log(MSG_WARNING,"Incorrect drop itemnum detected (%i==0 or >=%u) for record %i",newdrop->itemnum,nb_max_itemtype[newdrop->itemtype],record_id);
            delete newdrop;
            continue;
        }

        MDropList.push_back( newdrop );
    }

    DB->QFree( );
    Log( MSG_LOAD, "PYDrops Data loaded" );


    return true;
}

//LMA: Drops using the AND system.
bool CWorldServer::LoadPYDropsDataAnd( )
{
   	Log( MSG_INFO, "Loading PYDropsAnd Data " );
    MDropList.clear();
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT id,type,min_level,max_level,prob,mob,map,alt,ref,a_x,a_y,a_range FROM item_drops_and");
    if(result==NULL)
    {
        DB->QFree( );
        return false;
    }

    //Creating indexes
    int nb_max_itemtype[15];
    nb_max_itemtype[0]=0;
    nb_max_itemtype[13]=0;
    int nb_conditions=0;

    for(int j=1;j<10;j++)
    {
        nb_max_itemtype[j]=EquipList[j].max;
    }

    nb_max_itemtype[10]=UseList.max;
    nb_max_itemtype[11]=JemList.max;
    nb_max_itemtype[12]=NaturalList.max;
    nb_max_itemtype[14]=PatList.max;


    while(row = mysql_fetch_row(result))
    {
        CMDrops* newdrop = new (nothrow) CMDrops;
        assert(newdrop);
        newdrop->itemnum = atoi(row[0]);
        newdrop->itemtype = atoi(row[1]);
        newdrop->level_min = atoi(row[2]);
        newdrop->level_max = atoi(row[3]);
        newdrop->prob = atoi(row[4]);
        newdrop->mob = atoi(row[5]);
        newdrop->map = atoi(row[6]);
        newdrop->a_x=0;
        newdrop->a_y=0;
        newdrop->b_x=0;
        newdrop->b_y=0;

        nb_conditions=0;
        if(newdrop->mob>0)
        {
            nb_conditions++;
        }

        if(newdrop->level_min>0||newdrop->level_max>0)
        {
            nb_conditions++;
        }

        if(newdrop->map>0)
        {
            nb_conditions++;
        }

        //LMA: An area, only exists in a map of course.
        if(atoi(row[9])>0&&atoi(row[10])>0&&atoi(row[11])>0&&newdrop->map!=0)
        {
            nb_conditions++;
            newdrop->a_x=atoi(row[9])-atoi(row[11]);
            newdrop->a_y=atoi(row[10])+atoi(row[11]);
            newdrop->b_x=atoi(row[9])+atoi(row[11]);
            newdrop->b_y=atoi(row[10])-atoi(row[11]);
        }

        int record_id= atoi(row[8]);

        char *tmp;
        if((tmp = strtok( row[7] , "|"))==NULL)
        {
            newdrop->alt[0]=0;
        }
        else
        {
            newdrop->alt[0]=atoi(tmp);
        }

        for(unsigned int i=1;i<8; i++)
        {
            if((tmp = strtok( NULL , "|"))==NULL)
            {
                newdrop->alt[i]=0;
            }
            else
            {
                newdrop->alt[i]=atoi(tmp);
            }

        }

        //Log(MSG_INFO,"record_id %i, alt: %i, %i, %i, %i, %i, %i, %i, %i",record_id,newdrop->alt[0],newdrop->alt[1],newdrop->alt[2],newdrop->alt[3],newdrop->alt[4],newdrop->alt[5],newdrop->alt[6],newdrop->alt[7]);

        //LMA: checking stupid drops...
        if(newdrop->itemtype<=0||newdrop->itemtype==13||newdrop->itemtype>14)
        {
            Log(MSG_WARNING,"Incorrect drop itemtype detected (%i) for record %i",newdrop->itemtype,record_id);
            delete newdrop;
            continue;
        }

        //checking items.
        if (newdrop->itemnum<=0||newdrop->itemnum>=nb_max_itemtype[newdrop->itemtype])
        {
            Log(MSG_WARNING,"Incorrect drop itemnum detected (%i==0 or >=%u) for record %i",newdrop->itemnum,nb_max_itemtype[newdrop->itemtype],record_id);
            delete newdrop;
            continue;
        }

        //LMA: Saving drop.
        if(nb_conditions==1)
        {
            if(newdrop->map>0)
            {
                //special case, map drops are to be sent as multiple drop.
                DropsAnd[newdrop->map].push_back( newdrop );
            }
            else
            {
                DropsAnd[0].push_back( newdrop );
            }

        }
        else if (nb_conditions>1)
        {
            DropsAnd[newdrop->map].push_back( newdrop );
        }
        else
        {
            Log(MSG_WARNING,"No condition for Drop Record %i",record_id);
            delete newdrop;
            continue;
        }

    }

    DB->QFree( );
    Log( MSG_INFO, "PYDropsAnd Data loaded" );


    return true;
}

//hidden
//bool CWorldServer::LoadSkillBookDropsData( )
//{
//    Log( MSG_INFO, "Loading Skillbook data" );
//    MYSQL_ROW row;
//    MYSQL_RES *result = DB->QStore("SELECT id,itemtype,min,max,prob FROM list_skillbooks");
//    if(result==NULL)
//    {
//        DB->QFree( );
//        return false;
//    }
//    int c = 0;
//    while(row = mysql_fetch_row(result))
//    {
//
//        c++;
//        CMDrops* newdrop = new (nothrow) CMDrops;
//        assert(newdrop);
//        newdrop->itemnum = atoi(row[0]);
//        newdrop->itemtype = atoi(row[1]);
//        newdrop->level_min = atoi(row[2]);
//        newdrop->level_max = atoi(row[3]);
//        newdrop->prob = atoi(row[4]);
//        SkillbookList.push_back( newdrop );
//    }
//    DB->QFree( );
//    Log( MSG_INFO, "Skillbook Data loaded" );
//    return true;
//}

//PY: New method of spawning monsters using the new IFO code
bool CWorldServer::LoadMonstersFromIFO( )
{
  Log( MSG_INFO, "Monsters Spawn   (New IFO method)" );

  for (UINT i = 0; i < MapList.Map.size(); i++)
  {
    CMap *thismap = MapList.Map.at(i);
    Log( MSG_DEBUG, "Spawning monsters in map %i.",thismap->id );
    if(thismap == NULL)
    {
       Log( MSG_DEBUG, "Attempting to spawn monsters in a NULL map." );
       continue;
    }
    if(thismap->id == 0)
    {
       Log( MSG_DEBUG, "Attempting to spawn monsters in map with id = 0." );
       continue;
    }
    //Log( MSG_INFO, "Spawning monsters in map %i",thismap->id );
    for (UINT j=0;j<thismap->MonsterSpawnList.size(); j++)
    {
      //load first group of bmobs
      CSpawnArea* thisspawn = thismap->MonsterSpawnList.at(j);
      if(thisspawn == NULL)
      {
          Log( MSG_INFO, "Spawn does not exist");
          continue;
      }
      thisspawn->MobPointer = 1;    //set index pointer to first bmob
      thisspawn->MCount = 0;     //set monster count to 0. We haven't spawned any yet.
      for (UINT k=0;k<thisspawn->bcount[1];k++) //spawn the basic monsters within this bmob
      {
        fPoint position = RandInCircle( thisspawn->points, thisspawn->radius );
        thismap->AddMonster( thisspawn->bmob[1], position, 0, NULL, NULL, thisspawn->id );
        thisspawn->MCount++;
      }
    }
  }
  Log( MSG_INFO, "Monsters Spawn  finished (New IFO method)" );
  return true;
}

bool CWorldServer::LoadMonsters( )
{
	Log( MSG_LOAD, "Monsters Spawn              " );
	// Do our monster spawning
       for (UINT i = 0; i < MapList.Map.size(); i++)
       {
          CMap *thismap = MapList.Map.at(i);
          for (UINT j = 0; j < thismap->MobGroupList.size(); j++)
          {
            bool GroupFull = false;
            CMobGroup* thisgroup = thismap->MobGroupList.at(j);

            // Load some basic mobs onto map
            thisgroup->curBasic=0;
            int nb_groups=1;    //LMA: only one group at a time...

            for (UINT k = 0; k < thisgroup->basicMobs.size(); k++)
            {
              CMob* thismob = thisgroup->basicMobs.at(thisgroup->curBasic); //LMA
              thisgroup->curBasic++;

              if (thisgroup->curBasic >= thisgroup->basicMobs.size())
              {
                  thisgroup->curBasic = 0;
              }

              //LMA: Don't spawn all the mobs at once.
              thismob->real_amount=1;
              if(thismob->amount!=1)
              {
                    thismob->real_amount=RandNumber(1,thismob->amount);
                    if(thismob->real_amount>thismob->amount)
                    {
                        thismob->real_amount=thismob->amount;
                    }

              }

              //for (UINT l = 0; l < thismob->amount; l++)
              for (UINT l = 0; l < thismob->real_amount; l++)
              {
                fPoint position = RandInCircle( thisgroup->point, thisgroup->range );
                thismap->AddMonster( thismob->mobId, position, 0, thismob->mobdrop, thismob->mapdrop, thisgroup->id );


                //LMA: enough.
                if (thisgroup->active >= thisgroup->limit)
                {
                  GroupFull = true;
                  break;
                }

              }

              //LMA: One group at a time?
              if((k+1)>=nb_groups)
              {
                  GroupFull=true;
              }

              if (GroupFull)
              {
                  break;
              }

            }

          }

    }


	return true;
}

//PY: Loading upgrade chances for refining equipment
//Replacing the utter drivel written by LMA below (now deleted forever)
bool CWorldServer::LoadRefineChances ()
{
	WORD counter = 0;
	for (WORD i = 0; i < ListUpGrade.rowcount; i++)
	{
		UpGradeList[i].BindLevel	= ListUpGrade.rows[i][0];
		UpGradeList[i].BindSuccess	= ListUpGrade.rows[i][1];
		UpGradeList[i].TaliLevel	= ListUpGrade.rows[i][2];
		UpGradeList[i].TaliSuccess	= ListUpGrade.rows[i][3];
		counter++;
	}
	Log (MSG_INFO, "Refine Chance Data Loaded: %i entries.", counter);
	return true;
}

//#ifdef REFINENEW


bool CWorldServer::CleanConnectedList( )
{
    Log( MSG_LOAD, "Cleaning Connected Clients         " );
    DB->QExecute("UPDATE accounts set online=false");
    Log( MSG_LOAD, "Cleaning Connected loaded" );
    return true;
}

bool CWorldServer::LoadEquip( )
{
    //STB version
    Log( MSG_LOAD, "Equip Data - STB         " );
    for(int j=0;j<9;j++)
    {
        for(unsigned int i=0;i<STB_ITEM[j].rowcount;i++)
        {
            CEquip* newequip = new (nothrow) CEquip;
			//CItem* newequip = new (nothrow) CItem;
            if(newequip == NULL)
            {
                Log(MSG_WARNING, "\nError allocing memory: equip" );
                return false;
            }
			
            newequip->id				= i;
            newequip->equiptype			= (j + 1);
			newequip->restriction		= STB_ITEM[j].rows[i][3];
            newequip->type				= STB_ITEM[j].rows[i][4];
            newequip->price				= STB_ITEM[j].rows[i][5];
            newequip->pricerate			= STB_ITEM[j].rows[i][6];
            newequip->weight			= STB_ITEM[j].rows[i][7];
            newequip->quality			= STB_ITEM[j].rows[i][8];
            newequip->level				= STB_ITEM[j].rows[i][13];
            newequip->craft_skill		= STB_ITEM[j].rows[i][12];
            newequip->craft_level		= STB_ITEM[j].rows[i][13];
            newequip->craft_recipe		= STB_ITEM[j].rows[i][14];
            newequip->craft_difficult	= STB_ITEM[j].rows[i][15];
			newequip->durability		= STB_ITEM[j].rows[i][29];
            newequip->acc				= STB_ITEM[j].rows[i][30];		// it's Accuracy for weapons
			newequip->dodge				= STB_ITEM[j].rows[i][30];		// it's dodge for other equips	so we are loading both
            newequip->defense			= STB_ITEM[j].rows[i][31];
            newequip->magicresistence	= STB_ITEM[j].rows[i][32];
           
            if(j != BACK)
            {
                newequip->STLId			= STB_ITEM[j].rows[i][54];
                newequip->STLPrefix		= STB_ITEM[j].rows[i][53];
            }
            else
            {
                newequip->STLId			= STB_ITEM[j].rows[i][34];		//back items have a different number of columns
                newequip->STLPrefix		= 0;
            }

            if(newequip->equiptype == SHOE)
            {
                newequip->movespeed		= STB_ITEM[j].rows[i][33];
            }
            else
            {
                newequip->movespeed		= 0;
            }

            //All files from itemtype 1 to 10 are loaded here, it's a test for weapons I guess.
            if (j + 1 == WEAPON)				//applies to weapons only. needs to be +1 because J goes from 0 to 8
            {
				newequip->attackdistance	= STB_ITEM[j].rows[i][33];
				newequip->attackpower		= STB_ITEM[j].rows[i][35];
                newequip->attackspeed		= STB_ITEM[j].rows[i][36];
                newequip->magicattack		= STB_ITEM[j].rows[i][37];      //new. Staffs and wands are designated as magic weapons.
                if(newequip->magicattack > 1)newequip->magicattack = 0;		//all the blue named spears have weird numbers here. Maybe why NA had a massive damage glitch with them.
            }
			else
            {																// Should not leave these two unitialized
                newequip->attackpower		= 0;
                newequip->attackspeed		= 0;
				newequip->magicattack		= 0;					
				newequip->attackdistance	= 0;
            }

            //job1 to job3
            for(int k=0;k<3;k++)
            {
                //LMA: Weird values... Same for unions (23 and 26)
                //values from 1 to 5 mean unions!
                //0=visitor
                //others are to be checked against list_class.stb.
                //actually needed for itemtype from 1 to 9.
                newequip->occupation[k] = STB_ITEM[j].rows[i][(16+k)];
            }

            //LMA: not correct there are only 2 conditions and 2 values...
            /*
            for(int k=0;k<2;k++)
                newequip->condition1[k] = STB_ITEM[j].rows[i][(19+k)];
            for(int k=0;k<2;k++)
                newequip->condition2[k] = STB_ITEM[j].rows[i][(21+k)];
            for(int k=0;k<2;k++)
                newequip->stat1[k] = STB_ITEM[j].rows[i][(24+k)];
            for(int k=0;k<2;k++)
                newequip->stat2[k] = STB_ITEM[j].rows[i][(27+k)];
            */

            //LMA: equip conditions (job, str...)
            newequip->condition1[0]		= STB_ITEM[j].rows[i][19];
            newequip->condition1[1]		= STB_ITEM[j].rows[i][20];
            newequip->condition2[0]		= STB_ITEM[j].rows[i][21];
            newequip->condition2[1]		= STB_ITEM[j].rows[i][22];

            //Equip stat (max HP...)
            newequip->stat1[0]			= STB_ITEM[j].rows[i][24];
            newequip->stat1[1]			= STB_ITEM[j].rows[i][25];
            newequip->stat2[0]			= STB_ITEM[j].rows[i][27];
            newequip->stat2[1]			= STB_ITEM[j].rows[i][28];

            // Used for refine.
			
            newequip->refineitem		= STB_ITEM[j].rows[i][45];
            newequip->itemgrade			= STB_ITEM[j].rows[i][46];

            //LMA: used for breakid
            newequip->breakid			= 0;
            if(newequip->equiptype < 7 ||newequip->equiptype == 8 || newequip->equiptype == 9)
            {
                newequip->breakid		= STB_ITEM[j].rows[i][47];
            }

			newequip->rare_type			= STB_ITEM[j].rows[i][53];
			newequip->is_mileage = 0;
			if (newequip->rare_type = 31) { newequip->is_mileage = 1; }

            if(newequip->equiptype == 9)
            {
                newequip->BlockRate		= STB_ITEM[j].rows[i][30];
            }else{
                newequip->BlockRate		= 0;
            }
            if (newequip->STLId != 0)
            {
                newequip->IsValid = true;                               // if the item has an STL id then we can count it as a valid item
            }
            else {
                newequip->IsValid = false;
            }
            newequip->idx = i;                                          // quick lookup for the position in the vector
            //LMA: raretype not handled !!
            //newequip->raretype = STB_ITEM[j].rows[i][50];
            EquipList[newequip->equiptype].itemList.push_back( newequip );
            //EquipList[newequip->equiptype].Index[newequip->id] = newequip; // Index to read more quickly the data
        }
    }

    Log( MSG_LOAD, "Equip Data loaded" );


    return true;
}

bool CWorldServer::LoadJemItem( )
{
    Log( MSG_LOAD, "Jem Data - STB        " );
    for(unsigned int i=0;i<STB_ITEM[10].rowcount;i++)
    {
        CJemData* thisjem = new (nothrow) CJemData;
        if(thisjem == NULL)
        {
            Log(MSG_WARNING, "\nError allocing memory: jemitem" );
            continue;
        }
        thisjem->id					= i;
        thisjem->restriction		= STB_ITEM[10].rows[i][3];
		thisjem->type				= STB_ITEM[10].rows[i][4];
        thisjem->price				= STB_ITEM[10].rows[i][5];
        thisjem->pricerate			= STB_ITEM[10].rows[i][6];
        thisjem->weight				= STB_ITEM[10].rows[i][7];
        thisjem->quality			= STB_ITEM[10].rows[i][8];
        thisjem->craft_skill		= STB_ITEM[10].rows[i][12];
        thisjem->craft_level		= STB_ITEM[10].rows[i][13];
        thisjem->craft_recipe		= STB_ITEM[10].rows[i][14];
        thisjem->craft_difficult	= STB_ITEM[10].rows[i][15];
        thisjem->stat1[0]			= STB_ITEM[10].rows[i][16];
        thisjem->stat1[1]			= STB_ITEM[10].rows[i][17];
        thisjem->stat1[2]			= STB_ITEM[10].rows[i][25];
        thisjem->stat2[0]			= STB_ITEM[10].rows[i][18];
        thisjem->stat2[1]			= STB_ITEM[10].rows[i][19];
        thisjem->stat2[2]			= STB_ITEM[10].rows[i][26];

        //LMA: STL
        thisjem->STLId				= STB_ITEM[10].rows[i][48];

        JemList.Index.push_back( thisjem );
        //JemList.Index[thisjem->id]	= thisjem;

        //LMA: testing maps :) Don't need to test keys since it's ++
        //JemList.DataMap[i]=thisjem;
    }
    Log( MSG_LOAD, "Jem Data loaded" );
    return true;
}

bool CWorldServer::LoadNaturalItem( )
{
    Log( MSG_LOAD, "Natural Data - STB        " );
    for(unsigned int i=0;i<STB_ITEM[11].rowcount;i++)
    {
        CNaturalData* thisnatural = new (nothrow) CNaturalData;
        if(thisnatural == NULL)
        {
            Log(MSG_WARNING, "\nError allocing memory: natural" );
            return false;
        }
        thisnatural->id					= i;
		thisnatural->restriction		= STB_ITEM[11].rows[i][3];
        thisnatural->type				= STB_ITEM[11].rows[i][4];
        thisnatural->price				= STB_ITEM[11].rows[i][5];
        thisnatural->pricerate			= STB_ITEM[11].rows[i][6];
        thisnatural->weight				= STB_ITEM[11].rows[i][7];
        thisnatural->quality			= STB_ITEM[11].rows[i][8];
		thisnatural->craft_skill		= STB_ITEM[11].rows[i][12];
		thisnatural->craft_level		= STB_ITEM[11].rows[i][13];
		thisnatural->craft_recipe		= STB_ITEM[11].rows[i][14];
		thisnatural->craft_difficult	= STB_ITEM[11].rows[i][15];
        thisnatural->pricevalue			= STB_ITEM[11].rows[i][15];
		thisnatural->necessary_item_type = STB_ITEM[11].rows[i][18];
		thisnatural->projectile_type	= STB_ITEM[11].rows[i][17];
		thisnatural->refine_bonus		= STB_ITEM[11].rows[i][18];
        //LMA: STL:
        thisnatural->STLId				= STB_ITEM[11].rows[i][20];

        //NaturalList.Data.push_back( thisnatural );
        NaturalList.Index[thisnatural->id] = thisnatural;
    }

    Log( MSG_LOAD, "Natural Data loaded" );
    return true;
}

bool CWorldServer::LoadPatItem( )
{
    Log( MSG_LOAD, "PAT Data - STB         " );
    for(unsigned int i=0;i<STB_ITEM[13].rowcount;i++)
    {
        CPatData* newpat = new (nothrow) CPatData;
        if(newpat == NULL)
        {
            Log(MSG_WARNING, "Error allocing memory: pat\n" );
            return false;
        }
        newpat->id				= i;
		newpat->restriction		= STB_ITEM[13].rows[i][3];
        newpat->type			= STB_ITEM[13].rows[i][4];
        newpat->price			= STB_ITEM[13].rows[i][5];
        newpat->pricerate		= STB_ITEM[13].rows[i][6];
        newpat->weight			= STB_ITEM[13].rows[i][7];
        newpat->quality			= STB_ITEM[13].rows[i][8];
        newpat->craft_skill		= STB_ITEM[13].rows[i][12];
        newpat->craft_level		= STB_ITEM[13].rows[i][13];
        newpat->craft_recipe	= STB_ITEM[13].rows[i][14];
        newpat->craft_difficult	= STB_ITEM[13].rows[i][15];
        newpat->parttype		= STB_ITEM[13].rows[i][16];
        newpat->partversion		= STB_ITEM[13].rows[i][17];
        //newpat->level = STB_ITEM[13].rows[i][22];             // extra field added rev 70
        //newpat->condition[1] = STB_ITEM[13].rows[i][24];      // extra field added rev 70
        //newpat->modifier[1] = STB_ITEM[13].rows[i][25];       // extra field added rev 70
        //newpat->condition[2] = STB_ITEM[13].rows[i][27];      // extra field added rev 70
        //newpat->modifier[2] = STB_ITEM[13].rows[i][28];       // extra field added rev 70
        newpat->modifier		= STB_ITEM[13].rows[i][25];       // extra field added rev 70

        //LMA: Getting all stats...
        newpat->options[0]		= STB_ITEM[13].rows[i][24];
        newpat->val_options[0]	= STB_ITEM[13].rows[i][25];
        newpat->options[1]		= STB_ITEM[13].rows[i][27];
        newpat->val_options[1]	= STB_ITEM[13].rows[i][28];

        newpat->maxfuel			= STB_ITEM[13].rows[i][31];
        newpat->fuelcons		= STB_ITEM[13].rows[i][32];
        newpat->speed			= STB_ITEM[13].rows[i][33];
        newpat->attackdistance	= STB_ITEM[13].rows[i][35];
        newpat->attackpower		= STB_ITEM[13].rows[i][36];
        newpat->attackspeed		= STB_ITEM[13].rows[i][37];
        newpat->gauge			= STB_ITEM[13].rows[i][67];
        //PatList.Data.push_back( newpat );
        newpat->STLId			= STB_ITEM[13].rows[i][75];
        PatList.Index[newpat->id] = newpat;
		// update itemlist
		
		
    }
    Log( MSG_LOAD, "PAT Data loaded" );
    return true;
}

bool CWorldServer::LoadProductItem( )
{
    Log( MSG_LOAD, "Product Data - STB        " );
    for(unsigned int i=0;i<STB_PRODUCT.rowcount;i++)
    {
        CProductData* newproduct = new (nothrow) CProductData;
		CCraftRecipe* newRecipe = new (nothrow) CCraftRecipe;
		
        if(newproduct == NULL)
        {
            Log(MSG_WARNING, "Error allocing memory: product\n" );
            return false;
        }
		// PY New way using new recipe system
		newRecipe->NumberOfIngredients		= 0;						// will be updated later
		newRecipe->Id						= i;
		newRecipe->Family[0]				= STB_PRODUCT.rows[i][1];	// Not really used in evo but still in the STB
		newRecipe->ingredients[0].ItemID	= STB_PRODUCT.rows[i][2];
		newRecipe->ingredients[0].Count	= STB_PRODUCT.rows[i][3];
		if(newRecipe->ingredients[0].PopulateFromItemID ())
			newRecipe->NumberOfIngredients ++;							// if we have a valid entry then increment the ingredients count
		newRecipe->Family[1]				= STB_PRODUCT.rows[i][4];	// Not really used in evo but still in the STB
		newRecipe->ingredients[1].ItemID	= STB_PRODUCT.rows[i][5];
		newRecipe->ingredients[1].Count	= STB_PRODUCT.rows[i][6];
		if(newRecipe->ingredients[1].PopulateFromItemID ())
			newRecipe->NumberOfIngredients++;							// if we have a valid entry then increment the ingredients count
		newRecipe->Family[2]				= STB_PRODUCT.rows[i][7];	// Not really used in evo but still in the STB
		newRecipe->ingredients[2].ItemID	= STB_PRODUCT.rows[i][8];
		newRecipe->ingredients[2].Count	= STB_PRODUCT.rows[i][9];
		if(newRecipe->ingredients[2].PopulateFromItemID ())
			newRecipe->NumberOfIngredients++;							// if we have a valid entry then increment the ingredients count
		newRecipe->Family[3]				= STB_PRODUCT.rows[i][10];	// Not really used in evo but still in the STB
		newRecipe->ingredients[3].ItemID	= STB_PRODUCT.rows[i][11];
		newRecipe->ingredients[3].Count	= STB_PRODUCT.rows[i][12];
		if(newRecipe->ingredients[3].PopulateFromItemID ())
			newRecipe->NumberOfIngredients++;							//if we have a valid entry then increment the ingredients count
		

		// old way. left for compatibility until the entire system is upgraded
        newproduct->id = i;
        newproduct->item_0_family=STB_PRODUCT.rows[i][1];
        newproduct->item[0]=STB_PRODUCT.rows[i][2];
        newproduct->amount[0]=STB_PRODUCT.rows[i][3];
        newproduct->item_1_family=STB_PRODUCT.rows[i][4];
        newproduct->item[1]=STB_PRODUCT.rows[i][5];
        newproduct->amount[1]=STB_PRODUCT.rows[i][6];
        newproduct->item_2_family=STB_PRODUCT.rows[i][7];
        newproduct->item[2]=STB_PRODUCT.rows[i][8];
        newproduct->amount[2]=STB_PRODUCT.rows[i][9];
        newproduct->item_3_family=STB_PRODUCT.rows[i][10];
        newproduct->item[3]=STB_PRODUCT.rows[i][11];
        newproduct->amount[3]=STB_PRODUCT.rows[i][12];
        //ProductList.Data.push_back( newproduct );
        ProductList.Index[newproduct->id] = newproduct;
		RecipeList.push_back( newRecipe );

    }
    Log( MSG_LOAD, "Product Data loaded" );
    return true;
}

bool CWorldServer::LoadSellData( )
{
    Log( MSG_LOAD, "Sell Data - STB        " );
    for(unsigned int i=0;i<STB_SELL.rowcount;i++)
    {

        CCSellData* newsell = new (nothrow) CCSellData;
        if(newsell==NULL)
        {
            Log(MSG_WARNING, "\nError Allocing memory: sell" );
            return false;
        }
        newsell->id = i;

        for(unsigned int j=2;j<STB_SELL.fieldcount;j++)
        {
            if (j>=50)
            {
                Log(MSG_WARNING,"Loading SELL DATA, Too much Items for %i! %i>48",i,STB_SELL.fieldcount);
                break;
            }

            newsell->item[j-2] = STB_SELL.rows[i][j];
        }
        //SellList.Data.push_back( newsell );
        SellList.Index[newsell->id] = newsell;
    }
    Log( MSG_LOAD, "Sell Data loaded" );
    return true;
}

bool CWorldServer::LoadConsItem( )
{
    Log( MSG_LOAD, "Consumable Data - STB   " );
    for(unsigned int i=0;i<STB_ITEM[9].rowcount;i++)
    {
        CUseData* newuse = new (nothrow) CUseData;
        if(newuse == NULL)
        {
            Log(MSG_WARNING, "\nError allocing memory: use" );
            return false;
        }

        newuse->id				= i;
        newuse->restriction		= STB_ITEM[9].rows[i][3];
        newuse->type			= STB_ITEM[9].rows[i][4];
        newuse->price			= STB_ITEM[9].rows[i][5];
        newuse->pricerate		= STB_ITEM[9].rows[i][6];
        newuse->weight			= STB_ITEM[9].rows[i][7];
        newuse->quality			= STB_ITEM[9].rows[i][8];
        newuse->craft_skill		= STB_ITEM[9].rows[i][12];
        newuse->craft_level		= STB_ITEM[9].rows[i][13];
        newuse->craft_recipe	= STB_ITEM[9].rows[i][14];
        newuse->craft_difficult = STB_ITEM[9].rows[i][15];
        newuse->pricevalue		= STB_ITEM[9].rows[i][16];
        newuse->required_Stat	= STB_ITEM[9].rows[i][17];
        newuse->required_Amount = STB_ITEM[9].rows[i][18];
        newuse->affected_Stat	= STB_ITEM[9].rows[i][19];
        newuse->affected_Amount	= STB_ITEM[9].rows[i][20];
		newuse->skill_number	= STB_ITEM[9].rows[i][20];

        //LMA: adding cooldown.
        newuse->cooldown_type	= STB_ITEM[9].rows[i][25];
        newuse->cooldown		= STB_ITEM[9].rows[i][25];
        newuse->is_mileage		= 0;

        //STL
        newuse->STLId			= STB_ITEM[9].rows[i][29];

        //LMA: mileage box?
        if(newuse->restriction == 6 && newuse->quality == 100)
            newuse->is_mileage = 1;

        //UseList.Data.push_back( newuse );
        UseList.Index[newuse->id] = newuse;
    }
    Log( MSG_LOAD, "Consumable Data Loaded" );
    return true;
}


//LMA: Loading Zone data (no CF anymore).
bool CWorldServer::LoadZoneData( )
{
    UINT nb_active=0;

    Log( MSG_LOAD, "Zone Data - STB   " );
    for(unsigned int i=0;i<ZoneData.rowcount;i++)
    {
        CMap* newzone = new (nothrow) CMap( );
        if(newzone==NULL)
        {
            Log(MSG_WARNING, "\nError allocing memory: use" );
            return false;
        }


        //LMA: resetting to good values.
        newzone->id =0;
        newzone->dayperiod = 1;
        newzone->morningtime = 0;
        newzone->daytime = 0;
        newzone->eveningtime = 0;
        newzone->nighttime = 0;
        newzone->allowpvp = 0;
        newzone->pvp_mode = PVP_MODE_OFF;
        newzone->allowpat = 0;
        newzone->MapTime = 0;
        newzone->LastUpdate = 0;
        newzone->CurrentTime = 0;
        newzone->nb_summons=0;

        newzone->id = i;

        //We load only the interesting maps.
        if(ZoneData.rows[i][13]==0)
        {
            newzone->dayperiod = 0;
            newzone->morningtime = 0;
            newzone->daytime = 0;
            newzone->eveningtime = 0;
            newzone->nighttime = 0;
            newzone->allowpvp = 0;
            newzone->pvp_mode = PVP_MODE_OFF;
            newzone->allowpat = false;
            newzone->STLID=0;

            //QSD triggers.
            newzone->QSDzone=0;
            newzone->QSDkilling=0;
            newzone->QSDDeath=0;

            newzone->MapTime = 0;
            newzone->LastUpdate = clock( );
            newzone->CurrentTime = 0;
            newzone->is_cf=0;
            newzone->MonsterSpawnList.clear();
            newzone->MobGroupList.clear();

            MapList.Index[newzone->id] = newzone;
            continue;
        }

        //LMA: check if out of memory (shouldn't happen).
        if (newzone->id>=MapList.max)
        {
           Log(MSG_WARNING,"list Zone, index overflow trapped %i>%i (should not happen)",newzone->id,MapList.max);
           delete newzone;
           continue;
        }

        newzone->dayperiod = ZoneData.rows[i][13];
        newzone->morningtime = ZoneData.rows[i][14];
        newzone->daytime = ZoneData.rows[i][15];
        newzone->eveningtime = ZoneData.rows[i][16];
        newzone->nighttime = ZoneData.rows[i][17];
        newzone->allowpvp = ZoneData.rows[i][18];
        if(ZoneData.rows[i][30]==1)
        {
            newzone->allowpat = false;
        }else
        {
            newzone->allowpat = true;
        }
        //newzone->ghost = GetUIntValue(",");   //never used.
        newzone->STLID= ZoneData.rows[i][26];

        //QSD triggers.
        newzone->QSDzone=ZoneData.rows[i][22];
        newzone->QSDkilling=ZoneData.rows[i][23];
        newzone->QSDDeath=ZoneData.rows[i][24];
        newzone->is_cf=0;

        newzone->MapTime = 0;
        newzone->LastUpdate = clock( );
        newzone->CurrentTime = 0;
        newzone->MonsterSpawnList.clear();
        newzone->MobGroupList.clear();

        // PvP Zone Type Classification
        UINT raw_pvp = ZoneData.rows[i][18];
        UINT raw_zt = ZoneData.rows[i][20];

        if (raw_pvp == 0 || raw_pvp == 12)
        {
            // Mode 0: Safe PvE zone / Agits
            newzone->allowpvp = 0;
            newzone->pvp_mode = PVP_MODE_OFF;
        }
        else if (raw_pvp == 2 || newzone->id == 5)
        {
            // Mode 4: Union War / Cartel (Junon Cartel, Union battles)
            newzone->allowpvp = 1;
            newzone->pvp_mode = PVP_MODE_UNION;
        }
        else if (newzone->id == 9)
        {
            // Mode 2: Team / Arena PvP (Akram Arena)
            newzone->allowpvp = 1;
            newzone->pvp_mode = PVP_MODE_TEAM;
        }
        else if (newzone->id == 8 || (newzone->id >= 11 && newzone->id <= 13) || newzone->id == 59 || (newzone->id >= 101 && newzone->id <= 120))
        {
            // Mode 3: Clan Fields (Lion's Plains, Junon Clan Fields, Luna Clan Field, Pedion)
            newzone->allowpvp = 1;
            newzone->pvp_mode = PVP_MODE_CLAN;
        }
        else
        {
            // Mode 1: Free-For-All (Training Grounds, Desert of Dead, Sikuku Ruins, etc.)
            newzone->allowpvp = 1;
            newzone->pvp_mode = PVP_MODE_FFA;
        }

        nb_active++;

        MapList.Map.push_back(newzone);
        MapList.Index[newzone->id] = newzone;
    }

    Log( MSG_LOAD, "Zone Data STB Loaded" );

    //LMA: Some checks :)
    if(maxZone>NB_MAPS)
    {
        Log(MSG_WARNING,"The number of maps in list_zone.STB (%u) is not the same as NB_MAPS (%u), change NB_MAPS in code if you have more warning messages.",maxZone,NB_MAPS);
    }

    if(nb_active>=NB_GRIDS)
    {
        Log(MSG_WARNING,"It seems you have more active maps than you can handle later, %u>=%u, change NB_GRIDS to %u in code if you have more warning messages.",nb_active,NB_GRIDS,(nb_active+1));
    }


    return true;
}

//LMA: Grid Maps. Dumb ass rubbish just crashes everything
/*
bool CWorldServer::LoadGrids( )
{
	return false;
	Log( MSG_LOAD, "Grid Data                   " );
    FILE* fh = fopen( "data/map_grid.csv", "r" );
    if(fh==NULL)
    {
        Log(MSG_ERROR, "\nError loading file data/map_grid.csv" );
        return false;
    }

    //LMA maps
    int k=0;
    int j=0;
    for(k=0;k<NB_MAPS;k++)
    {
        allmaps[k].grid_id=-1;
        allmaps[k].always_on=false;
    }
    k=0;
    //LMA END
    int lx=0;
    int ly=0;
    int rx=0;
    int ry=0;

    char line[500];
    fgets( line, 500, fh );// this is the column name
    while(!feof(fh))
    {
        memset( &line, '\0', 500 );
        fgets( line, 500, fh );

        j = GetUIntValue(",", &line);
        if (j==0)
           continue;

        if(j>=NB_MAPS)
        {
            Log(MSG_WARNING,"Incorrect map when loading grids (%i>=%u), change NB_MAPS?",j,NB_MAPS);
            continue;
        }

        if(k>=NB_GRIDS)
        {
            Log(MSG_WARNING,"We try to load more active maps that we can handle (%i>=%u), change NB_GRIDS",k,NB_GRIDS);
            continue;
        }

        lx = GetUIntValue(",");
        ly = GetUIntValue(",");
        rx = GetUIntValue(",");
        ry = GetUIntValue(",");

        //Map info
        allmaps[j].grid_id=k;         //Id of map in gridmaps
        allmaps[j].always_on=GetUIntValue(",")==0?false:true;

        //Grid now
        gridmaps[k].always_on=allmaps[j].always_on;
        gridmaps[k].length=rx-lx;
        gridmaps[k].width=ly-ry;
        gridmaps[k].org_x=lx;
        gridmaps[k].org_y=ry;

        //getting nb of columns...
        allmaps[j].nb_col=(int) ceil(gridmaps[k].length/MINVISUALRANGE);
        allmaps[j].nb_row=(int) ceil(gridmaps[k].width/MINVISUALRANGE);

        //Log(MSG_INFO,"map %i, row=%i, col=%i",j,allmaps[j].nb_row,allmaps[j].nb_col);

        if (gridmaps[k].width==0)
           gridmaps[k].width=1;

        if (gridmaps[k].length==0)
           gridmaps[k].length=1;

         //resetting values (old way).
         

        //LMA: making the right grid
        gridmaps[k].nb_cells=(allmaps[j].nb_col+2)*(allmaps[j].nb_row+2);
        gridmaps[k].coords = new int[gridmaps[k].nb_cells];
        for (j=0;j<gridmaps[k].nb_cells;j++)
        {
            gridmaps[k].coords[j]=0;
        }

       k++;
    }

    fclose(fh);


    Log( MSG_LOAD, "Grid reseted.");
    return true;
}
*/

bool CWorldServer::LoadItemStats( )
{
    Log( MSG_LOAD, "Item Stats - STB   " );
    for(unsigned int i=0;i<STB_ITEM[10].rowcount;i++)
    {
        CItemStas* tempstats=new (nothrow) CItemStas;
        StatsList[i]=tempstats;
        StatsList[i]->stat[0] = STB_ITEM[10].rows[i][16];
        StatsList[i]->value[0] = STB_ITEM[10].rows[i][17];
        StatsList[i]->stat[1] = STB_ITEM[10].rows[i][18];
        StatsList[i]->value[1] = STB_ITEM[10].rows[i][19];
    }

    Log( MSG_LOAD, "Item Stats Loaded" );
    return true;
}

//LMA: Loading grades bonuses (% and +)
bool CWorldServer::LoadGrades( )
{
    Log( MSG_LOAD, "Grades - STB   " );
    for(unsigned int i=0;i<ListGrade.rowcount;i++)
    {
        CGrade* tempGrade=new (nothrow) CGrade;
        GradeList[i]=tempGrade;
        GradeList[i]->atk_addbonus=ListGrade.rows[i][0];
        GradeList[i]->atk_percent=ListGrade.rows[i][1];
        GradeList[i]->acc_addbonus=ListGrade.rows[i][2];
        GradeList[i]->acc_percent=ListGrade.rows[i][3];
        GradeList[i]->def_addbonus=ListGrade.rows[i][4];
        GradeList[i]->def_percent=ListGrade.rows[i][5];
        GradeList[i]->mdef_addbonus=ListGrade.rows[i][6];
        GradeList[i]->mdef_percent=ListGrade.rows[i][7];
        GradeList[i]->dodge_addbonus=ListGrade.rows[i][8];
        GradeList[i]->dodge_percent=ListGrade.rows[i][9];
    }

    Log( MSG_LOAD, "Grades Loaded - STB" );


    return true;
}

//LMA: Job requirements.
bool CWorldServer::LoadJobReq( )
{
    Log( MSG_LOAD, "JobReq - STB   " );
    for(unsigned int i=0;i<ListClass.rowcount;i++)
    {
        vector<UINT>* myvector=new vector<UINT>;
        myvector->clear();
        ClassList[i]=myvector;

        if(ListClass.rows[i][1]==0)
        {
            continue;
        }

        for(int k=1;k<11;k++)
        {
            if(ListClass.rows[i][k]==0)
            {
                break;
            }

            ClassList[i]->push_back(ListClass.rows[i][k]);
        }

    }

    Log( MSG_LOAD, "JobReq Loaded" );
    return true;
}


//PY stat lookup table
bool CWorldServer::LoadStatLookup( )
{
    Log( MSG_INFO, "Loading Stats Lookup Table" );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT id, stat_number FROM list_extra_stats");
    if(result==NULL)
    {
        DB->QFree( );
        return false;
    }
    UINT counter = 1;

    while( row = mysql_fetch_row(result) )
    {
        if(counter>=MAX_EXTRA_STATS)
        {
            Log(MSG_WARNING,"Too many stats, change MAX_EXTRA_STATS value (%u)",MAX_EXTRA_STATS);
            break;
        }

        StatLookup[counter].id = atoi(row[0]);
        StatLookup[counter].statnumber = atoi(row[1]);
        counter++;
    }

    DB->QFree( );
    Log( MSG_INFO, "Stats Lookup Data Loaded" );
    return true;
}
//PY end

#ifdef PYCUSTOM
bool CWorldServer::LoadCustomTeleGate()
{
    Log( MSG_LOAD, "Loading Custom Telegate data" );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT id,sourcex,sourcey,sourcemap,destx,desty,destmap,radius,active FROM list_customgates");
    CustomGateList.clear();
    if(result==NULL)
    {
        DB->QFree( );
        return false;
    }
    while( row = mysql_fetch_row(result) )
    {
        CCustomGate* thisgate = new (nothrow) CCustomGate;
        if(thisgate == NULL)
        {
            Log(MSG_ERROR, "Error allocing memory       " );
            DB->QFree( );
            return false;
        }
        thisgate->id = atoi(row[0]);
        thisgate->source.x = (float)atof(row[1]);
        thisgate->source.y = (float)atof(row[2]);
        thisgate->sourcemap = atoi(row[3]);
        thisgate->dest.x = (float)atof(row[4]);
        thisgate->dest.y = (float)atof(row[5]);
        thisgate->destmap = atoi(row[6]);
        thisgate->radius = atoi(row[7]);
        thisgate->active = atoi(row[8]);
        CustomGateList.push_back(thisgate);
    }
    DB->QFree( );
    Log( MSG_LOAD, "Custom Telegate Data Loaded" );
    return true;
}

bool CWorldServer::LoadCustomEvents( )
{
    Log( MSG_LOAD, "Custom Events data          " );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT id,eventtype,npcname,x,y,map,radius,active,pc1,pc2,pc3,pc4,pc5,pc6,pc7,pc8,pc9,pc10,pt1,pt2,pt3,pt4,pt5,pt6,pt7,pt8,pt9,pt10,pi1,pi2,pi3,pi4,pi5,pi6,pi7,pi8,pi9,pi10,pn1,pn2,pn3,pn4,pn5,pn6,pn7,pn8,pn9,pn10,script1,script2,script3,script4,itemname,collecttype,collectnum,level FROM list_customevents");
    CustomEventList.clear();
    if(result==NULL)
    {
        DB->QFree( );
        return false;
    }
    int i;
    while( row = mysql_fetch_row(result) )
    {
        CCustomEvent* thisevent = new (nothrow) CCustomEvent;
        if(thisevent == NULL)
        {
            Log(MSG_ERROR, "Error allocing memory" );
            DB->QFree( );
            return false;
        }
        thisevent->id = atoi(row[0]);
        thisevent->eventtype = atoi(row[1]);
        strcpy(thisevent->npcname,row[2]);
        thisevent->location.x = (float)atof(row[3]);
        thisevent->location.y = (float)atof(row[4]);
        thisevent->map = atoi(row[5]);
        thisevent->radius = atoi(row[6]);
        thisevent->active = atoi(row[7]);
        for(i=1;i<11;i++)
        {
            thisevent->prizecost[i] = atoi(row[7+i]);
        }
        for(i=1;i<11;i++)
        {
            thisevent->prizetype[i] = atoi(row[17+i]);
        }
        for(i=1;i<11;i++)
        {
            thisevent->prizeid[i] = atoi(row[27+i]);
        }
        for(i=1;i<11;i++)
        {
            strcpy(thisevent->prizename[i].prizename ,row[37+i]);
        }
        strcpy(thisevent->script1,row[48]);
        strcpy(thisevent->script2,row[49]);
        strcpy(thisevent->script3,row[50]);
        strcpy(thisevent->script4,row[51]);
        strcpy(thisevent->itemname,row[52]);
        thisevent->collecttype = atoi(row[53]);
        thisevent->collectid = atoi(row[54]);
        thisevent->level = atoi(row[55]);
        CustomEventList.push_back(thisevent);
    }
    DB->QFree( );
    Log( MSG_LOAD, "Custom Events data loaded" );
    return true;
}
#endif

bool CWorldServer::LoadWayPoints()
{
    Log( MSG_INFO, "Loading Waypoints" );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT mapid,wpnum,mapx,mapy,wptype FROM list_waypoints");
    if(result==NULL)
    {
        DB->QFree( );
        return false;
    }
    if (mysql_num_rows(result) == 0)
    {
        DB->QFree( );
        Log( MSG_INFO, "list_waypoints currently contains no data" );
        return true;
    }
    // first clear the waypoint list
    for(int i=0;i<100;i++)
    {
        for(int j=0;j<100;j++)
        {
            GServer->WPList[i][j].pos.x = 0;
            GServer->WPList[i][j].pos.y = 0;
            GServer->WPList[i][j].WPType = 0;
        }
    }
    while( row = mysql_fetch_row(result) )
    {
        int thismap = atoi(row[0]);
        int thisindex = atoi(row[1]);
        GServer->WPList[thismap][thisindex].pos.x = atof(row[2]);
        GServer->WPList[thismap][thisindex].pos.y = atof(row[3]);
        GServer->WPList[thismap][thisindex].WPType = atoi(row[4]);
    }
    DB->QFree( );
    GServer->TDSpawnRate = 1000;
    GServer->TDSpawnTimer = clock();
    GServer->TDStance = 0; //default walk speed
    GServer->TDLives = 100; //default lives for TD event
    return true;
}

//LMA: Loading breaks, chests and blue crafts from list_break.stb
bool CWorldServer::LoadBreakChestBlueList()
{
    Log( MSG_LOAD, "Break / Chest / Blue Craft - STB" );
    int nb_craft=0;
    int nb_chest=0;
    int nb_break=0;
    int choice=0;
    UINT itemtype=0;
    UINT itemnum=0;

    //New naRose break code.
    int old_break=3;


    if(BreakData.fieldcount>70)
    {
       Log(MSG_INFO,"Loading new naRose Break file");
       old_break=4;
    }
    else
    {
       Log(MSG_INFO,"Loading old naRose Break file");
    }

    for(unsigned int i=0;i<BreakData.rowcount;i++)
    {
        //let's check if we have a break, a chest or a blue craft stuff...
        if(BreakData.rows[i][1]==0)
        {
            continue;
        }

        choice=0;
        itemtype= int(BreakData.rows[i][1]/1000);
        if (itemtype>0&&itemtype<10)
        {
            //break
            choice=1;
            itemnum=BreakData.rows[i][1] % 1000;
        }

        if(choice==0&&itemtype==10)
        {
            //chest.
            choice=2;
            itemnum=BreakData.rows[i][1] % 1000;
        }

        if (choice==0)
        {
            itemtype= int(BreakData.rows[i][1]/100000);
            if (itemtype>1&&itemtype<10)
            {
                //blue craft
                //In fact they are breaks?
                //choice=3;
                choice=1;
                itemnum=BreakData.rows[i][1] % 100000;
            }

        }

        if (choice==0)
        {
            //LMA: who wants to be a milionnaire? ^_^
            itemtype= int(BreakData.rows[i][1]/1000000);
             if (itemtype==10)
            {
                //chest or dispensers
                choice=2;
                itemnum=BreakData.rows[i][1] % 1000000;
            }

            if (itemtype>1&&itemtype<10)
            {
                //blue craft
                //In fact they are breaks?
                //choice=3;
                choice=1;
                itemnum=BreakData.rows[i][1] % 1000000;
            }

        }

        //LMA: for now a chest is also a break.
        CBreakList* newbreak = new (nothrow) CBreakList;
        if(newbreak==NULL)
        {
            Log(MSG_WARNING, "Error allocing memory: break list\n" );
            return false;
        }

        newbreak->itemnum=itemnum;
        newbreak->itemtype=itemtype;
        //We read the 20 items.
        for(int j=0;j<20;j++)
        {
            newbreak->product[j]=BreakData.rows[i][2+j*old_break];
            newbreak->amount_min[j]=0;

            if(old_break!=3)
            {
                newbreak->amount_min[j]=BreakData.rows[i][3+j*old_break];
                newbreak->amount_max[j]=BreakData.rows[i][4+j*old_break];
                newbreak->prob[j]=BreakData.rows[i][5+j*old_break];
            }
            else
            {
                newbreak->amount_max[j]=BreakData.rows[i][3+j*old_break];
                newbreak->prob[j]=BreakData.rows[i][4+j*old_break];
            }

            if(newbreak->amount_min[j]==0)
            {
                newbreak->amount_min[j]=1;
            }

        }

        if(old_break!=3)
        {
            //newbreak->numToGive = BreakData.rows[i][83];
            //newbreak->total = BreakData.rows[i][84];
            newbreak->reward_min = BreakData.rows[i][82];
            newbreak->reward_max = BreakData.rows[i][83];
            newbreak->nb_reward = BreakData.rows[i][84];
        }
        else
        {
            //newbreak->numToGive = BreakData.rows[i][62];
            //newbreak->total = BreakData.rows[i][63];
            newbreak->reward_min = 1;
            newbreak->reward_max = BreakData.rows[i][62];
            newbreak->nb_reward = BreakData.rows[i][63];
        }

        nb_break++;
        //BreakList.push_back( newbreak );
        BreakList[i]=newbreak;
        //Log(MSG_INFO,"Break added: (%i:%i), numtogive %i, total %i",newbreak->itemtype,newbreak->itemnum,newbreak->numToGive,newbreak->total);

        //time to make the right choice :)
        switch (choice)
        {
                case 1:
                {
                    //LMA: Commented because we add all the chests as regular breaks now, so the code below is done beforehand.
                    /*//Break, should be easy.
                    CBreakList* newbreak = new (nothrow) CBreakList;
                    if(newbreak==NULL)
                    {
                        Log(MSG_WARNING, "Error allocing memory: break list\n" );
                        return false;
                    }

                    newbreak->itemnum=itemnum;
                    newbreak->itemtype=itemtype;
                    //We read the 20 items.
                    for(int j=0;j<20;j++)
                    {
                        newbreak->product[j]=BreakData.rows[i][2+j*old_break];
                        newbreak->amount_min[j]=0;

                        if(old_break!=3)
                        {
                            newbreak->amount_min[j]=BreakData.rows[i][3+j*old_break];
                            newbreak->amount_max[j]=BreakData.rows[i][4+j*old_break];
                            newbreak->prob[j]=BreakData.rows[i][5+j*old_break];
                        }
                        else
                        {
                            newbreak->amount_max[j]=BreakData.rows[i][3+j*old_break];
                            newbreak->prob[j]=BreakData.rows[i][4+j*old_break];
                        }

                        if(newbreak->amount_min[j]==0)
                        {
                            newbreak->amount_min[j]=1;
                        }

                    }

                    if(old_break!=3)
                    {
                        newbreak->numToGive = BreakData.rows[i][83];
                        newbreak->total = BreakData.rows[i][84];
                    }
                    else
                    {
                        newbreak->numToGive = BreakData.rows[i][62];
                        newbreak->total = BreakData.rows[i][63];
                    }

                    nb_break++;
                    //BreakList.push_back( newbreak );
                    BreakList[i]=newbreak;
                    //Log(MSG_INFO,"Break added: (%i:%i), numtogive %i, total %i",newbreak->itemtype,newbreak->itemnum,newbreak->numToGive,newbreak->total);
                    */
                    break;
                }
                case 2:
                {
                    //chest... Well let's adapt Drakia's code.
                    CChest* newchest = new (nothrow) CChest;
                    if(newchest==NULL)
                    {
                        Log( MSG_WARNING, "Error allocing memory for chest\n" );
                        return false;
                    }

                    newchest->breakid=i;

                    //Patch for Xmas gift boxes...
                    newchest->chestid = itemnum;
                    /*if (itemnum>=245&&itemnum<=247)
                    {
                        newchest->chestid+=2;
                    }*/

                   //We read the 20 items.
                   int nb_rewards=0;
                   UINT rewtype=0;
                   UINT rewnum=0;
                   bool isok=true;
                   newchest->probmax = 0;

                    for(int j=0;j<20;j++)
                    {
                        if (BreakData.rows[i][2+j*old_break]==0)
                        {
                            //No more rewards to be read.
                            break;
                        }

                        isok=true;
                        rewtype=int(BreakData.rows[i][2+j*old_break]/1000);
                        rewnum=BreakData.rows[i][2+j*old_break]%1000;

                        if(rewnum==0||rewtype>14||rewtype==0)
                        {
                            //Log(MSG_WARNING,"Problem getting item from Break STB (line %i): %i error %i::%i",i,BreakData.rows[i][2+j*3],rewtype,rewnum);
                            isok=false;
                        }

                        //other chance (halloween candle ghost, itemnum>1000)
                        if(!isok)
                        {
                            isok=true;
                            rewtype=int(BreakData.rows[i][2+j*old_break]/1000000);
                            rewnum=BreakData.rows[i][2+j*old_break]%1000000;

                            if(rewnum==0||rewtype>14||rewtype==0)
                            {
                                Log(MSG_WARNING,"Problem getting item from Break STB (line %i): %i",i,BreakData.rows[i][2+j*old_break]);
                                isok=false;
                            }
                            else
                            {
                                //Log(MSG_WARNING,"Item from Break STB retrieved (line %i): %i item (%i::%i)",i,BreakData.rows[i][2+j*3],rewtype,rewnum);
                            }

                        }

                        if(!isok)
                        {
                            continue;
                        }

                        nb_rewards++;
                        CReward* Reward = new (nothrow) CReward;
                        if(Reward==NULL)
                        {
                            Log(MSG_WARNING, "Error allocing memory Reward" );
                            return false;
                        }

                        Reward->id = rewnum;
                        Reward->type=rewtype;
                        Reward->rewardamount_min=0;

                        if(old_break!=3)
                        {
                            Reward->rewardamount_min=BreakData.rows[i][3+j*old_break];
                            Reward->rewardamount_max=BreakData.rows[i][4+j*old_break];
                            Reward->prob=BreakData.rows[i][5+j*old_break];
                            newchest->probmax+=BreakData.rows[i][5+j*old_break];
                        }
                        else
                        {
                            Reward->rewardamount_max=BreakData.rows[i][3+j*old_break];
                            Reward->prob=BreakData.rows[i][4+j*old_break];
                            newchest->probmax+=BreakData.rows[i][4+j*old_break];
                        }

                        if(Reward->rewardamount_min==0)
                        {
                            Reward->rewardamount_min=1;
                        }

                        newchest->Rewards.push_back( Reward );
                    }

                    if(old_break!=3)
                    {
                        //newchest->rewardposs = BreakData.rows[i][83];   //LMA: left for compatibility
                        newchest->reward_min=BreakData.rows[i][82];
                        newchest->reward_max=BreakData.rows[i][83];
                        newchest->nb_reward=BreakData.rows[i][84];
                    }
                    else
                    {
                        //LMA: TODO: check the min in the "old" STB
                        //newchest->rewardposs = BreakData.rows[i][62];   //LMA: left for compatibility
                        newchest->reward_min=1;
                        newchest->reward_max=BreakData.rows[i][62];
                        newchest->nb_reward=BreakData.rows[i][63];
                    }

                    if (newchest->reward_min==0&&newchest->reward_max>0)
                    {
                        newchest->reward_min=1;
                    }

                    nb_chest++;
                    ChestList.push_back( newchest );
                    //Log(MSG_INFO,"Chest added: (%i:%i), numtogive %i",10,newchest->chestid,newchest->rewardposs);
                    break;
                }
                case 3:
                {
                    //blue craft... Not yet handled...
                    //ChestList.push_back( newbreak );
                    nb_craft++;
                    break;
                }
                case 0:
                {
                    Log(MSG_WARNING,"Can't interpret itemid %i in list_break.stb",BreakData.rows[i][1]);
                    break;
                }

        }


    }

    Log(MSG_INFO,"We parsed %i Breaks, %i chests, %i blue craft",nb_break,nb_chest,nb_craft);
    Log( MSG_LOAD, "Break / Chest / Blue Craft - STB - Done" );


    return true;
}

bool CWorldServer::LoadGoList ()
{
	Log (MSG_INFO, "Loading Go List data");
	MYSQL_ROW row;
	MYSQL_RES *result = DB->QStore ("SELECT id, lvlmin, access_level, map_id, locx, locy, isactive, mapname FROM list_golist");
	if (result == NULL)
	{
		DB->QFree ();
		return false;
	}
	if (mysql_num_rows (result) == 0)
	{
		DB->QFree ();
		return false;
	}
	while (row = mysql_fetch_row (result))
	{
		GoCommand *tempdata = new (nothrow) GoCommand;
		if (tempdata != NULL)
		{ 
			tempdata->Go_id			= atoi (row[0]);
			tempdata->Lvl_min		= atoi (row[1]);
			tempdata->Access_lvl	= atoi (row[2]);
			tempdata->Mapid			= atoi (row[3]);
			tempdata->Coord.x		= (float)atoi (row[4]);
			tempdata->Coord.y		= (float)atoi (row[5]);
			tempdata->Isactive		= (bool)atoi (row[6]);
			tempdata->MapName		= _strdup (row[7]);
			GoList.push_back (tempdata);
		}
	}
	DB->QFree ();
	return true;
}

// From osprose
bool CWorldServer::LoadConfig( )
{
    Log( MSG_INFO, "Loading database config" );
    MYSQL_ROW row;
    MYSQL_RES *result = DB->QStore("SELECT exp_rate, drop_rate, zuly_rate, blue_chance, stat_chance, slot_chance, \
        refine_chance, rare_refine, kill_on_fail, player_damage, monster_damage, player_acc, monster_acc, \
        pvp_acc, skill_damage, maxlevel, drop_type, savetime, partygap, maxstat, cfmode, autosave, mapdelay, \
        visualdelay, worlddelay, fairymode, fairystay, fairywait, fairytestmode, osrosever, testgrid, jrose, \
        is_pegasus, monmax, massexport, uwnbplayers, uwside, pc_drop_zuly, drop_rev, advertise_active, advertise_time, item_drop_rate, max_refine, no_trade_unequip, more_sockets FROM list_config");

    if(result==NULL)
    {
        DB->QFree( );
        return false;
    }
    if (mysql_num_rows(result) == 0)
    {
        DB->QFree( );
        return false;
    }

    while( row = mysql_fetch_row(result) )
    {
       GServer->Config.EXP_RATE = atoi(row[0]);
       GServer->Config.DROP_RATE = atoi(row[1]);
       GServer->Config.ItemDropRate = 1;        //can be added to config database table later
       GServer->Config.ZULY_RATE = atoi(row[2]);
       GServer->Config.BlueChance = atoi(row[3]);
       GServer->Config.StatChance = atoi(row[4]);
       GServer->Config.SlotChance = atoi(row[5]);
       GServer->Config.RefineChance = atoi(row[6]);
       GServer->Config.Rare_Refine = atoi(row[7]);
       GServer->Config.KillOnFail = atoi(row[8]);
       GServer->Config.PlayerDmg = atoi(row[9]);
       GServer->Config.MonsterDmg = atoi(row[10]);
       // Not implemented in server yet - Drakia
       //GServer->Config.PlayerAcc = atoi(row[11]);
       //GServer->Config.MonsterAcc = atoi(row[12]);
       //GServer->Config.PvpAcc = atoi(row[13]);
       //GServer->Config.SkillDmg = atoi(row[14]);
        // new update - from PurpleYouko
       GServer->Config.MaxLevel = atoi(row[15]);
       GServer->Config.DROP_TYPE = atoi(row[16]);
       GServer->Config.SAVETIME = atoi(row[17]);
       GServer->Config.Partygap = atoi(row[18]);
       GServer->Config.MaxStat = atoi(row[19]);
       GServer->Config.Cfmode = atoi(row[20]);
        // added to remove from worldserver.conf
        //need to remove old from worldserver.cpp
       GServer->Config.AutoSave = atoi(row[21]);
       //LMA: in osRose we are using: AUTOSAVE...
       GServer->Config.AUTOSAVE = atoi(row[21]);
       GServer->Config.MapDelay = atoi(row[22]);
       GServer->Config.VisualDelay = atoi(row[23]);
       GServer->Config.WorldDelay = atoi(row[24]);
       GServer->Config.FairyMode = atoi(row[25]);
       GServer->Config.FairyStay = atoi(row[26]);
       GServer->Config.FairyWait = atoi(row[27]);
       GServer->Config.FairyTestMode = atoi(row[28]);
       GServer->Config.osRoseVer = _strdup(row[29]);
       //GServer->Config.testgrid = atoi(row[30]);
	   GServer->Config.testgrid = 0;						//PY: I don't care what you put in your config table. Grids are a badly coded pile of fucking retarded crap so they are disabled
       GServer->Config.jrose = atoi(row[31]);
       GServer->Config.is_pegasus = atoi(row[32]);
       GServer->Config.monmax = atoi(row[33]);
       GServer->Config.massexport = atoi(row[34]);

	   GServer->Config.AdvertiseActive = atoi(row[39]);
       GServer->Config.AdvertiseClock = atoi(row[40]);
       GServer->Config.lastAd = 0;

       GServer->Config.ItemDropRate = atoi(row[41]);
	   GServer->Config.max_refine = atoi (row[42]);			//sets maximimum item refine level. Valid values are 9 (defualt) or 15
	   GServer->Config.no_unequip_during_trade = atoi (row[43]);
       GServer->Config.more_sockets = atoi(row[44]);        // Sets the boolean value. If true (1) then allows items other than weapons, subweapons and armore to be socketed and gemmed
	   if (GServer->Config.max_refine != 15 && GServer->Config.max_refine != 9)
	   { 
		   Log (MSG_WARNING, "Invalid value for max refine level in list_config.  value %i, changed to 9 (default). Please enter either 9 or 15", atoi (row[42]));
		   GServer->Config.max_refine = 9;

	   }

       //LMA: Union Wars.
       UWNbPlayers = atoi(row[35]);
       ObjVar[1113][2] = atoi(row[36]);

        //LMA: % of getting zuly in drop.
       GServer->Config.pc_drop_zuly = atoi(row[37]);
       if(GServer->Config.pc_drop_zuly <= 0 || GServer->Config.pc_drop_zuly >= 100)
       {
           GServer->Config.pc_drop_zuly = 30;
           Log(MSG_WARNING,"Wrong value for percentage zuly drop %i, changed to 30",atoi(row[37]),GServer->Config.pc_drop_zuly);
       }

       //LMA: Using "old" or new drop system.
       GServer->Config.drop_rev=false;
       if(atoi(row[38])!=0)
       {
           Log(MSG_INFO,"We'll use the new AND drop System");
           GServer->Config.drop_rev=true;
        }

    }

    //LMA: jRose.
    if(Config.jrose==1)
       Log (MSG_INFO, "Handling ONLY jRose client.");
    else
       Log (MSG_INFO, "Handling ONLY RoseNA client.");

    //LMA: Pegasus
    if(Config.is_pegasus==1)
       Log (MSG_INFO, "Handling Pegasus STB, AIP, QSD, database.");
    else
       Log (MSG_INFO, "Handling naRose STB, AIP, QSD, database.");

    #ifdef QHACK
        Log(MSG_INFO,"Quest Hack trapping activated.");
    #endif

    #ifdef DEBUGMSG
        DEBUGF("osRose worldserver launched.");
        DEBUGS("osRose worldserver launched.");

        #ifndef _DEBUG
            Log(MSG_INFO,"Debug messages will be outputed to windows stack.");
        #else
            Log(MSG_INFO,"Debug messages will be outputed here because of debug build.");
        #endif

    #endif

    #ifdef _DEBUG
        Log(MSG_WARNING,"osRose is currently running in debug build!");
    #endif

    DB->QFree( );

   //Checking UW values, are they ok?
   if(ObjVar[1113][2]<0||ObjVar[1113][2]>2)
   {
       ObjVar[1113][2]=0;
       GServer->DB->QExecute("UPDATE list_config SET uwside=%i",ObjVar[1113][2]);
   }

    Log( MSG_INFO, "Config Data Loaded" );

    if(GServer->Config.doubleExpActive == 1)
    {
        GServer->Config.lastDoubleMap = 0;
		doubleExpMapActivate();
    }

    LoadLottoConfig();

	LoadAdverds();


    return true;
}
