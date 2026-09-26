/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007,2008,2009 OSRose Team http://www.dev-osrose.com

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

    developed with Main erose/hrose source server + some change from the original eich source
*/
#include "character.h"
#include "worldserver.h"
#include <cmath>

// return true if this character is monster
bool CCharacter::IsMonster( )
{
    if(CharType==2 || CharType==3)
        return true;
    return false;
}

// return true if this character is player
bool CCharacter::IsPlayer( )
{
    if(CharType==1)
        return true;
    return false;
}

// return true if HP is lower to 1
bool CCharacter::IsDead( )
{
    if(Stats->HP<=0)
        return true;
    return false;
}

// return true if this character is summon
bool CCharacter::IsSummon( )
{
    if(CharType==3)
        return true;
    return false;
}

// Virtual function [return item drop]
CDrop* CCharacter::GetDrop( )
{
    return NULL;
}

// start action [attack]
void CCharacter::StartAction( CCharacter* Target, BYTE action, UINT skillid, bool restart, CCharacter* receiver )
{
    if(IsPlayer())
    {
        Log(MSG_INFO,"A Player does an action %i, skill %i",action,skillid);
    }
    else
    {
        //action = 2; //SKILL_ATTACK      force all monsters to use skill attack here.
        //skillid = 1117; //Permafrost chill.
        Log(MSG_INFO,"A Monster does an action %i, skill %i",action,skillid);
    }

    BEGINPACKET( pak, 0 );
    if (restart)
    {
       Target = GetCharTarget( );
       action = Battle->atktype;
       skillid = Battle->skillid;
    }

    //Drakia: If the target is NULL, we should only do something that doesn't require a target.
    if (Target == NULL && (action != SKILL_AOE &&action != BUFF_SELF &&action != BUFF_AOE &&action != MONSTER_BUFF_SELF &&action != AOE_TARGET && action != SUMMON_BUFF))
    {
        Log(MSG_WARNING,"We tried to start attack %i without a target",action);
        return;
    }

	//PY: Code from 137 for comparisson and later upgrade
	WORD distance = 0;
	CSkills* skill = NULL;
	UINT action137 = 1;
	if (skillid != 0) {
		skill = GServer->GetSkillByID (skillid);

		// Valid skill ?
		if (skill == NULL || skill->id == 0) return;

		// test if Character meet up to the requirements. Cancel skill usage if needed
		//if (!skill->checkUsagePrereqs (this)) return;		//We do not yet have this function here

		action137 = skill->skilltype;
	}

    //LMA: don't attack a dead or an offline player... It's stupid...
    if(Target != NULL)
    {
        if(Target->IsPlayer())
        {
            CPlayer* thisplayer=reinterpret_cast<CPlayer*>(Target);
            if(!thisplayer->Session->inGame)
            {
                //Log(MSG_HACK,"We don't attack a player not in game yet..., %s",thisplayer->CharInfo->charname);
                return;
            }

            // PvP Safety Check: do not allow hostile action against player if map PvP is disabled
            if(action == NORMAL_ATTACK || action == SKILL_ATTACK)
            {
                if(!CanAttackCharacter(Target))
                {
                    ClearBattle(Battle);
                    return;
                }
            }
        }

        if(Target->IsDead())
        {
            //but we can if it's a friendly (restore...).
            if(action != SKILL_BUFF)
            {
                ClearBattle(Battle);
                return;
            }
        }

        // Bonfires cannot be attacked
        if ( Target->IsMonster( ) )
        {
            CMonster* mon = reinterpret_cast<CMonster*>( Target );
            if ( mon->IsBonfire( ) && action != SKILL_BUFF )
            {
                ClearBattle( Battle );
                return;
            }
        }
		if (Target == this)
			distance = 0;								// don't bother calculating distance if we are self targeted
		else
			distance = (int)GServer->distance (this->Position->current, Target->Position->current);

    }

    switch(action)
    {
		case NORMAL_ATTACK:
        {
            RESETPACKET( pak, 0x798 );
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, Target->clientid );
            ADDWORD    ( pak, Stats->Base_Speed );
            ADDFLOAT   ( pak, Target->Position->current.x * 100 );
            ADDFLOAT   ( pak, Target->Position->current.y * 100 );
            Battle->target = Target->clientid;
            Battle->atktarget = Target->clientid;
            Battle->atktype = action;

            if ( IsTargetReached( Target ) )
            {
                Position->destiny = Position->current;
            }
            else
            {
                Position->destiny = Target->Position->current;
            }
            Position->lastMoveTime = clock( );
        }
        break;
        case SKILL_ATTACK:
        case SKILL_BUFF:
        {
            CSkills* sk = GServer->GetSkillByID( skillid );

        	RESETPACKET( pak, 0x7b3 );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, Target->clientid );
        	ADDWORD    ( pak, skillid );
        	ADDWORD    ( pak, distance );
            ADDFLOAT   ( pak, Target->Position->current.x * 100 );
            ADDFLOAT   ( pak, Target->Position->current.y * 100 );
            ADDFLOAT   ( pak, Position->aoedestiny.x * 100 );
            ADDFLOAT   ( pak, Position->aoedestiny.y * 100 );
            Battle->target = Target->clientid;

            if(action == SKILL_ATTACK)
            {
                Battle->skilltarget = Target->clientid;
                Battle->atktarget = Target->clientid;
            }
            else
            {
                Battle->bufftarget = Target->clientid;
            }

            Battle->atktype = action;
            if ( IsTargetReached( Target, sk ) )
            {
                Position->destiny = Position->current;
            }
            else
            {
                Position->destiny = Target->Position->current;
            }
            Battle->skillid = skillid;
            Position->lastMoveTime = clock( );
        }
        break;
        case MONSTER_SKILL_ATTACK:
        case MONSTER_SKILL_BUFF:
        {
            CSkills* sk = GServer->GetSkillByID( skillid );

        	RESETPACKET( pak, 0x7b3 );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, Target->clientid );
        	ADDWORD    ( pak, skillid );
            ADDBYTE    ( pak, 0x2b );
            ADDBYTE    ( pak, 0x01 );
            ADDFLOAT   ( pak, Target->Position->current.x * 100 );
            ADDFLOAT   ( pak, Target->Position->current.y * 100 );
            ADDBYTE    ( pak, 0x06 );
            Battle->target = Target->clientid;

            if(action == MONSTER_SKILL_ATTACK)
            {
                Battle->skilltarget = Target->clientid;
                Battle->atktarget = Target->clientid;
            }
            else
            {
                Battle->bufftarget = Target->clientid;
            }

            Battle->atktype = action;
            if ( IsTargetReached( Target, sk ) )
            {
                Position->destiny = Position->current;
            }
            else
            {
                Position->destiny = Target->Position->current;
            }
            Battle->skillid = skillid;
            Position->lastMoveTime = clock( );
        }
        break;
        case SUMMON_BUFF:
        {
        	RESETPACKET( pak, 0x7b3 );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, clientid );
        	ADDWORD    ( pak, skillid );
            ADDBYTE (pak,0x2b);
            ADDBYTE (pak,0x01);
            ADDFLOAT   ( pak, Position->current.x*100 );
            ADDFLOAT   ( pak, Position->current.y*100 );
            ADDBYTE    ( pak, 0x06);
            Battle->target = 0;
            Battle->bufftarget = clientid;
            Battle->atktype = action;
            //Position->destiny  = Target->Position->current;			//PY Unnecessary.this action is repeated in DoAttack
            Battle->skillid = skillid;
            Position->lastMoveTime = clock( );
            GServer->SendToVisible( &pak, this );
            return;

        }
        break;
        case SKILL_AOE:
        case BUFF_SELF:
        case BUFF_AOE:
        {
            RESETPACKET( pak, 0x7b2);
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, skillid );
            Battle->atktype = action;
            Battle->skillid = skillid;
        }
        break;
        case MONSTER_BUFF_SELF:
        {
            RESETPACKET( pak, 0x7b2);
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, skillid );
            Battle->atktype = action;
            Battle->skillid = skillid;
        }
        break;
        case AOE_TARGET:
        {
            //LMA 2008/09/02: new version, the target is a zone, not a monster... so we stick with aoedestiny ;)
            //PY these skills cast AOE around current position so...
			Position->aoedestiny = Position->current;
			Position->destiny  = Position->current;
			RESETPACKET( pak, 0x7b4 );
            ADDWORD    ( pak, clientid );
            ADDWORD    ( pak, skillid );
            ADDFLOAT   ( pak, Position->aoedestiny.x*100 );
            ADDFLOAT   ( pak, Position->aoedestiny.y*100 );
            Battle->atktype = action;
            Battle->skillid = skillid;
            Battle->skilltarget = 0;
            Battle->target = 0;
            Position->lastMoveTime = clock( );
            Log(MSG_INFO,"StartAction, AOE_TARGET, target (%.2f,%.2f)",Position->aoedestiny.x,Position->aoedestiny.y);
        }
        break;
        case STAY_STILL_ATTACK:
        {
             //LMA: Very special case where the monster don't really attack (mc)
             Battle->atktype = action;
             Battle->skillid = skillid;
             Battle->skilltarget = Target->clientid;
             return;
        }
		break;
		
        default: return;
    }
    //if (getClient()==NULL)
       GServer->SendToVisible( &pak, this );
    /*else
        getClient();*/

    if (!IsPlayer()) Battle->contatk = true;
}

