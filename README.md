# RosE Resurrected — Server Emulator

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-14-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.12%2B-orange.svg)]()
[![Database](https://img.shields.io/badge/Database-MariaDB%20%2F%20MySQL-lightgrey.svg)]()
[![License](https://img.shields.io/badge/License-GPL%20v2-green.svg)]()

**RosE Resurrected** is a high-performance, modular C++ server emulator for **ROSE Online (naROSE Evolution v132–v142)**. Built upon the proven osROSE Evolution architecture, this emulator introduces modern enhancements, strict server-side exploitation defenses, fully autonomous player and cleric grind-spot buff bots, complete quest triggers, clan storage vaults, and scalable multi-platform compilation support.

---

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Current Status & Completed Milestones](#current-status--completed-milestones)
3. [Roadmap & Pending Milestones](#roadmap--pending-milestones)
4. [Compilation Guide](#compilation-guide)
   - [Linux Build (Ubuntu / Debian / Mint / WSL2)](#linux-build-ubuntu--debian--mint--wsl2)
   - [Windows Build (Visual Studio 2017–2022)](#windows-build-visual-studio-20172022)
5. [Database Setup & Configuration](#database-setup--configuration)
6. [Starting & Managing the Servers](#starting--managing-the-servers)
7. [PlayerBot & Grind-Spot Buff Bot AI Deep-Dive](#playerbot--grind-spot-buff-bot-ai-deep-dive)
8. [Maintenance & Git Synchronization Workflow](#maintenance--git-synchronization-workflow)
9. [Credits & Licensing](#credits--licensing)

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
                             | (rose_resurrected)|
                             +-------------------+
```

- **Login Server (Port 29000)**: Handles account authentication, password hashing, IP bans, capacity gating, and dispatching the active channel list.
- **Character Server (Port 29100)**: Manages character selection, creation, deletion, clan metadata, Alt+N clan skill blocks, friend lists, and cross-channel whispers.
- **World Server (Port 29200)**: Orchestrates zone maps, combat damage formulas, AIP creature behavior, player inventory/storage, party sync, quests, NPC dialogs, and autonomous PlayerBots.
- **Assets (`3DData/`)**: Server reads game definition files (`STB` tables, `STL` localized strings, `AIP` state scripts, `QSD` quest flows, and `IFO` object placement).

---

## Current Status & Completed Milestones

### Core Systems (90%+ Complete)
- **Networking & Cryptography (100%)**: TriggerSoft table encryption (`0x703` / `0x7ff`), session token validation, and SQL-injection-safe queries.
- **Character Core (95%)**: Full level progression (1–255), stat point allocation (STR, DEX, INT, CON, CHA, SEN), stamina regeneration, stance synchronization.
- **Combat Mechanics (90%)**: Normal attack cycles, physical/magical defenses, accuracy vs. dodge, critical hit rolls, resurrection, and death penalties.
- **Economy & Crafting (90%)**: 8-byte Evolution item structures, refine system up to +15 with GradeList multipliers, gem socketing, item appraisal, player trade, and personal vending shops.
- **Social Systems (90%)**: Parties with shared EXP and drop modes, friend lists, chatrooms with disconnect broadcast (`0x7e3:0x16`), and whisper/shout channels.

### Milestone 1 — Stability & Security (Completed & Verified)
- **PvP Town Safety Guard**: Enforced server-side checks in `pakStartAttack` and `StartAction` to prevent attacks in towns (`map->allowpvp == 0`) and block friendly-fire within teams (`pvp_id`).
- **Server Capacity Limit**: Implemented `Config.MaxConnections` enforcement in `SocketServer.cpp` and `worldserver.cpp` to reject incoming connections gracefully when full.
- **Valkyrie Charm INT Scaling**: Restored INT scaling in `Buff.cpp` for Add Damage (`A_ADDDMG`, buff 83).
- **Driving Skill Restrictions**: Permitted driving-specific skills while inside Cart/Castle Gear while rejecting on-foot skills.

### Milestone 2 — Quest Engine & Timers (Completed & Verified)
- **Active Timed Quest Expiration**: 1-second interval ticking loop checking quest time limits; automatically marks expired quests and dispatches client failure notifications.
- **Delayed Trigger Queue (`QUESTREWD(013)`)**: Player-bound delayed trigger execution queue processed each tick in `MainProcess.cpp`.
- **IFO Interactive Objects (`QUESTCOND(012)`)**: Evaluates WarpGates, levers, and NPC event IDs.
- **Time & Channel Conditions**: Implemented `QUESTCOND(016)` (zone time), `QUESTCOND(017)` (object variables), `QUESTCOND(022)` (server channel checks), and `QUESTREWD(012)` (LTB NPC shout/announcements).

### Milestone 3 — PAT (Cart & Castle Gear) & Economy (Completed & Verified)
- **Complete Cart Weapon Formulas**: Implemented unique formulas in `playerstats.cpp` for all 5 Cart weapon subclasses (Hammer, Mirror/Crystal, Bow, Katar/Spear, Launcher/Gun), with stat scaling and grade bonuses.
- **Cart Accuracy Calibration**: Eliminated accuracy warning logs with consistent offsets for all Cart weapon IDs (including legacy ID 551).
- **Equipment Repair Pricing**: Formula based on base item price and lost condition:
  $$\text{repair\_cost} = \lfloor \text{bprice} \times \frac{99 - \text{lifespan}}{100} \times 0.05 \rfloor$$
- **Storage Transaction Fees**: Configurable deposit fee deductions while keeping withdrawals free.

### Milestone 4 — Clan System Expansion (Completed & Verified)
- **Clan Skills Wire Protocol**: Added `skills VARCHAR(500)` column to `list_clan`. Packet `0x7e0:0x33` (`SendClanInfo`) transmits the full 120-byte skills block (60 `WORD`s) to populate the client Alt+N Clan window.
- **Clan Passive Calculation**: Unified `CPlayer::ApplyClanPassive()` applied across core attributes (STR, DEX, INT, CON, CHA, SEN) and combat stats (ATK, DEF, MDEF, ACC, DODGE, CRIT, HASTE, MOVE SPEED, MAX HP, MAX MP).
- **Clan Storage Shared Vault**: Table `clan_storage` (`clanid`, `slotnum` PK) supporting multi-slot item persistence. Clan vault opening (`0x7ad:0x10/0x20`) and deposit/withdrawal (`0x7af:0x10/0x11`) with Master/Deputy rank checks ($\ge 5$).
- **Clan Bank & Funds**: Added `money BIGINT UNSIGNED` to `list_clan` with 8-byte QWORD serialization and Zuly deposit/withdrawal (`0x7b1:0x20/0x21`).
- **Clan Quests**: Implemented `QUESTCOND(025)` (contribution check), `QUESTCOND(028)` (clan funds check), `QUESTCOND(030)` (clan skill learned check), `QUESTREWD(024)` (funds adjustment), and `QUESTREWD(026)` (clan skill learning with real-time online member stat recalculation).

### Milestone 5 — PvP Arena, Union Wars & Clan Wars (Completed & Verified)
- **Multi-Mode PvP Zone Engine**: Parsed `LIST_ZONE.STB` columns 18 (`allowpvp`) and 20 (`zone_type`) into 5 distinct PvP modes (`ePvpMode`):
  - **Mode 0 (Safe / PvE)**: Safe towns, normal adventure maps, Agits (Zones 15–18, 40). Zero player-versus-player combat permitted.
  - **Mode 1 (Free-for-All / FFA)**: Training Grounds (Map 6), Desert of the Dead (Map 29), Temple Campgrounds (Map 46), Forgotten Temple (56–57), Sikuku Ruins (66), Gates of Muris (82). Players can attack anyone outside their own party.
  - **Mode 2 (Team / Arena)**: Akram Arena / Colosseum (Map 9). Players are assigned to Red or Blue teams and can only attack the opposing team.
  - **Mode 3 (Clan Fields)**: Lion's Plains (Map 8), Junon Clan Fields (11–13), Luna Clan Field (59), Pedion (101–120). Clanmates share team IDs and cannot damage each other.
  - **Mode 4 (Union Wars / Cartel)**: Junon Cartel (Map 5) and Union conflict zones. Players in the same Union share team IDs.
- **Unified Combat Safety Engine (`CCharacter::CanAttackCharacter`)**: Enforced comprehensive safety checks across normal auto-attacks, targeted skills (`pakStartAttack`, `pakStartSkill`), AoE attacks (`AoeSkill`), AoE debuffs (`AoeDebuff`), and targeted buffs/heals (cannot heal or buff hostile enemies). Protects Large Bonfires (`montype 806`) and player summons.
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

### Milestone 6 — Autonomous Vending Bots System (Completed & Verified)
- **18 Permanent Autonomous Market Bots**: Positioned in high-traffic hubs across Junon Polis (Map 2) and Canyon City of Zant (Map 1).
- **18 Specialized Stock Catalogs (`VendingCatalog.cpp`)**: Covering Weapons (+4 to +7 refines), Class Armor Sets & Shields, Cut Jewels (T4–T7 Diamonds, Rubies, Sapphires), Refining Catalysts (Grade 4–10 Talismans/Bindrunes, Planetary Runes, Arua Fate), Crafting Drops & Spirit Stones, Consumables & Town Return Scrolls, Heavy Ammunition, Attribute Jewelry, Wings, and PAT Vehicle Parts.
- **Wire-Level Shop Simulation**: Overhead shop banners streamed in `CreateSpawnPacket`, shop sitting stance (`0x02`), catalog browsing via `0x7c4` (`pakShowShop`), and direct item purchases via `0x7c5` (`pakBuyShop`).
- **Infinite Auto-Restocking & Database Safety**: Auto-replenishment logic in `worldpackets.cpp` keeps stacks permanently at 999 and equipment at 1. Direct bypass in `CPlayer::SaveSlot41` (`if ( is_bot ) return;`) prevents bot virtual items from querying or modifying MySQL tables.
- **Promotional Overhead Shouts**: Periodic advertising messages broadcast across the network via `CPlayerBot::Say()` (`0x783`) every 60–90 seconds.

---

## Roadmap & Pending Milestones

### Milestone 7 — Dynamic Dungeon Instancing & Polish (Next Target)
- [ ] **Dynamic `CMap` Instancing**: Clone cave maps (e.g. Goblin Cave, George's Cave) into private party-specific instances with automatic teardown on party exit.
- [ ] **Dungeon Wipe Timers**: Auto-teleport players to respawn towns on full party wipe or instance timeout.
- [ ] **Mail System Attachments**: Support per-message deletion, Zuly attachments, and item parcel transfers.
- [ ] **Advanced AIP Opcodes**: Implement missing AIP checks (CDT 015, CDT 021, CDT 025, ACT 031, ACT 032).

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

The compiled binaries (`loginserver`, `charserver`, `worldserver`) are output directly to `VS_Projects/Server.Run.2/`.

---

### Windows Build (Visual Studio 2017–2022)

#### 1. Prerequisites
- Visual Studio (Community, Professional, or Enterprise) with the **Desktop development with C++** workload installed.
- MySQL Connector/C 6.1 (or MariaDB Connector/C) installed, or use the pre-configured headers in `VS_Projects/3rd_party/`.

#### 2. Building the Solution
1. Open `VS_Projects/servers.sln` in Visual Studio.
2. Set the build configuration to **Release** and platform to **Win32** (or **x86**).
3. Build the entire solution: **Build -> Build Solution** (or press `Ctrl+Shift+B`).
4. Output binaries (`loginserver.exe`, `charserver.exe`, `worldserver.exe`) will be generated in `VS_Projects/Server.Run.2/`.

---

## Database Setup & Configuration

### 1. Database Schema
Ensure MariaDB/MySQL is running on `localhost:3306`:
```bash
sudo systemctl start mariadb
```

Import the database using the automated importer script:
```bash
cd Database
chmod +x import_db.sh
./import_db.sh rose_resurrected root localhost
```

### 2. Server Configuration Files
Configuration files are located in `VS_Projects/Server.Run.2/`:

- `loginserver.conf`:
  ```ini
  [DATABASE]
  mysql_host=localhost
  mysql_user=root
  mysql_pass=sproggins
  mysql_database=rose_resurrected
  mysql_port=3306

  [SERVERDATA]
  serverid=0
  servertype=0
  serverip=127.0.0.1
  serverport=29000
  servername=LoginServer
  ```

- `charserver.conf`:
  ```ini
  [DATABASE]
  mysql_host=localhost
  mysql_user=root
  mysql_pass=sproggins
  mysql_database=rose_resurrected
  mysql_port=3306

  [SERVERDATA]
  serverid=1
  servertype=1
  serverip=127.0.0.1
  serverport=29100
  servername=RosE Resurrected
  ```

- `worldserver.conf`:
  ```ini
  [DATABASE]
  mysql_host=localhost
  mysql_user=root
  mysql_pass=sproggins
  mysql_database=rose_resurrected
  mysql_port=3306

  [SERVERDATA]
  serverid=1
  servertype=2
  serverip=127.0.0.1
  serverport=29200
  servername=RosE Resurrected
  maxconnections=500

  [WORLDSERVER]
  accesslevel=100
  welcome_msg=Welcome to RosE Resurrected.
  ```

---

## Starting & Managing the Servers

Scripts are located in `VS_Projects/Server.Run.2/`:

### Start All Daemons
```bash
cd VS_Projects/Server.Run.2
./start_servers.sh
```
This launches `loginserver`, `charserver`, and `worldserver` in background processes, logs PIDs to `.pid` files, and streams standard output to `log/*_stdout.log`.

### Stop All Daemons
```bash
cd VS_Projects/Server.Run.2
./stop_servers.sh
```

### Viewing Server Logs
```bash
tail -f log/worldserver_stdout.log
tail -f log/charserver_stdout.log
tail -f log/loginserver_stdout.log
```

---

## PlayerBot & Grind-Spot Buff Bot AI Deep-Dive

RosE Resurrected features an advanced autonomous bot system designed to make the world feel populated and provide grinding support.

```
                    +------------------------------------+
                    |        CPlayerBot Controller       |
                    +-----------------+------------------+
                                      |
         +----------------------------+----------------------------+
         |                                                         |
         v                                                         v
+-------------------------------+                       +-------------------------------+
|      Grinding Combat Bots     |                       |   Grind-Spot Cleric Buff Bots |
|  - Autonomous Roaming / Hunt  |                       |  - SisterAngela, Maria, etc.  |
|  - Skill Rotations by Job     |                       |  - Large Bonfire Maintenance  |
|  - HP/MP Resting Mechanics    |                       |  - Smart Real-Player Buffing  |
|  - Anti-Bounce Movement Sync  |                       |  - Full 7 Cleric Charms Aura  |
+-------------------------------+                       +-------------------------------+
```

### 1. Autonomous Grinding Combat Bots (`PlayerBot.cpp`)
- **State Machine**: Bots cycle between `BOT_STATE_IDLE`, `BOT_STATE_SEEKING`, `BOT_STATE_COMBAT`, and `BOT_STATE_RESTING`.
- **Target Selection & Leashing**: Evaluates nearest hostile monsters within sensor range, honors combat leashes, and avoids stealing targets from low-health players.
- **Class-Specific Rotations**:
  - *Soldier / Knight / Champion*: Taunt, charge, melee strike sequences, shield defensive buffs.
  - *Muse / Mage / Cleric*: Spell distance kiting, elemental nuke casting, self-healing thresholds.
  - *Hawker / Raider / Scout*: Bow/gun range kiting, stealth opener, high-critical rapid attacks.
  - *Dealer / Bourgeois / Artisan*: Gun/launcher artillery fire, summon support bots, zuly shot skills.
- **Resting & Recovery**: When HP or MP drops below 25%, bots execute the sit stance (`Status->Stance = SITTING`), enter natural regeneration state, and stand when recovery reaches 95%.
- **Anti-Bounce Movement Sync**: Position packets (`0x79a`) are rate-limited with interpolation heuristics to prevent client-side rubberbanding.

### 2. Worldwide Grind-Spot Buff Bots (The Cleric Sisters)
Placed at key leveling spots across the world, these cleric bots provide essential buffs and bonfires:

| Sister | Location | Zone / Region |
| :--- | :--- | :--- |
| **SisterAngela** | Adventurer Plains | Starter Jellybean & Chorpy fields |
| **SisterMaria** | Valley of Luxen | Goblin & Beetle grinding grounds |
| **SisterTeresa** | Breezy Hills | Pomic & Honey Bee areas |
| **SisterClaire** | El Vergel | Desert training grounds |
| **SisterRose** | Anima Lake | Mid-level aquatic & beast spots |
| **SisterGrace** | Forest of Wisdom | Ghost & Elder monster zones |
| **SisterFaith** | Kenji Beach | High-level beach combat grounds |

### 3. Large Bonfire (ID 806) Aura & Maintenance
- **3x Natural Regeneration Aura**: Nearby players within a 25-meter radius gain triple HP and MP natural recovery.
- **Automatic Bonfire Maintenance**: The bot monitors bonfire duration and automatically renews the fire before it expires.
- **Combat Immunity & Exclusion**: Bonfire entities have special targeting masks preventing monsters from attacking or extinguishing the flame.

### 4. Smart Priority Buff Queue
- **Real-Player Priority**: The bot's sensory loop checks for players entering the camp radius. Real human players are placed at the front of the queue ahead of bot companions.
- **7-Charm Full Cleric Blessing Rotation**:
  1. **Pure Charm**: Maximum HP boost.
  2. **Mental Charm**: Maximum MP boost.
  3. **Dash / Haste Charm**: Movement and attack speed amplification.
  4. **Might Charm**: Physical attack power boost.
  5. **Defense Charm**: Armor and physical defense increase.
  6. **Magic Defense Charm**: Magic resistance shield.
  7. **Blessing / Accuracy Charm**: Hit rate and critical strike rating boost.
- **Anti-Looping Timer**: The bot inspects active buff bits on target players. It will not recast charms that have more than 60 seconds of remaining duration, preventing spell-spam lockups.

### 5. GM / Admin In-Game Bot Commands
Authorized Game Masters (Access Level $\ge 300$) can control bots using in-game chat commands:
- `/bot spawn [class] [level] [name]`: Spawns a bot with designated class, level, and gear.
- `/bot buff`: Triggers immediate area buffing from nearby cleric bots.
- `/bot kill [name]`: Despawns an active bot.
- `/bot list`: Lists active bot instances and coordinates.

### 6. Autonomous Market Vending Bots (`VendingCatalog.cpp` & `PlayerBot.cpp`)
Stationed permanently in high-density player trade corridors in Junon Polis and Canyon City of Zant, 18 autonomous vendor bots provide an active, authentic economy:

| # | Vendor Name | Zone / Map | Landmark Spot | Stall Signboard Title | Specialty Goods |
|---|---|---|---|---|---|
| 1 | `Merchant_Koji` | Junon Polis (2) | Spawn Point | `[Gems] T5-T7 Jewels & Diamonds` | Cut Diamonds, Rubies, Sapphires, Topazes (T5–T7) |
| 2 | `Trader_Jin` | Junon Polis (2) | Spawn / Mildun | `[Weapons] Rare Swords & Bows` | Caliburn +6, Death Bringer +7, Infernal Bow +6, Piercing Gun +6 |
| 3 | `Shop_Milo` | Junon Polis (2) | Near Mildun | `[Pots] HP/MP & Return Scrolls` | HP/MP Bottles, Return Scrolls (Zant, Junon, Eucar, Xita) |
| 4 | `Refiner_Orin` | Junon Polis (2) | Next to Crune | `[Refine] Talismans & Runes` | Grade 4–10 Talismans/Bindrunes, Planetary Runes, Arua Fate |
| 5 | `Artisan_Bax` | Junon Polis (2) | Crune & Storage | `[Crafting] Ores, Woods & Leathers` | Damascus, Mithril, Soft Leather, Spirit Stones, Heart/Feather drops |
| 6 | `Armorer_Gale` | Junon Polis (2) | Near Saki | `[Armor] Class Sets & Shields` | Knight Plate +4 set, Holy Body +6 set, Plate Shield +5 |
| 7 | `Mechanic_Torque`| Junon Polis (2) | Bridge / Mayor | `[PAT] Frames, Engines & Wheels` | Cart frames, high-output engines, durable wheels & cores |
| 8 | `Jeweler_Serena` | Junon Polis (2) | Near Mayor | `[Jewelry] Stat Rings & Necklaces` | Attribute rings, necklaces, and earrings with stat bonuses |
| 9 | `WingMaster_Aero`| Junon Polis (2) | Bridge Walkway | `[Wings] Angel, Devil & Fairies` | Angel, Devil, and Fairy wings with move speed bonuses |
| 10 | `Quartermaster_Rook`| Junon Polis (2)| Valor Shops | `[Ammo] Elemental Arrows & Bullets`| Elemental arrows (Fire, Lightning, Storm), Silver/Gold bullets |
| 11 | `Dealer_Vance` | Junon Polis (2) | Junon Order | `[Gear] Dual Weapons & Katars` | High-critical dual swords and assassin katars |
| 12 | `Vendor_Pippin` | Zant (1) | South Ramp | `[Starter] HP/MP Pots & Scrolls` | Novice health/mana vials and Plains/Zant return scrolls |
| 13 | `Scout_Robin` | Zant (1) | Ramp Walkway | `[Ammo] Hunting Arrows & Bullets` | Affordable wooden arrows and lead bullets |
| 14 | `Peddler_Toby` | Zant (1) | Entrance Path | `[Materials] Monster Drops & Iron` | Iron, leathers, claws, and early crafting materials |
| 15 | `Smith_Brant` | Zant (1) | Central Plaza | `[Weapons] Swords, Staffs & Guns` | Beginner weapons (Wooden Sword to Long Sword +4, Short Bows) |
| 16 | `Tailor_Lydia` | Zant (1) | Near Judy | `[Armor] Novice & Leather Armor` | Beginner clothing and leather armor sets |
| 17 | `GemTrader_Ruby`| Zant (1) | Near Raffle | `[Gems] Cut Jewels & Talismans` | Cut starter gems (T1–T3) and Grade 1–3 Talismans |
| 18 | `Collector_Felix`| Zant (1) | Near Keenu | `[Accessories] Rings & Back Bags`| Basic stat rings and leather backpacks for carry weight |

#### Architectural Highlights:
- **Wire Stance & Banners**: On spawn, bots set `Shop->open = true` and `Status->Stance = 1`. Approaching clients receive the sitting shop posture (`0x02`) and overhead banner via `CreateSpawnPacket`.
- **Live Trading**: Players interact via normal client UI (`pakShowShop` / `pakBuyShop`). Zulies transfer automatically.
- **Infinite Auto-Restock**: Deductions in `pakBuyShop` are intercepted when `otherclient->is_bot` is true, immediately restoring stackables to 999 and gear to 1.
- **Database Safety**: `SaveSlot41` skips MySQL queries for bots, keeping database tables clean and free of virtual bot items.
- **Immunity & Permanence**: Bots are marked static (`isDynamic = false`) so they are never pruned, and their HP/MP is auto-restored each cycle.

### 7. Dynamic Dungeon Instancing Engine (`CDungeonManager.cpp`)
Provides private, party-isolated `CMap` instances (slots 200–999) for all cave and temple dungeons across the world:
- **Supported Dungeons**:
  - **Goblin Cave**: Maps 24 (B1), 25, 26 (B2), 27, 28 (B3)
  - **George's Cave**: Maps 31 (B1), 32 (B2), 33 (B3)
  - **Oblivion Temple / Sea of Seeds**: Maps 41, 42, 43, 44
  - **Forgotten Temple**: Maps 56, 57
- **Client Protocol Compatibility**: The server streams `(base_zone != 0 ? base_zone : id)` in wire teleport packets (`0x7a8`). The client always receives a valid STB zone ID (0–121) avoiding client-side crashes, while the server engine tracks isolated combat, visibility, and monster spawns under dynamic instance index 200+.
- **Isolated Spawns & Respawns**: Monster spawn areas (`CSpawnArea`) are deep-copied from base IFO definitions upon instance creation. Each party fights their own private mobs with independent death/respawn timers and drop ownership.
- **Dungeon Timers & Warning Broadcasts**: 30-minute countdown timers per instance with automated warning broadcasts at 10m, 5m, and 1m. When the timer hits 0m, the dungeon collapses and players are safely evacuated to the cave entrance.
- **Party Wipe Detection & Auto-Evacuation**: When all players inside an instance fall in combat, `CDungeonManager::HandlePartyWipe` triggers an automated failure sequence: revives players with 20% HP/MP without death EXP penalties, purges debuffs, teleports them back to the entrance portal, and tears down the failed instance.
- **Player In-Game Commands**:
  - `/dungeon time`: Checks remaining minutes and seconds in current instance.
  - `/dungeon info`: Displays instance ID, base cave, party ID, and player count.
  - `/dungeon leave`: Safely exits the instance back to the cave entrance.
  - `/dungeon wipe`: (GM only) Simulates or forces party wipe handling.

### 8. Mail System & Parcel Delivery (`charpackets.cpp` & `gmcmds.cpp`)
Replaces legacy destructive mail wiping with a modern persistent mail and parcel delivery system:
- **Database Schema**: Added `is_read`, `zuly`, `item_head`, `item_data`, `item_name`, and `is_claimed` columns to `mail_list`.
- **Non-Destructive Reading**: Opening the client memo UI displays attached Zulies and items without deleting messages from the database.
- **Per-Message Deletion**: Action `0x04` (`DELETE FROM mail_list WHERE id=%u AND sendtocharid=%u`) deletes only the targeted message.
- **In-Game Chat Command Suite**:
  - `/mail list`: Lists last 10 messages with IDs, sender, read status, and attachment status.
  - `/mail read <id>`: Displays sender, message body, and attachment summary.
  - `/mail claim <id>`: Validates unclaimed mail, transfers attached Zulies (`0x7b1`) and item attachments (`0x71f`) into player inventory, and marks `is_claimed = 1`.
  - `/mail send <player> <zuly> [inv_slot] <message>`: Validates balance and inventory slot, deducts Zulies and item with live inventory packets, and persists mail parcel.
  - `/mail delete <id>`: Deletes specific mail by ID.

### 9. Complete Monster AIP Scripting Engine (`AiConditions.cpp` & `AiActions.cpp`)
All stubbed monster artificial intelligence opcodes are fully implemented:
- **Charm Attribute Evaluation**: Case 5 in `AI_GetAbility` queries player Charm (`Attr->Cha`).
- **World & Economy Variables**: Added `WorldVar[256]` and `EconomyVar[256]` arrays to `CWorldServer`.
- **Conditions**: `AICOND(015)` (WorldVar check), `AICOND(016)` (EconomyVar check), `AICOND(023)` (Game Map Time / daylight window check), `AICOND(026)` (Server Channel range check).
- **Actions**: `AIACT(026)` (Set WorldVar), `AIACT(027)` (Set EconomyVar), `AIACT(032)` (Set Zone PvP mode and broadcast packet `0x70f`), `AIACT(033)` (Set Zone Regen rate).

---

## Maintenance & Git Synchronization Workflow

To ensure the repository remains up to date as development progresses:

### Quick Commit & Push
```bash
git add -A
git commit -m "Milestone update: <description of changes>"
git push origin main
```

### Keeping the Repository Clean
The included `.gitignore` automatically prevents compiled binaries, CMake caches, and server logs (`worldserver.log`) from being committed, keeping the repository light and fast to clone.

---

## Credits & Licensing

- **Original Project**: OSRose Development Team ([dev-osrose.com](http://www.dev-osrose.com))
- **Evolution Enhancements**: Eich, Lmame, Tomiz, PY, Terr0risT, and community contributors.
- **RosE Resurrected**: Modern refactoring, stability patches, clan vaults, and autonomous bot AI.
- **License**: GNU General Public License v2 (GPL-2.0). See [LICENSE](LICENSE) for details.
