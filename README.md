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

---

## Roadmap & Pending Milestones

### Milestone 6 — Dynamic Dungeon Instancing & Polish (Next Target)
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