bool CCharacter::IsOnBattle( )
{
    //Bonfire don't attack...
    if(Battle->atktype==0) return false;
    switch(Battle->atktype)
    {
        case NORMAL_ATTACK:
        {
            if(Battle->atktarget!=0) return true;
        }
        break;
        case SKILL_ATTACK:
		{
			if (Battle->skilltarget != 0 && Battle->skillid != 0) return true;
		}
		break;
        case AOE_TARGET:
        {
            if(Battle->skillid != 0) return true;
        }
        break;
        case SKILL_BUFF:
        {
            if(Battle->bufftarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case SUMMON_BUFF:
        {
             if(Battle->bufftarget!=0&&Battle->skillid!=0) return true;
        }
        break;
        case MONSTER_SKILL_BUFF:
        {
             if(Battle->bufftarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case MONSTER_SKILL_ATTACK:
        {
             if(Battle->skilltarget!=0 && Battle->skillid!=0) return true;
        }
        break;
        case MONSTER_BUFF_SELF:
        case SKILL_AOE:
        case SKILL_SELF:
        case BUFF_SELF:
        case BUFF_AOE:
        {
            if(Battle->skillid != 0) return true;
        }
        break;
        case STAY_STILL_ATTACK:
        {
              //LMA: Very special case where the monster don't really attack (mc)
              return true;
         }
        default: return false;
    }
    return false;
}

bool CCharacter::CanAttack( ) // updated by Core
{
    // we cannot attack while sleeping or stunned
    if (Status->Sleep != 0xff || Status->Stun != 0xff)
    {
        return false;
    }

    if (Battle->lastAtkTime == 0)
    {
        return true;
    }

    if (0 == Stats->Attack_Speed)
    {
        return false;
    }

    clock_t etime = clock() - Battle->lastAtkTime;
    int weapontype = IsPlayer() ? getWeaponType() : 0;
    if (weapontype == 212)
    {
        return true;
    }

    float speed_percent = 1.0f + ((float)Stats->Attack_Speed_Percent / 100.0f);
    float effective_speed = (float)Stats->Attack_Speed * speed_percent;
    if (effective_speed <= 0.0f)
    {
        return false;
    }

    float delay_sec = 0.0f;
    if (weapontype == BOW || weapontype == GUN || weapontype == LAUNCHER || weapontype == CROSSBOW)
    {
        delay_sec = (float)(GServer->ATTK_SPEED_MODIF * 4 / 3) / effective_speed;
    }
    else
    {
        delay_sec = 100.0f / effective_speed;
    }

    if (etime < (clock_t)(CLOCKS_PER_SEC * delay_sec))
    {
        return false;
    }

    return true;
}

fPoint CCharacter::GetCombatPosition( CCharacter* target, float combatDist )
{
    fPoint dest = Position->current;
    if ( !target ) return dest;

    if ( combatDist <= 0.0f )
    {
        float atkDist = Stats->Attack_Distance > 1.5f ? Stats->Attack_Distance : 1.5f;
        if ( atkDist > 3.5f )
        {
            combatDist = atkDist * 0.85f;
        }
        else
        {
            combatDist = ( atkDist * 0.8f < 1.8f ) ? 1.8f : ( atkDist * 0.8f );
            if ( combatDist > atkDist - 0.3f && atkDist > 1.8f )
            {
                combatDist = atkDist - 0.3f;
            }
        }
    }

    float dx = Position->current.x - target->Position->current.x;
    float dy = Position->current.y - target->Position->current.y;
    float curDist = sqrt( ( dx * dx ) + ( dy * dy ) );

    if ( curDist > 0.05f )
    {
        dest.x = target->Position->current.x + ( dx / curDist ) * combatDist;
        dest.y = target->Position->current.y + ( dy / curDist ) * combatDist;
        dest.z = 0.0f;
    }
    else
    {
        // Target and attacker are at identical coordinates: choose an offset based on clientid
        float angle = (float)( ( clientid * 73 ) % 360 ) * ( 3.14159265f / 180.0f );
        dest.x = target->Position->current.x + cos( angle ) * combatDist;
        dest.y = target->Position->current.y + sin( angle ) * combatDist;
        dest.z = 0.0f;
    }
    return dest;
}

// return true if target is reached
bool CCharacter::IsTargetReached( CCharacter* Enemy, CSkills* skill )
{
	if (Enemy == NULL) return true;
	CMap* map = GServer->MapList.Index[Position->Map];
    float distance = GServer->distance( Position->current, Enemy->Position->current );

    float reach = Stats->Attack_Distance > 1.5f ? Stats->Attack_Distance : 1.5f;
    if (skill != NULL)
    {
        reach = (float)skill->range;
        if (reach <= 0.0f)
            reach = Stats->Attack_Distance > 1.5f ? Stats->Attack_Distance : 1.5f;
    }

    float target_allowance = 1.7f;
    if (Enemy->IsMonster())
    {
        CMonster* mon = (CMonster*)Enemy;
        if (mon->thisnpc != NULL && mon->thisnpc->atkdistance > 0.0f)
        {
            target_allowance = 1.2f + (mon->thisnpc->atkdistance * 0.3f);
            if (target_allowance < 1.7f) target_allowance = 1.7f;
        }
    }

    return (distance <= (reach + target_allowance));
}

// return true if the character is moving
bool CCharacter::IsMoving( )
{
    if(Position->current.x == Position->destiny.x && Position->current.y == Position->destiny.y )
        return false;
    return true;
}

bool CCharacter::stopMoving( bool send_packet )
{
     if( Position->current.x == Position->destiny.x && Position->current.y == Position->destiny.y )
        return false;
     Position->destiny = Position->current; //STOP MOVING

     // Never send movement packets during battle - doing so interrupts attack animations and causes stance flickering
     if ( send_packet && !IsOnBattle() )
     {
         if ( IsPlayer() )
         {
             BEGINPACKET( pak, 0x770 );
             ADDWORD    ( pak, clientid );
             ADDFLOAT   ( pak, Position->current.x * 100 );
             ADDFLOAT   ( pak, Position->current.y * 100 );
             ADDWORD    ( pak, (WORD)( Position->current.z * 100 ) );
             GServer->SendToVisible( &pak, this );
         }
         else if ( IsMonster() )
         {
             CMonster* mon = (CMonster*)this;
             BEGINPACKET( pak, 0x797 );
             ADDWORD    ( pak, clientid );
             ADDWORD    ( pak, 0x0000 );
             ADDWORD    ( pak, Stats->Move_Speed );
             ADDFLOAT   ( pak, Position->current.x * 100 );
             ADDFLOAT   ( pak, Position->current.y * 100 );
             ADDWORD    ( pak, 0xcdcd );
             ADDBYTE    ( pak, mon->thisnpc ? mon->thisnpc->stance : 0 );
             GServer->SendToVisible( &pak, this );
         }
     }

     return true;
}

// VIRTUAL [add player damage]
void CCharacter::AddDamage( CCharacter* enemy, long long hitpower)
{
    Log(MSG_INFO,"In CCharacter::AddDamage");
}

// return character target
CCharacter* CCharacter::GetCharTarget( )
{
    CCharacter* thistarget = GServer->MapList.Index[Position->Map]->GetCharInMap( Battle->target );
    if (thistarget != NULL)
    {
        //Log(MSG_DEBUG, "getchartarget found target OK");
        return thistarget;
    }
    //Log(MSG_DEBUG, "getchartarget failed to find target");
    //if I don't currently have a target then return the character who just hit me
    thistarget = GServer->MapList.Index[Position->Map]->GetCharInMap( Battle->hitby );
    //if (thistarget == NULL)
    //{
    //    Log(MSG_DEBUG, "getcharinmap(battle->hitby) still failed to find a target");
    //    return thistarget;
    //}
    // still no target. Let's find teh closest player
    //CPlayer* thisplayer = NULL;
    //float distance = 0xffff;
    //for(UINT i=0;i<GServer->MapList.Index[Position->Map]->PlayerList.size();i++)
    //{
    //    CPlayer* thisclient = GServer->MapList.Index[Position->Map]->PlayerList.at(i);
    //    float tempdist = GServer->distance( Position->current, thisclient->Position->current );
    //    if(tempdist<distance)
    //    {
    //        thisplayer = thisclient;
    //        distance = tempdist;
    //    }
    //}
    //thistarget = (CCharacter*) thisplayer;
    //if (thistarget == NULL)
    //{
    //    Log(MSG_DEBUG, "still failed to find a target");
    //}
    //else
    //{
    //    Log(MSG_DEBUG, "OK Found a target.. finally id = ", thistarget->clientid);
    //}
    return thistarget;
}

// return character Buff target
CCharacter* CCharacter::GetCharBuffTarget( )
{
    return GServer->MapList.Index[Position->Map]->GetCharInMap( Battle->bufftarget );
}

// VIRTUAL [reduce ammon/bullets/cannons]
void CCharacter::ReduceABC( int amount, bool do_packet )
{
	return;
}

// VIRTUAL //LMA: Searching for replacement arrows, bullets...
int CCharacter::SearchABC ( int slot )
{
	return 0;
}

// VIRTUAL [get extra buff value]
int CCharacter::GetEVBuff( )
{
    return 0;
}

// VIRTUAL [return intelligence]
unsigned int CCharacter::GetInt( )
{
    return 15;
}

// VIRTUAL [return Sensibility]
unsigned int CCharacter::GetSen( )
{
    return 10;
}

// VIRTUAL [return Concentration]
unsigned int CCharacter::GetCon( )
{
    return 15;
}

void CCharacter::RefreshBuff( )
{
    bool bflag = false;
    for( UINT i=0;i<30;i++)
    {
        if(MagicStatus[i].Buff == 0) continue;
        clock_t etime = clock() - MagicStatus[i].BuffTime;
        if( etime >= MagicStatus[i].Duration * CLOCKS_PER_SEC )
        {

            Log(MSG_INFO,"Magic Status %i, vanish after: %i", MagicStatus[i].Status, MagicStatus[i].Duration);

            switch(MagicStatus[i].Status)
            {
                case 18: // attack power up
                case 19: // attack power down
                case 48: // attack power up
                {
                    if(i<15)
                    {
                        Status->Attack_up = 0xff;
                    }
                    else
                    {
                        Status->Attack_down = 0xff;
                    }
                    Stats->Attack_Power = GetAttackPower( );
                }
                break;
                case 20: // def up
                case 21: // def down
                case 49: // def up
                {
                    if(i<15)
                    {
                        Status->Defense_up= 0xff;
                    }
                    else
                    {
                        Status->Defense_down = 0xff;
                    }
                    Stats->Defense = GetDefense( );
                }
                break;
                case 24: //Accuracy up
                case 25: //Accuracy down
                case 51: //attack Accuracy up.
                {
                    if(i<15)
                    {
                        Status->Accuracy_up= 0xff;
                    }
                    else
                    {
                        Status->Accuracy_down = 0xff;
                    }
                    Stats->Accuracy = GetAccuracy( );
                }
                break;
                case 22: // macic resistance up
                case 23: // magic resistance down
                case 50: // magic resistance up
                {
                    if(i<15)
                    {
                        Status->Magic_Defense_up = 0xff;
                    }
                    else
                    {
                        Status->Magic_Defense_down = 0xff;
                    }
                    Stats->Magic_Defense = GetMagicDefense( );
                }
                break;
                case 28: //dodge up
                case 29: //dodge down
                case 53: //dodge rate up
                {
                    if(i<15)
                    {
                        Status->Dodge_up = 0xff;
                    }
                    else
                    {
                        Status->Dodge_down = 0xff;
                    }
                    Stats->Dodge = GetDodge( );
                }
                break;
                case 14: //dash
                case 15: //slow
                case 46: //movement speed increased
                {
                    if(i<15)
                    {
                        Status->Dash_up = 0xff;
                    }
                    else
                    {
                        Status->Dash_down = 0xff;
                    }
                    Stats->Move_Speed = GetMoveSpeed( );
                }
                break;
                case 16: // haste attack
                case 17: // slow attack
                case 47: // attack speed up
                {
                    if(i<15)
                    {
                        Status->Haste_up = 0xff;
                    }
                    else
                    {
                        Status->Haste_down = 0xff;
                    }
                    Stats->Attack_Speed_Percent = GetAttackSpeedPercent( );
                }
                break;
                case 26: // crit up
                case 27: // crit down
                case 52: // crit up
                {
                    if(i<15)
                    {
                        Status->Critical_up = 0xff;
                    }
                    else
                    {
                        Status->Critical_down = 0xff;
                    }
                    Stats->Critical = GetCritical( );
                }
                break;
                case 12: // max HP up
                case 44: // max HP up
                {
                    if(i<15)
                    {
                        Status->HP_up = 0xff;
                    }
                    else
                    {
                        Status->HP_down = 0xff;
                    }
                    Stats->MaxHP = GetMaxHP( );

                    if(Stats->HP > Stats->MaxHP)
                    {
                        Stats->HP = Stats->MaxHP;
                    }
                }
                break;
                case 13: // max MP up
                case 45: // max MP up
                {
                    if(i<15)
                    {
                        Status->MP_up = 0xff;
                    }
                    else
                    {
                        Status->MP_down = 0xff;
                    }
                    Stats->MaxMP = GetMaxMP( );

                    if(Stats->MP > Stats->MaxMP)
                    {
                        Stats->MP = Stats->MaxMP;
                    }
                }
                break;
                case 32: // faint
                {
                    Status->Stuned = 0xff;
                    Status->CanAttack = true;
                    //printf("removing stun\n");
                }
                break;
                case 7: case 8: case 9: case 10: case 11: case 89: //poisoned
                {
                    Status->Poisoned = 0xff;
                    //printf("removing poison\n");
                }
                break;
                case 30: // muted
                {
                    Status->Muted = 0xff;
                    Status->CanCastSkill = true;
                }
                break;
                case 31: // sleep May need to be fixed later to accomodate multiple status effects.
                {
                    Status->Sleep = 0xff;
                    Status->CanMove = true;
                    Status->CanCastSkill = true;
                    Status->CanAttack = true;
                }
                break;
                case 36: //A_Extra_Damage:
                case 54: //A_GMExtra_Damage:
                case 83: //Valkyrie Charm:
                {
                    if(i<15)
                    {
                        Status->ExtraDamage_up = 0xff;
                        Stats->ExtraDamage_add = 0;//We put extardamage add value to 0 if we lost the adddmg buff
                    }
                    else
                    {
                        Status->ExtraDamage_down = 0xff;
                        Stats->ExtraDamage_add = 0;//We put extardamage add value to 0 if we lost the adddmg buff
                    }
                }
                break;
                case 56: //Taunt
                {
                    Status->Taunt = 0xff;
                    //printf("removing Taunt\n");
                }
                break;
                case 58: case 61: case 71: case 77:  case 78: case 79: case 80: case 88: //flame
                {
                    Status->Flamed = 0xff;
                }
                break;
                case 33://Stealth,Camoflauge
                {
                    if(IsAttacking( ))
                    {
                        MagicStatus[i].Duration = 0;
                    }
                    Status->Stealth = 0xff;
                    //printf("removing Stealth\n");
                }
                break;
                case 86://Stealth,Weary
                {
                    Status->Weary = 0xff;
                    Status->CanCastSkill = true;
                    //printf("removing Weary\n");
                }
                break;
                case 34://Cloaking
                {
                    Status->Cloaking = 0xff;
                    //printf("removing Cloaking\n");
                }
                break;
                case 35: //ShieldDamage:
                {
                    if(i<15)
                    {
                        Status->ShieldDamage_up = 0xff;
                        //Stats->ShieldDamage = 0xff;
                    }
                    else
                    {
                        Status->ShieldDamage_down = 0xff;
                        //Stats->ShieldDamage = 0xff;
                    }
                }
                break;
                case 55://Detect
                {
                    Status->Detect = 0xff;
                    Status->Cloaking = 0xff;
                    Status->Stealth = 0xff;
                    //printf("Detect Done\n");
                }
                break;
                case 38://Purify
                {
                    //Buff_Down
                    Status->Attack_down = 0xff;
                    Status->Defense_down = 0xff;
                    Status->Accuracy_down = 0xff;
                    Status->Magic_Defense_down = 0xff;
                    Status->Dodge_down = 0xff;
                    Status->Dash_down = 0xff;
                    Status->Haste_down = 0xff;
                    Status->Critical_down = 0xff;
                    Status->HP_down = 0xff;
                    Status->MP_down = 0xff;
                    Status->ExtraDamage_down = 0xff;
                    Status->ShieldDamage_down = 0xff;

                    //Bad Status
                    Status->Stun = 0xff;
                    Status->Poisoned = 0xff;
                    Status->Muted = 0xff;
                    Status->Sleep = 0xff;
                    Status->Flamed = 0xff;

                    //Stats
                    Stats->Attack_Power = GetAttackPower( );
                    Stats->Defense = GetDefense( );
                    Stats->Accuracy = GetAccuracy( );
                    Stats->Magic_Defense = GetMagicDefense( );
                    Stats->Dodge = GetDodge( );
                    Stats->Move_Speed = GetMoveSpeed( );
                    Stats->Attack_Speed = GetAttackSpeed( );
                    Stats->Attack_Speed_Percent = GetAttackSpeedPercent();
                    Stats->Critical = GetCritical( );
                    Stats->MaxHP = GetMaxHP( );
                    Stats->MaxMP = GetMaxMP( );
                    //printf("Purify Done\n");
                }
                break;
				case 39:// Dispell
                {
                    //Buff_Down
                    Status->Attack_down = 0xff;
                    Status->Defense_down = 0xff;
                    Status->Accuracy_down = 0xff;
                    Status->Magic_Defense_down = 0xff;
                    Status->Dodge_down = 0xff;
                    Status->Dash_down = 0xff;
                    Status->Haste_down = 0xff;
                    Status->Critical_down = 0xff;
                    Status->HP_down = 0xff;
                    Status->MP_down = 0xff;
                    Status->ExtraDamage_down = 0xff;
                    Status->ShieldDamage_down = 0xff;

                    //Buff_Up
                    Status->Attack_up = 0xff;
                    Status->Defense_up = 0xff;
                    Status->Accuracy_up = 0xff;
                    Status->Magic_Defense_up = 0xff;
                    Status->Dodge_up = 0xff;
                    Status->Dash_up = 0xff;
                    Status->Haste_up = 0xff;
                    Status->Critical_up = 0xff;
                    Status->HP_up = 0xff;
                    Status->MP_up = 0xff;
                    Status->ExtraDamage_up = 0xff;
                    Status->ShieldDamage_up = 0xff;

                    //Bad Status
                    Status->Stun = 0xff;
                    Status->Poisoned = 0xff;
                    Status->Muted = 0xff;
                    Status->Sleep = 0xff;
                    Status->Flamed = 0xff;

                    //Stats
                    Stats->Attack_Power = GetAttackPower( );
                    Stats->Defense = GetDefense( );
                    Stats->Accuracy = GetAccuracy( );
                    Stats->Magic_Defense = GetMagicDefense( );
                    Stats->Dodge = GetDodge( );
                    Stats->Move_Speed = GetMoveSpeed( );
                    Stats->Attack_Speed = GetAttackSpeed( );
                    Stats->Critical = GetCritical( );
                    Stats->MaxHP = GetMaxHP( );
                    Stats->MaxMP = GetMaxMP( );
                    //printf("Dispell Done\n");
                }
                break;
                default:
                {
                    Log(MSG_WARNING,"Unknow skill status in charfunctions %u.",MagicStatus[i].Status);
                }
            }

            MagicStatus[i].Status = 0;
            MagicStatus[i].Buff = 0;
            MagicStatus[i].BuffTime = 0;
            MagicStatus[i].Duration = 0;
            MagicStatus[i].Value = 0;
            bflag = true;
        }
        else if ( ((MagicStatus[i].Status >= 7 && MagicStatus[i].Status <= 11) || MagicStatus[i].Status == 89) && etime > 1*CLOCKS_PER_SEC) //Do poison dmg every 1.5 seconds
        {
             Stats->HP -= MagicStatus[i].Status; //Actually take 7, 8, 9, 10 or 11 from the health. Based on the Status itself, LMA: can be 89 noc too.
             MagicStatus[i].BuffTime += 1*CLOCKS_PER_SEC;
             MagicStatus[i].Duration -= 1;
             //printf("did %i poison dmg to the player, still %i seconds and %i HP remain \n", MagicStatus[i].Status, MagicStatus[i].Duration, Stats->HP);
             //Log(MSG_WARNING,"did %i poison dmg to the player / monster, still %i seconds and %I64i HP remain", MagicStatus[i].Status, MagicStatus[i].Duration, Stats->HP);

            //LMA: If dead, let's the client resynch
            if(IsDead())
            {
                BEGINPACKET( pak, 0x79f );
                ADDWORD    ( pak, clientid );
                ADDDWORD   ( pak, 1);
                GServer->SendToVisible( &pak, this );
                Log(MSG_INFO,"death poison for %i, amount: %i",clientid,MagicStatus[i].Status);
            }

             //A bunch of messy code to send dmg packet
             BEGINPACKET( pak, 0x7b6 );
             ADDWORD    ( pak, clientid );
             ADDWORD    ( pak, 0 );
             ADDDWORD   ( pak, 0x000007f8 );
             ADDBYTE    ( pak, 0x00 );
             ADDDWORD   ( pak, MagicStatus[i].Status );

             //If Enemy is killed
             if( IsDead())
             {
                 //printf("char died\n");
                 CDrop* thisdrop = NULL;
                 ADDDWORD   ( pak, 16 );
                 if( !IsSummon( ) && !IsPlayer( ))
                 {
                    //LMA: No drop if already dead and drop done.
                    if(drop_dead)
                    {
                        Log(MSG_WARNING,"Trying to make a monster (CID %u, type %u) drop again but already did.",clientid,char_montype);
                    }
                    else
                    {
                         thisdrop = GetDrop( );
                         if( thisdrop!=NULL)
                         {
                             CMap* map = GServer->MapList.Index[thisdrop->posMap];
                             map->AddDrop( thisdrop );
                         }

                    }

                 }

                 //GServer->SendToVisible( &pak, this, thisdrop );
                 GServer->SendToVisible( &pak, this);
             }
             else
             {
                 //If enemy is still alive
                 ADDDWORD   ( pak, 4 );
                 GServer->SendToVisible( &pak, this );
             }
         }
        else if ( MagicStatus[i].Status == 58 || MagicStatus[i].Status == 61 || MagicStatus[i].Status == 71 || MagicStatus[i].Status >= 77 && MagicStatus[i].Status <= 80 || MagicStatus[i].Status == 88 && etime > 1*CLOCKS_PER_SEC) //Do flame dmg every 1.5 seconds
        {
             Stats->HP -= MagicStatus[i].Status;
             MagicStatus[i].BuffTime += 1*CLOCKS_PER_SEC;
             MagicStatus[i].Duration -= 1;
             printf("did %i flame dmg to the player, still %i seconds and %i HP remain \n", MagicStatus[i].Status, MagicStatus[i].Duration, Stats->HP);

            //LMA: If dead, let's the client resynch
            if(IsDead())
            {
                BEGINPACKET( pak, 0x79f );
                ADDWORD    ( pak, clientid );
                ADDDWORD   ( pak, 1);
                GServer->SendToVisible( &pak, this );
                Log(MSG_INFO,"death flame for %i, amount: %i",clientid,MagicStatus[i].Status);
            }

             //A bunch of messy code to send dmg packet
             BEGINPACKET( pak, 0x7b6 );
             ADDWORD    ( pak, clientid );
             ADDWORD    ( pak, 0 );
             ADDDWORD   ( pak, 0x000007f8 );
             ADDBYTE    ( pak, 0x00 );
             ADDDWORD   ( pak, MagicStatus[i].Status );

             //If Enemy is killed
             if( IsDead())
             {
                 //printf("char died\n");
                 CDrop* thisdrop = NULL;
                 ADDDWORD   ( pak, 16 );
                 if( !IsSummon( ) && !IsPlayer( ))
                 {
                    //LMA: No drop if already dead and drop done.
                    if(drop_dead)
                    {
                        Log(MSG_WARNING,"Trying to make a monster (CID %u, type %u) drop again but already did.",clientid,char_montype);
                    }
                    else
                    {
                         thisdrop = GetDrop( );
                         if( thisdrop!=NULL)
                         {
                             CMap* map = GServer->MapList.Index[thisdrop->posMap];
                             map->AddDrop( thisdrop );
                         }

                    }

                 }
                 //GServer->SendToVisible( &pak, this, thisdrop );
                 GServer->SendToVisible( &pak, this);
             }
             else
             {
                 //If enemy is still alive
                 ADDDWORD   ( pak, 4 );
                 GServer->SendToVisible( &pak, this );
             }
         }

    }
    if(bflag)
    {
        BEGINPACKET( pak,0x7b7 );
        ADDWORD    ( pak, clientid );
        ADDDWORD   ( pak, GServer->BuildBuffs( this ) );
        GServer->SendToVisible( &pak, this );
    }
}


// VIRTUAL [return party pointer]
CParty* CCharacter::GetParty( )
{
    return NULL;
}

// VIRTUAL [return clan pointer]
CLAN* CCharacter::GetClan( )
{
    return NULL;
}

bool CCharacter::IsAttacking( )
{
    if(IsOnBattle( ))
    {
        switch(Battle->atktype)
        {
            case SKILL_BUFF:
            case BUFF_SELF:
            case BUFF_AOE:
                return false;
        }
    }
    else return false;
    return true;
}

// [VIRTUAL]
void CCharacter::reduceItemsLifeSpan(bool atacked)
{
	return;
}

// [VIRTUAL]
unsigned CCharacter::getWeaponType()
{
	return 0;
}

/*
// [VIRTUAL]
bool CCharacter::AddClanPoints(unsigned int count)
{
}
*/

// [VIRTUAL]
CClientSocket* CCharacter::getClient()
{
   return NULL;
}
