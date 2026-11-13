# RosE Resurrected — Server Emulator

***

Recommended Client to use with Server : https://162.35.161.103/downloads/Rose-client.zip

***

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-14-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.12%2B-orange.svg)]()
[![Database](https://img.shields.io/badge/Database-MariaDB%20%2F%20MySQL-lightgrey.svg)]()
[![License](https://img.shields.io/badge/License-GPL%20v2-green.svg)]()

**RosE Resurrected** is a high-performance, modular C++ server emulator for **ROSE Online (naROSE Evolution v132–v142)**. Built upon the proven osROSE Evolution architecture, this emulator introduces modern enhancements, strict server-side exploitation defenses, an intelligent automated AI companion squad system, autonomous ambient grinding bots, world grind-spot buff clerics, dynamic dungeon instancing, persistent parcel mail, clan storage vaults, and scalable multi-platform compilation support.

---

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Current Status & Completed Milestones](#current-status--completed-milestones)
   - [Core Systems Overview](#core-systems-overview)
   - [Milestone 1 — Stability & Security](#milestone-1--stability--security-completed--verified)
   - [Milestone 2 — Quest Engine & Timers](#milestone-2--quest-engine--timers-completed--verified)
   - [Milestone 3 — PAT (Cart & Castle Gear) & Economy](#milestone-3--pat-cart--castle-gear--economy-completed--verified)
   - [Milestone 4 — Clan System Expansion](#milestone-4--clan-system-expansion-completed--verified)
   - [Milestone 5 — PvP Arena, Union Wars & Clan Wars](#milestone-5--pvp-arena-union-wars--clan-wars-completed--verified)
   - [Milestone 6 — Autonomous Vending Bots System](#milestone-6--autonomous-vending-bots-system-completed--verified)
   - [Milestone 7 — Dynamic Dungeon Instancing & Mail System](#milestone-7--dynamic-dungeon-instancing--mail-system-completed--verified)
   - [Milestone 8 — Combat Mechanics Polish, Spawn Physics & Vanguard AI](#milestone-8--combat-mechanics-polish-spawn-physics--vanguard-ai-completed--verified)
   - [Milestone 9 — Intelligent AI Companion & Party Bot System](#milestone-9--intelligent-ai-companion--party-bot-system-completed--verified)
3. [Roadmap & Upcoming Milestones](#roadmap--upcoming-milestones)
4. [Intelligent AI Companion & PlayerBot Deep-Dive](#intelligent-ai-companion--playerbot-deep-dive)
   - [1. Real Player Auto-Grouping](#1-real-player-auto-grouping)
   - [2. Dynamic Spec Detection & Party Composition](#2-dynamic-spec-detection--party-composition)
   - [3. Bi-Directional Level Synchronization](#3-bi-directional-level-synchronization)
   - [4. Dynamic Job Tier Progression & Down-Scaling](#4-dynamic-job-tier-progression--down-scaling)
   - [5. Skill Memory Wipe & Authentic Skill Verification](#5-skill-memory-wipe--authentic-skill-verification)
   - [6. Spec-Specific Combat Strategies & Rotations](#6-spec-specific-combat-strategies--rotations)
   - [7. Dynamic 2D Trigonometric Formations & Distance Tethering](#7-dynamic-2d-trigonometric-formations--distance-tethering)
   - [8. Session Persistence & Relog Refilling](#8-session-persistence--relog-refilling)
   - [9. Guild Wars 2 Style Open-World Auto-Sharing](#9-guild-wars-2-style-open-world-auto-sharing)
   - [10. Ambient Grinding Bots & D&D Dice Engine](#10-ambient-grinding-bots--dd-dice-engine)
   - [11. Worldwide Grind-Spot Buff Bots (The Cleric Sisters)](#11-worldwide-grind-spot-buff-bots-the-cleric-sisters)
   - [12. Autonomous Market Vending Bots (Junon & Zant)](#12-autonomous-market-vending-bots-junon--zant)
   - [13. Social Engine, Whispers, Duels, Trading & GM Teleport Suite](#13-social-engine-whispers-duels-trading--gm-teleport-suite)
5. [Dynamic Dungeon Instancing Engine](#dynamic-dungeon-instancing-engine)
6. [Mail & Parcel Delivery Engine](#mail--parcel-delivery-engine)
7. [Compilation Guide](#compilation-guide)
   - [Linux Build (Ubuntu / Debian / Mint / WSL2)](#linux-build-ubuntu--debian--mint--wsl2)
   - [Windows Build (Visual Studio 2017–2022)](#windows-build-visual-studio-20172022)
8. [Database Setup & Server Configuration](#database-setup--server-configuration)
9. [Starting & Managing the Servers](#starting--managing-the-servers)
10. [Maintenance & Git Synchronization Workflow](#maintenance--git-synchronization-workflow)
11. [Credits & Licensing](#credits--licensing)

---

## Architecture Overview

RosE Resurrected utilizes a distributed 3-tier server model communicating via TCP sockets using TriggerSoft 256-byte substitution cryptography:

```
                          +------------------------+
                          |   ROSE Online Client   |
                          |  (naROSE Evo Client)   |
                          +-----------+------------+
                                      |
                     +----------------+----------------+
                     |                |                |
             (Port 29000)      (Port 29100)      (Port 29200)
                     |                |                |
                     v                v                v
             +---------------+ +---------------+ +---------------+
             |  LoginServer  | |  CharServer   | |  WorldServer  |
             +-------+-------+ +-------+-------+ +-------+-------+
                     |                 |                 |
                     +-----------------+-----------------+
                                       |
                                       v
                             +-------------------+
                             |  MariaDB / MySQL  |
                             |      (roseza)     |
                             +-------------------+
```

- **Login Server (Port 29000)**: Handles account authentication, password verification, IP bans, capacity gating, and dispatching active channel lists.
- **Character Server (Port 29100)**: Manages character selection, creation, deletion, clan metadata, Alt+N clan skill blocks, friend lists, and cross-channel whispers.
- **World Server (Port 29200)**: Orchestrates zone maps, combat damage formulas, AIP creature behavior, player inventory/storage, party synchronization, quests, NPC dialogs, dynamic dungeon instances, and the entire AI PlayerBot / Companion framework.
- **Assets (`3DData/`)**: Server reads game definition files (`STB` tables, `STL` localized strings, `AIP` state scripts, `QSD` quest flows, and `IFO` object placement).

---

## Current Status & Completed Milestones

### Core Systems Overview
- **Networking & Cryptography (100%)**: TriggerSoft table encryption (`0x703` / `0x7ff`), session token validation, and SQL-injection-safe queries.
- **Character Core (95%)**: Full level progression (1–255), stat point allocation (STR, DEX, INT, CON, CHA, SEN), stamina regeneration, stance synchronization.
- **Combat Mechanics (95%)**: Normal attack cycles, physical/magical defenses, accuracy vs. dodge, critical hit rolls, shield block mitigation, resurrection, and death penalties.
- **Economy & Crafting (90%)**: 8-byte Evolution item structures, refine system up to +15 with GradeList multipliers, gem socketing, item appraisal, player trade, and personal vending shops.
- **Social Systems (95%)**: Parties with shared EXP and drop modes, friend lists, chatrooms with disconnect broadcast (`0x7e3:0x16`), and whisper/shout channels.

---

### Milestone 1 — Stability & Security (Completed & Verified)
- **PvP Town Safety Guard**: Enforced server-side checks in `pakStartAttack` and `StartAction` to prevent attacks in towns (`map->allowpvp == 0`) and block friendly-fire within teams (`pvp_id`).
- **Server Capacity Limit**: Implemented `Config.MaxConnections` enforcement in `SocketServer.cpp` and `worldserver.cpp` to reject incoming connections gracefully when full.
- **Valkyrie Charm INT Scaling**: Restored INT scaling in `Buff.cpp` for Add Damage (`A_ADDDMG`, buff 83).
- **Driving Skill Restrictions**: Permitted driving-specific skills while inside Cart/Castle Gear while rejecting on-foot skills.

---

### Milestone 2 — Quest Engine & Timers (Completed & Verified)
- **Active Timed Quest Expiration**: 1-second interval ticking loop checking quest time limits; automatically marks expired quests and dispatches client failure notifications.
- **Delayed Trigger Queue (`QUESTREWD(013)`)**: Player-bound delayed trigger execution queue processed each tick in `MainProcess.cpp`.
- **IFO Interactive Objects (`QUESTCOND(012)`)**: Evaluates WarpGates, levers, and NPC event IDs.
- **Time & Channel Conditions**: Implemented `QUESTCOND(016)` (zone time), `QUESTCOND(017)` (object variables), `QUESTCOND(022)` (server channel checks), and `QUESTREWD(012)` (LTB NPC shout/announcements).

---

### Milestone 3 — PAT (Cart & Castle Gear) & Economy (Completed & Verified)
- **Complete Cart Weapon Formulas**: Implemented unique formulas in `playerstats.cpp` for all 5 Cart weapon subclasses (Hammer, Mirror/Crystal, Bow, Katar/Spear, Launcher/Gun), with stat scaling and grade bonuses.
- **Cart Accuracy Calibration**: Eliminated accuracy warning logs with consistent offsets for all Cart weapon IDs (including legacy ID 551).
- **Equipment Repair Pricing**: Formula based on base item price and lost condition:
  $$\text{repair\_cost} = \left\lfloor \text{bprice} \times \frac{99 - \text{lifespan}}{100} \times 0.05 \right\rfloor$$
- **Storage Transaction Fees**: Configurable deposit fee deductions while keeping withdrawals free.

---

### Milestone 4 — Clan System Expansion (Completed & Verified)
- **Clan Skills Wire Protocol**: Added `skills VARCHAR(500)` column to `list_clan`. Packet `0x7e0:0x33` (`SendClanInfo`) transmits the full 120-byte skills block (60 `WORD`s) to populate the client Alt+N Clan window.
- **Clan Passive Calculation**: Unified `CPlayer::ApplyClanPassive()` applied across core attributes (STR, DEX, INT, CON, CHA, SEN) and combat stats (ATK, DEF, MDEF, ACC, DODGE, CRIT, HASTE, MOVE SPEED, MAX HP, MAX MP).
- **Clan Storage Shared Vault**: Table `clan_storage` (`clanid`, `slotnum` PK) supporting multi-slot item persistence. Clan vault opening (`0x7ad:0x10/0x20`) and deposit/withdrawal (`0x7af:0x10/0x11`) with Master/Deputy rank checks ($\ge 5$).
- **Clan Bank & Funds**: Added `money BIGINT UNSIGNED` to `list_clan` with 8-byte QWORD serialization and Zuly deposit/withdrawal (`0x7b1:0x20/0x21`).
- **Clan Quests**: Implemented `QUESTCOND(025)` (contribution check), `QUESTCOND(028)` (clan funds check), `QUESTCOND(030)` (clan skill learned check), `QUESTREWD(024)` (funds adjustment), and `QUESTREWD(026)` (clan skill learning with real-time online member stat recalculation).

---

### Milestone 5 — PvP Arena, Union Wars & Clan Wars (Completed & Verified)
- **Multi-Mode PvP Zone Engine**: Parsed `LIST_ZONE.STB` columns 18 (`allowpvp`) and 20 (`zone_type`) into 5 distinct PvP modes (`ePvpMode`):
  - **Mode 0 (Safe / PvE)**: Safe towns, normal adventure maps, Agits (Zones 15–18, 40). Zero player-versus-player combat permitted.
  - **Mode 1 (Free-for-All / FFA)**: Training Grounds (Map 6), Desert of the Dead (Map 29), Temple Campgrounds (Map 46), Forgotten Temple (56–57), Sikuku Ruins (66), Gates of Muris (82). Players can attack anyone outside their own party.
  - **Mode 2 (Team / Arena)**: Akram Arena / Colosseum (Map 9). Players are assigned to Red or Blue teams and can only attack the opposing team.
  - **Mode 3 (Clan Fields)**: Lion's Plains (Map 8), Junon Clan Fields (11–13), Luna Clan Field (59), Pedion (101–120). Clanmates share team IDs and cannot damage each other.
  - **Mode 4 (Union Wars / Cartel)**: Junon Cartel (Map 5) and Union conflict zones. Players in the same Union share team IDs.
- **Unified Combat Safety Engine (`CCharacter::CanAttackCharacter`)**: Comprehensive safety checks across normal auto-attacks, targeted skills (`pakStartAttack`, `pakStartSkill`), AoE attacks (`AoeSkill`), AoE debuffs (`AoeDebuff`), and targeted buffs/heals (cannot heal or buff hostile enemies). Protects Large Bonfires (`montype 806`) and player summons.
- **PvP Death Penalty Immunity**: Exempted all PvP zones, Clan Fields, and the Arena from death EXP loss in `battle.cpp:TakeExp`.
- **Akram Arena / Colosseum Manager (`CArenaManager`)**:
  - Automated round states (`IDLE`, `COUNTDOWN`, `ACTIVE`, `ENDED`).
  - Auto-balances joined players into Red Team (Base 301 at `5211.4f, 4867.9f`) and Blue Team (Base 302 at `5200.5f, 4725.1f`).
  - Automated respawn timer (5 seconds) reviving dead combatants directly at their team base with full HP/MP and no death penalties.
  - Round end triggered by 10 kills or 5-minute timeout.
  - Rewards: +50,000 Zulies and +50,000 EXP to winners; +15,000 Zulies and +15,000 EXP consolation to losers.
  - In-game commands: `/arena join`, `/arena leave`, `/arena score`, `/arena status`, `/arena start`, `/arena stop`.
- **Union Wars & Clan Field Scoring**:
  - Lowered minimum player requirement threshold (`UWNbPlayers`) to 1 in `worldserver.cpp:390` so Union Wars launch cleanly.
  - Hooked Clan Field kills in `battle.cpp:UWKill` to award +5 Clan Contribution Points (CP) to the killer.

---

### Milestone 6 — Autonomous Vending Bots System (Completed & Verified)
- **18 Permanent Autonomous Market Bots**: Positioned in high-traffic commercial corridors across Junon Polis (Map 2) and Canyon City of Zant (Map 1).
- **18 Specialized Stock Catalogs (`VendingCatalog.cpp`)**: Covering Weapons (+4 to +7 refines), Class Armor Sets & Shields, Cut Jewels (T1–T7), Refining Catalysts (Grade 4–10 Talismans/Bindrunes, Planetary Runes, Arua Fate), Crafting Drops & Spirit Stones, Consumables & Town Return Scrolls, Heavy Ammunition, Attribute Jewelry, Wings, and PAT Vehicle Parts.
- **Wire-Level Shop Simulation**: Overhead shop banners streamed in `CreateSpawnPacket`, shop sitting stance (`0x02`), catalog browsing via `0x7c4` (`pakShowShop`), and direct item purchases via `0x7c5` (`pakBuyShop`).
- **Infinite Auto-Restocking & Database Safety**: Auto-replenishment logic in `worldpackets.cpp` keeps stacks permanently at 999 and equipment at 1. Direct bypass in `CPlayer::SaveSlot41` (`if ( is_bot ) return;`) prevents bot virtual items from querying or modifying MySQL tables.
- **Promotional Overhead Shouts**: Periodic advertising messages broadcast across the network via `CPlayerBot::Say()` (`0x783`) every 60–90 seconds.

---

### Milestone 7 — Dynamic Dungeon Instancing & Mail System (Completed & Verified)
- **Dynamic `CMap` Instancing (`CDungeonManager.cpp`)**: Provides private, party-isolated instances (slots 200–999) for cave and temple dungeons across the world:
  - **Goblin Cave**: Maps 24 (B1), 25, 26 (B2), 27, 28 (B3).
  - **George's Cave**: Maps 31 (B1), 32 (B2), 33 (B3).
  - **Oblivion Temple / Sea of Seeds**: Maps 41, 42, 43, 44.
  - **Forgotten Temple**: Maps 56, 57.
- **Client Protocol Compatibility**: The server streams `(base_zone != 0 ? base_zone : id)` in wire teleport packets (`0x7a8`), ensuring client stability while isolating combat, visibility, and monster spawns server-side.
- **Isolated Spawns & Respawns**: Monster spawn areas (`CSpawnArea`) are deep-copied from base IFO definitions upon instance creation. Each party fights their own private mobs with independent timers and drop ownership.
- **Dungeon Timers & Warning Broadcasts**: 30-minute countdown timers per instance with automated warning broadcasts at 10m, 5m, and 1m. When the timer hits 0m, players are safely evacuated to the cave entrance.
- **Party Wipe Detection & Auto-Evacuation**: When all players inside an instance fall in combat, `CDungeonManager::HandlePartyWipe` triggers an automated failure sequence: revives players with 20% HP/MP without death EXP penalties, purges debuffs, teleports them back to the entrance portal, and tears down the failed instance.
- **Dungeon Commands**: `/dungeon time`, `/dungeon info`, `/dungeon leave`, `/dungeon wipe`.
- **Persistent Mail & Parcel Delivery (`charpackets.cpp`, `gmcmds.cpp`)**:
  - Schema extension: Added `is_read`, `zuly`, `item_head`, `item_data`, `item_name`, and `is_claimed` columns to `mail_list`.
  - Non-destructive reading: opening the client memo UI displays attached Zulies and items without deleting messages.
  - Per-message deletion via action `0x04`.
  - Complete command suite: `/mail list`, `/mail read <id>`, `/mail claim <id>`, `/mail send <player> <zuly> [inv_slot] <message>`, `/mail delete <id>`.
- **Monster AIP Scripting Engine (`AiConditions.cpp`, `AiActions.cpp`)**:
  - Evaluates Charm (`Attr->Cha`), `WorldVar[256]`, and `EconomyVar[256]` arrays.
  - Implemented `AICOND(015)`, `AICOND(016)`, `AICOND(023)` (daylight check), `AICOND(026)` (channel range check).
  - Implemented `AIACT(026)`, `AIACT(027)`, `AIACT(032)` (Set Zone PvP mode and broadcast packet `0x70f`), `AIACT(033)` (Set Zone Regen rate).

---

### Milestone 8 — Combat Mechanics Polish, Spawn Physics & Vanguard AI (Completed & Verified)
- **Spawn "Falling from the Sky" Glitch Resolution**:
  - Fixed coordinate validation in `player.cpp:202` (`SpawnToPlayer` packet `0x793`). Stationary avatars and bots now emit matching origin/destination coordinates with `CMD_STOP` (`0x0000`), instructing the ROSE client physics engine to place models firmly on the terrain heightmap rather than dropping them from mid-air.
  - In `charfunctions.cpp:481` (`CCharacter::stopMoving()`) and `PlayerBot.cpp` (`CPlayerBot::StopMoving()`), replaced legacy `0x79a` (`GSV_MOVE`) packets with official `0x770` (`GSV_STOP`), including precise terrain altitude (`z * 100`).
  - Newly instantiated bots start in `WALKING` stance instead of `RUNNING` to eliminate initial visual bounce.
- **Combat Shield Block & Defense Scaling Fixes**:
  - In `battle.cpp:166` (`NormalAttack`), corrected inverted shield block calculation where the attacker's block rate was tested and subtracted from outgoing damage. The formula now checks the defender's `Enemy->Stats->Block_Rate` and mitigates incoming physical damage using `Enemy->Stats->Blocked_dmg`.
  - In `battle.cpp:241`, verified magic defense scaling to properly evaluate `atkdefmult = (float)Stats->Attack_Power / EnemyMDef;`.
- **Dynamic Party Bot Formations (`GetFormationOffset`)**:
  - Party bots follow leaders using dynamic 2D trigonometric rotation relative to the leader's current heading angle, completely eliminating coordinate stacking and clumping:
    - **Slot 0 & 1 (Vanguard Frontline)**: Soldier/Knight bots maintain forward flank positions (2.6m forward, 1.2m left/right) to intercept aggressive monsters.
    - **Slot 2 & 3 (Ranged Flanks)**: Hawker/Scout/Dealer bots flank rearward (1.8m back, 3.2m left/right).
    - **Slot 4 & 5 (Rear Support)**: Muse/Cleric bots trail in the backline (3.8m directly behind).
- **Autonomous Support, Healing & Resurrection**:
  - **Party Buffing (`CheckPartyBuffs`)**: Muse/Cleric bots automatically scan party members within 22m and maintain essential charms with friendly in-game shouts.
  - **Party Resurrection (`CheckPartyResurrect`)**: Cleric bots (Level $\ge 30$) detect dead party members within 20m, revive them with 35% HP and 25% MP, refresh active buffs, dispatch `0x79f` packets, and restore bot state to `BOT_STATE_FOLLOW`.
  - **Vanguard Aggro Intercept & Taunt (`CheckPartyTaunt`)**: Soldier/Knight bots identify monsters targeting squishy party members, intercept the mob, taunt them, and maintain threat.
  - **Courteous Looting Etiquette**: Party bots only loot items directly under their feet ($<3$m), reserving dropped gear and gold for the party leader and immediately snapping back into formation.
- **Player Bot Party Commands**:
  - `/party bot buff`: Forces all Muse/Cleric party bots to refresh full buffs immediately.
  - `/party bot follow`: Commands all party bots to drop current engagements and return to formation.
  - `/party bot assist`: Orders party bots to focus-fire on the leader's current combat target.
  - `/party bot info`: Displays active formation slots, bot roles, HP, and MP stats.
- **Guild Wars 2 Style Open-World Auto-Sharing**:
  - Implemented in `battle.cpp` (`GiveExp`, `DropMonsterItem`, and `CPlayer::CheckQuest`):
    - **Universal Full EXP Sharing**: Every contributor who damaged a monster receives full monster EXP multiplied by a 15% co-op team bonus.
    - **Non-Locked Shared Drops**: Loot permissions are granted to all participants who dealt damage.
    - **Universal Quest Kill Credit**: Every attacking player and bot receives full kill credit towards active quests.

---

### Milestone 9 — Intelligent AI Companion & Party Bot System (Completed & Verified)
- **Instant World Auto-Grouping**: Real players entering the game world (`pakDoID`) are automatically grouped into a balanced party with intelligent AI companion bots.
- **Smart Spec & Role Composition**: Automatically evaluates the real player's class (Tank, Healer, DPS, or Visitor/Novice) and populates the squad with complementary roles. When a player advances their class, the party dynamically rebalances.
- **Bi-Directional Level Synchronization**: Bots scale their level, base attributes, tiered weapons, armor, and shields to match the leader's level upon joining, during active following, and whenever the player levels up (`CheckPlayerLevelUP`).
- **Strict Dynamic Job Tier Enforcement**:
  - **Level 1–9**: Scaled strictly to **Visitor (`Job 0`)** with zero class abilities.
  - **Level 10–69**: Promoted to **1st Job** (`111` Soldier, `211` Muse, `311` Hawker).
  - **Level 70+**: Advanced to **2nd Job** (`121` Knight / `122` Champion, `221` Mage / `222` Cleric, `322` Scout).
- **Skill Slot Memory Wipe & Authentic Skill Verification**:
  - On every level sync or change, all 362 skill slots in `m_player->cskills` are zeroed out to eliminate leftover high-level skills.
  - New skill learning strictly follows authentic ROSE level requirements (Level 1–9 Visitors only learn novice Dual Scratch `101`; buffs require Muse Level 20+).
  - All combat skills, heals, taunts, and buffs verify learned presence via `GetLearnedSkill` and `HasSkill`.
- **Combat Strategies & Rotations**: Specialized rotations for Tank (aggro peel/taunt), Healer (prioritized triage healing & buff maintenance), and DPS (focused ranged/melee nukes).
- **Flocking Formation & Distance Tethering**: Dynamic 2D trigonometric positioning relative to the leader's heading, with separation jitter preventing clumping and instant teleportation back to the leader if separated by $> 25\text{--}32\text{m}$.
- **Session Persistence & Relog Refilling**: Leaving the party or relogging automatically restores and refills the player's companion squad back to full capacity.
- **GM Named Teleport Suite (`/tele <name>` & `/go <name>`)**: Fast instant travel to 35+ named landmarks, towns, and dungeons.

---

## Roadmap & Upcoming Milestones

### Milestone 10 — Advanced World Events, Raids & Planetary Expansion (In Design)
- [ ] **Planet Luna & Orlo Expansion Polish**: Review and verify space flight transitions, low-gravity physics, and unique monster spawn definitions.
- [ ] **World Boss Multi-Phase Mechanics**: Implement multi-phase enrage timers, area-of-effect telegraphs, and tiered reward chests for world bosses (*King Spider*, *Chapeau*, *Karkome*).
- [ ] **Seasonal Event Automation**: Automated in-game holidays (Halloween pumpkin drops, Christmas gift boxes, Summer festival events).
- [ ] **Server Security & Integrity Auditing**: Automated packet validation and speed/teleport anomaly detection.

---

## Intelligent AI Companion & PlayerBot Deep-Dive

RosE Resurrected features an industry-leading, fully autonomous PlayerBot and AI Companion squad system designed to make the world feel populated, vibrant, and alive while providing complete solo-player gameplay support.

```
                    +------------------------------------+
                    |        CPlayerBot Controller       |
                    +-----------------+------------------+
                                      |
         +----------------------------+----------------------------+
         |                            |                            |
         v                            v                            v
+------------------+         +------------------+         +------------------+
| AI Companion Bot |         | Grind-Spot Cleric|         | Ambient Grinders |
| - Real Auto-Group|         | - 7-Charm Aura   |         | - 1200m Sensor   |
| - Bi-Dir Leveling|         | - 3x Regen Fire  |         | - PAT Mounts     |
| - Strict Job Tier|         | - Real Player Prio|        | - D&D Dice Engine|
| - Formation Flocks|        | - Bonfire Health |         | - Anti-KS Logic  |
+--------+---------+         +--------+---------+         +--------+---------+
         |                            |                            |
         +----------------------------+----------------------------+
                                      |
         +----------------------------+----------------------------+
         |                                                         |
         v                                                         v
+----------------------------------+     +----------------------------------+
|   Social Engine & Economy        |     |   PvP & Dungeon Raiding Engine   |
|   - Real-Player /wave Greetings  |     |   - Open-World /duel System      |
|   - 18 Autonomous Market Vendors |     |   - Akram Arena /arena Queueing  |
|   - Direct /trade & Whispers     |     |   - Dynamic Dungeon Crawling     |
|   - Clan Tags & Earned Titles    |     |   - World Boss Raid Groups       |
+----------------------------------+     +----------------------------------+
```

### 1. Real Player Auto-Grouping
Whenever a real human player enters the game world (`pakDoID`), the server's bot manager immediately verifies whether the player is in a party. If not, the server forms a new party, designates the player as party leader, and assigns an intelligent squad of AI companion bots.

### 2. Dynamic Spec Detection & Party Composition
The bot manager dynamically inspects the player's job class and level:
- **Player is a Tank (Soldier/Knight/Champion)**: Squad spawns 1 Healer/Buffer (Muse/Cleric) + 2 DPS bots (1 Magic Mage + 1 Physical Ranged Scout/Hawker).
- **Player is a Healer (Muse/Cleric)**: Squad spawns 1 Vanguard Tank (Soldier/Knight) + 2 DPS bots.
- **Player is a DPS (Mage/Hawker/Scout/Dealer)**: Squad spawns 1 Vanguard Tank + 1 Dedicated Healer/Buffer + 1 complementary DPS bot.
- **Player is a Visitor (Novice, Level 1–9)**: Squad spawns a balanced squad of fellow Novice companions with role-tailored weapons and armor. When the player chooses a job path at Level 10, companion bots adjust their jobs to maintain party balance.

### 3. Bi-Directional Level Synchronization
Companion bots dynamically scale their level up or down to match the player:
- **On Joining the Party**: Bots instantly set their level, attributes, and gear to the leader's level.
- **On Player Level-Up (`CheckPlayerLevelUP`)**: Bots immediately level up in sync with the leader, play level-up visual effects, and upgrade their gear.
- **On Player Level-Down (Testing or Relogging)**: Bots scale down immediately, recalculating base attributes and equipment tiers.

### 4. Dynamic Job Tier Progression & Down-Scaling
To maintain authentic game mechanics, companion bots strictly reflect job tier restrictions:
- **Level 1–9**: Reverted strictly to **Visitor (`Job 0`)** with novice wooden weapons, basic clothing, and zero class spells.
- **Level 10–69**: Promoted to **1st Job** (`111` Soldier, `211` Muse, `311` Hawker).
- **Level 70+**: Advanced to **2nd Job** (`121` Knight / `122` Champion, `221` Mage / `222` Cleric, `322` Scout).

### 5. Skill Memory Wipe & Authentic Skill Verification
A common issue in emulators is bots retaining high-level skills after scaling down. RosE Resurrected implements comprehensive skill safety:
- **Full Skill Memory Wipe**: Every level synchronization zeroes out all 362 skill slots (`m_player->cskills`) before populating eligible skills.
- **Level-Gated Skill Assignment**:
  - *Visitors (Level 1–9)*: Only possess Dual Scratch (`101`). Zero buffs, zero healing spells.
  - *Muses (Level 10+)*: Unlock Cure (`921`) and Mana Bolt (`851`).
  - *Muses (Level 20+)*: Unlock Hustle Charm (`926`) and Ice Bolt (`821`).
  - *Muses / Clerics (Level 35+)*: Unlock Recovery (`981`), Clobber Charm (`1270`), and Mana Spear (`861`).
  - *Clerics (Level 50+)*: Unlock Party Heal (`1031`), Resilience Charm (`1004`), Battle Charm (`1254`), Precision Charm (`1019`), and Resurrection (`1051`).
- **Verification Before Casting**: Methods `GetLearnedSkill()` and `HasSkill()` verify that a skill exists in the bot's learned skill list with a valid level before casting.

### 6. Spec-Specific Combat Strategies & Multi-Step Synergy Combos
- **Tank (Soldier / Knight / Champion)**:
  - Continuously scans for monsters attacking the party leader or squishy allies (`CheckPartyTaunt`), immediately locking monster target and threat.
  - **3-Step Tank Combo**:
    - *Step 0 (Opener & Taunt)*: Piercing Lunge (`336`) or Double Attack (`261`) with threat shout (*"Piercing Lunge! Eyes on me, monster!"*).
    - *Step 1 (Armor Break)*: Fatal Thrust (`291`) to pierce and lower enemy defense.
    - *Step 2 (Finisher & Aggro Lock)*: Voltage Crash (`351`) ground slam.
- **Healer / Buffer (Muse / Cleric)**:
  - Strict triage healing loop: Leader (HP < 75%) $\rightarrow$ Tank (HP < 65%) $\rightarrow$ Self (HP < 60%) $\rightarrow$ Other Party Members.
  - Maintains 5 core charms (Hustle, Clobber, Resilience, Battle, Precision) when out of combat or during lulls.
  - Revives fallen party members within 20m (`CheckPartyResurrect`), granting 35% HP / 25% MP.
  - When party HP is safe, assists with Ice Bolt (`821`) to slow enemies by 40% and Mana Spear (`861`).
- **Magic DPS (Muse / Mage)**:
  - **3-Step Spell Combo**:
    - *Step 0 (Crowd Control)*: Ice Bolt (`821`) chilling and slowing enemy movement speed by 40% (*"Ice Bolt! Chilling and slowing target!"*).
    - *Step 1 (Arcane Burst)*: Mana Spear (`861`) dealing heavy burst damage.
    - *Step 2 (Overload Finisher)*: Voltage Jolt (`871`) lightning finisher.
- **Ranged DPS (Hawker / Scout)**:
  - **3-Step Archer Combo**:
    - *Step 0 (Crippler / Pin)*: Clamp Arrow (`1431`) pinning enemy movement (*"Clamp Arrow! You can't run!"*).
    - *Step 1 (Volley Burst)*: Triple Arrow (`1486`) / Double Arrow (`1441`).
    - *Step 2 (CC Finisher)*: Stun Arrow (`1501`) stunning the target (*"Stun Arrow! Hold still, beast!"*).
- **Dealer / Bourgeois**:
  - Gun & Artillery rotations: Twin Shot opener $\rightarrow$ Sniping / Triple Shot $\rightarrow$ Demolition Expertise finisher.
- **Novice / Visitor (Level 1–9)**:
  - Basic melee novice rotations using Dual Scratch (`101`) with enthusiastic beginner battle shouts.
- **In-Combat Potion Consumption**:
  - When HP drops below 35%, bots consume HP potions (`0x7a3` effect, restoring 35% MaxHP) and call out (*"Using an HP Potion to stay in the fight!"*).
  - When MP drops below 40%, bots consume MP potions (restoring 40% MaxMP).

### 7. Quest-Aware Target Prioritization & Callouts
Companion bots actively scan and assist with the leader's quest journal:
- **Quest Objective Scanner**: Inspects `m_followTarget->quest.quests[0..9]` to cross-reference nearby monsters with active `die_quest` triggers.
- **Spotted Target Callouts**: Scans every 25 seconds and notifies the leader when quest mobs are within 25 meters (*"Spotted a quest target nearby! Ready when you are, [Leader]!"*).
- **Proactive Pulling (Hunter Mode)**: In Aggressive stance, bots pull quest targets directly to the leader's formation.
- **Quest Completion Celebrations**: When a quest objective is slain, bots celebrate in chat (*"Quest target down! Great teamwork, [Leader]!"*).

### 8. Synchronized Camp Resting
Companion bots loyally mirror the leader's stance:
- When the leader sits down (`/sit` or stance 2), companion bots automatically take a seat in a perimeter circle around the leader to regenerate HP and MP together.
- Companions engage in friendly camp banter (*"Resting up with you, leader."*, *"Good fight! Taking a quick breather by the camp."*).
- When the leader stands up, all bots instantly stand and resume formation.

### 9. Tactical Party Stances & Commands (`/party bot <mode>`)
Players have complete command over their squad's tactical behavior:
- `/party bot aggressive` (or `/party bot hunt`): **Hunter Stance** — Bots proactively pull quest targets and nearby level-appropriate monsters within 22m back to the leader to grind efficiently.
- `/party bot defensive` (or `/party bot guard`): **Bodyguard Stance (Default)** — Bots maintain formation and defend the leader and party members from aggressive attackers.
- `/party bot passive` (or `/party bot hold`): **Pacifist Stance** — Bots hold fire, cease all attacks, and strictly follow the leader in formation (ideal for dodging aggro in dungeon corridors).
- `/party bot assist` (or `/party bot focus`): **Focus Fire Stance** — Immediately focuses all squad members' attacks onto the monster currently targeted by the player.
- `/party bot buff`: Forces all Muse/Cleric party bots to refresh party charms immediately.
- `/party bot follow`: Breaks combat engagements and rallies all bots into formation behind the player.
- `/party bot info`: Displays active squad member names, levels, jobs, HP/MP percentages, stances, and states.

### 7. Dynamic 2D Trigonometric Formations & Distance Tethering
Party bots maintain realistic tactical formations using 2D trigonometric rotation relative to the leader's heading vector:
- **Frontline Vanguards (Slots 0 & 1)**: 2.6m ahead, 1.2m left/right.
- **Ranged Flankers (Slots 2 & 3)**: 1.8m back, 3.2m left/right.
- **Rearguard Healers (Slots 4 & 5)**: 3.8m directly behind.
- **Separation & Flocking**: Repulsion vectors prevent bots from stacking on top of each other or the player.
- **Distance Tethering**: If the leader moves $> 25\text{--}32\text{m}$ away, changes zones, or teleports, companion bots instantly despawn and respawn alongside the leader.

### 8. Session Persistence & Relog Refilling
If a player leaves the party, kicks companion bots, or logs out and reconnects, `CBotManager::RefillPartyCompanions` automatically restores missing squad slots up to party capacity.

### 9. Guild Wars 2 Style Open-World Auto-Sharing
Eliminates kill-stealing frustration by encouraging cooperative open-world play:
- **Universal Full EXP Sharing**: Every contributor who damaged a monster receives 100% monster EXP plus a 15% cooperative bonus.
- **Shared Drops**: Monsters drop loot that can be picked up by any contributor.
- **Universal Quest Credit**: Every participant receives full kill credit towards active quests.

### 10. Ambient Grinding Bots & D&D Dice Engine
Over 100 ambient bots roam the world living authentic player lives:
- **1,200m Map-Wide Sensory Radius**: Bots roam and hunt across entire zones, migrating to monster territories when cities are peaceful.
- **D&D Probabilistic Dice Rolls ($d20$ / $d100$)**:
  - Proximity Greetings: $d20 \ge 15$ (~30% chance).
  - Stranger Buffing: $d20 \ge 14$ (~35% chance for Muses/Clerics).
  - Combat Cheers: $d20 \ge 12$ (~45% chance).
  - Town Repair Trips: $d100 \ge 85$ (~15% chance per hunt cycle).
- **Anti-Clustering & Target Reservation (`IsMobReserved`)**: Eliminates bot trains by preventing multiple bots from converging on the same monster.
- **PAT Vehicles**: Bots level 30+ mount Carts, and level 70+ mount Castle Gear robots for long-distance travel.
- **Resting & Recovery**: Bots sit (`Status->Stance = SITTING`) when HP < 40% or MP < 30% to regenerate.

### 11. Worldwide Grind-Spot Buff Bots (The Cleric Sisters)
Stationed at prime leveling areas across Planet Junon:

| Sister | Location | Zone / Region |
| :--- | :--- | :--- |
| **SisterAngela** | Adventurer Plains | Starter Jellybean & Chorpy fields |
| **SisterMaria** | Valley of Luxen | Goblin & Beetle grinding grounds |
| **SisterTeresa** | Breezy Hills | Pomic & Honey Bee areas |
| **SisterClaire** | El Vergel | Desert training grounds |
| **SisterRose** | Anima Lake | Mid-level aquatic & beast spots |
| **SisterGrace** | Forest of Wisdom | Ghost & Elder monster zones |
| **SisterFaith** | Kenji Beach | High-level beach combat grounds |

- **Large Bonfire (ID 806)**: Emits a 25-meter aura granting 3x HP and MP natural regeneration. Bots maintain and renew bonfires automatically.
- **7-Charm Blessing Rotation**: Pure, Mental, Dash/Haste, Might, Defense, Magic Defense, and Accuracy charms. Real human players are placed at the front of the queue.

### 12. Autonomous Market Vending Bots (Junon & Zant)
18 permanent vendors populate commercial avenues in Junon Polis and Canyon City of Zant:
- Full stock of weapons (+4 to +7), class armor sets, T1–T7 cut gems, refining catalysts (Talismans, Planetary Runes, Arua Fate), crafting materials, ammunition, and PAT parts.
- Infinite auto-restocking keeps stacks at 999 and equipment at 1.
- Database queries are bypassed (`SaveSlot41` skips bot items), ensuring zero database clutter.

### 13. Social Engine, Whispers, Duels, Trading & GM Teleport Suite
- **Interactive Whispers (`/w <BotName>`)**: Bots respond intelligently to player whispers with their coordinates, party invitations, or duel challenges.
- **Open-World Duels (`/duel <BotName>`)**: Players can challenge any bot to a duel. Bots yield at 10% HP with a polite bow.
- **Direct Trading (`/trade <BotName>`)**: Bots accept trades, placing 25,000 Zulies into the trade window.
- **Party Bot Chat Commands**:
  - `/party bot buff`: Forces healers to buff the party immediately.
  - `/party bot follow`: Orders all bots to cease combat and follow.
  - `/party bot assist`: Focuses fire on the leader's target.
  - `/party bot info`: Displays bot health, mana, and roles.
- **GM Fast Teleport Suite (`/tele <name>` & `/go <name>`)**:
  - *Cities & Fields*: `/tele zant`, `/tele adventure`, `/tele junonpolis` (`/go junon`), `/tele eucar`, `/tele xitasanctuary` (`/tele xita`), `/tele luxem`, `/tele breezy`, `/tele elvergel`, `/tele animalake`, `/tele kenji`, `/tele forestofwisdom`, `/tele desertofthedead`.
  - *Dungeons & Arenas*: `/tele goblincaveb1` through `b3`, `/tele georgecaveb1` through `b2`, `/tele forgottentempleb1` through `b2`, `/tele clanfield`, `/tele teamdeathmatcharena`.

---

## Dynamic Dungeon Instancing Engine

The `CDungeonManager` handles dynamic `CMap` instances (slots 200–999) for all caves and temples:
- **Private Party Instances**: Parties enter private copies of Goblin Cave, George's Cave, Oblivion Temple, and Forgotten Temple.
- **Wire Compatibility**: Wire teleport packets translate instance indices to valid client STB map IDs (0–121) to prevent client crashes.
- **Countdown Timers & Evacuation**: 30-minute timers with broadcasts at 10m, 5m, and 1m.
- **Party Wipe Recovery**: If all party members fall in combat, the instance triggers auto-evacuation: revives players with 20% HP/MP at the entrance with zero death EXP penalties.

---

## Mail & Parcel Delivery Engine

The modernized mail system provides non-destructive messaging and item parcel transfers:
- **Schema**: Columns `is_read`, `zuly`, `item_head`, `item_data`, `item_name`, and `is_claimed` in `mail_list`.
- **Commands**:
  - `/mail list`: Lists last 10 messages with attachment status.
  - `/mail read <id>`: Displays message contents and attachments.
  - `/mail claim <id>`: Transfers Zulies and items into inventory and marks claimed.
  - `/mail send <player> <zuly> [inv_slot] <message>`: Sends mail with attachments.
  - `/mail delete <id>`: Deletes specific mail.

---

## Compilation Guide

### Linux Build (Ubuntu / Debian / Mint / WSL2)

#### 1. Install Build Dependencies
```bash
sudo apt update
sudo apt install -y build-essential cmake git libmariadb-dev libmariadb-dev-compat
```

#### 2. Build via the Automation Script
From the repository root:
```bash
chmod +x build.sh
./build.sh
```

#### 3. Manual CMake Build (Alternative)
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

Binaries (`loginserver`, `charserver`, `worldserver`) are compiled directly into `VS_Projects/Server.Run.2/`.

---

### Windows Build (Visual Studio 2017–2022)

1. Open `VS_Projects/servers.sln` in Visual Studio.
2. Select **Release** configuration and **Win32** (or **x86**) platform.
3. Build the solution (**Build -> Build Solution** or `Ctrl+Shift+B`).
4. Output binaries are generated in `VS_Projects/Server.Run.2/`.

---

## Database Setup & Server Configuration

### 1. Database Setup
Ensure MariaDB is running:
```bash
sudo systemctl start mariadb
```

Import or verify the database using:
```bash
cd Database
chmod +x import_db.sh
./import_db.sh roseza root localhost
```

### 2. Server Configuration Files
Configuration files are located in `VS_Projects/Server.Run.2/`:
- `loginserver.conf` (Port 29000)
- `charserver.conf` (Port 29100)
- `worldserver.conf` (Port 29200, `maxconnections=500`)

---

## Starting & Managing the Servers

Scripts are located in `VS_Projects/Server.Run.2/`:

### Start All Servers
```bash
cd VS_Projects/Server.Run.2
./start_servers.sh
```

### Stop All Servers
```bash
cd VS_Projects/Server.Run.2
./stop_servers.sh
```

### Viewing Logs
```bash
tail -f log/worldserver.log
tail -f log/charserver.log
tail -f log/loginserver.log
```

---

## Maintenance & Git Synchronization Workflow

Whenever performing work that modifies server code, database schemas, configs, or documentation, always use the automated synchronization script:

```bash
cd /media/jacque/4a179792-f2d6-434b-b81c-5b9536015214/Games/R.O.S.E/osrose-evo-server
./git_sync.sh "Concise description of changes"
```

The script automatically stages all changes, creates a signed commit, and pushes to `origin/main`. Compiled binaries, logs, and build artifacts are excluded by `.gitignore`.

---

## Credits & Licensing

- **Original Project**: OSRose Development Team ([dev-osrose.com](http://www.dev-osrose.com))
- **Evolution Enhancements**: Eich, Lmame, Tomiz, PY, Terr0risT, and community contributors.
- **RosE Resurrected**: Modern refactoring, stability patches, clan vaults, dynamic dungeon instancing, intelligent AI companions, and autonomous bot AI.
- **License**: GNU General Public License v2 (GPL-2.0). See [LICENSE](LICENSE) for details.
