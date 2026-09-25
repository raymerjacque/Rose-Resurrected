#pragma once
/*---------------------------------------------------------------------------------------------------------
	Poject 137 Header File:

	File		: Enums.h

	Purpose		: Enum definitions.
	
	Revision	:  WHO	  | WHEN			| WHAT

				:   FK		2018-09-20		Created
				:   FK		2018-09-21		Added ItemType Enum.
				
	Comments	: Use pragma region if you are adding stuff for a different purpose.

---------------------------------------------------------------------------------------------------------*/

/// Chat Window Colors as implemented on the client, encoded to int32;
enum ChatWindow {
	TopWindow    = 0x01,
	BottomWindow = 0x02
};

/// For Building 1 Generic Message Component.
enum MessageType {
	msgTypeInfo,
	msgTypeHelp,
	msgTypeWarning,
	msgTypeError,
	msgTypeSystem,
	msgTypeStatus,
	msgTypeNotice,
	msgTypeGMNotice,
	msgTypeAnnouncement,
	msgTypeDebug,
	msgTypeBuySell,

};

enum DefinedColors {
	/// MsgColors for the top window
	rgbWhite        = 0xffffff,		// MsgColor for Top Chatbox
	rgbGray         = 0xffe0e5,		// System Message Type
	rgbOrange       = 0xff9600,		// MsgColor for Top Chatbox
	rgbYellow       = 0xffe47a,		// MsgColor for Top Chatbox
	rgbBlue         = 0x6496ff,		// MsgColor for Top Chatbox
	
	/// MessageColors mathing the client Scheme:
	rgbNormal		= 0xffffff,		// All			 255, 255, 255 		// White
	rgbShout		= 0x6496ff,		// Shout		 100, 250, 255 		// Blue-ish White	
	rgbParty		= 0xff9600,		// Party		 255, 150, 0   		// Orange	
	rgbTrade		= 0x64ff00,		// Trade		 100, 255, 0   		// Funky Green
	rgbWhisper		= 0xff64b4,		// Whisper		 255, 100, 180 		// Purpleish
	rgbNotice		= 0xffbcac,		// Notice		 255, 188, 172 		// Salmon Pink
	rgbSystem		= 0xffe0e5,		// System		 255, 224, 229 		// Grayish
	rgbError		= 0xff2828,		// Error		 255, 40 , 40  		// Red
	rgbQuest		= 0x97ddf1,		// Quest		 151, 221, 241 		// Darker than shout, blue
	rgbClan		    = 0xffe47a,		// Clan			 255, 228, 122 		// Fat Yellow
	rgbQuestReward	= 0xe0a2d1,		// QuestReward   224, 162, 209 		// Pink
	rgbHelpCaption  = 0xf8c471,		// Custom Help Header;
	rgbHelpMessage  = 0xfad7a0,		// Custom Help text;
	rbgGMNotice     = 0xff1493		// Deep Pink
};

enum class EntityState {
	Initialize,						// Preparing Character
	Ok,								// Character is participating
	LifeCycleDone,					// Character is can be deleted
	Removed							// Character is deleted.
};

enum class iComparer{
	Simple,
	Normal,
	Strict
};
#pragma region Item Related Info 

enum class StatType {
	Dodge,
	Accuracy
};

enum class ItemFamily {
	Equipment,						// Includes Clothes + Weapons	(1,2,3,4,5,6,8,9)
	Consumables,					// Consumables					(10)	
	Materials,						// Materials					(12)
	Jewelry,						// Jewelry						(7)
	Gems,							// Gems							(11)
	Quests,							// Quest Items					(13)
	Pat,							// CG+Cart						(14)
	Money,							// Just Money					(31)
	Unknown							// 0
};

enum eConditions {
	cPlanet = 75,
	cLevel = 31,
	cStrenght = 10,
	cDexterity = 11,
	cIntelligence = 12,
	cConcentration = 13,
	cCharm = 14,
	cSensibility = 15,
};

enum eJob {
	/// These map to CharInfo->Job;
	jobVisitor	= 0,
	jobSoldier	= 111,
	jobKnight	= 121,
	jobChampion	= 122,
	jobMuse		= 211,
	jobMage		= 221,
	jobCleric	= 222,
	jobHawker	= 311,
	jobRaider	= 321,
	jobScout	= 322,
	jobDealer	= 411,
	jobBourgeois= 421,
	jobArtisan	= 422
};

enum eClass {
	// these map to Job Conditions.
	// If you want to obtain the JobClass use CharInfo->jobClass() method;
	classVisitor = 0,		// No job
	classSoldier = 41,		// First Job
	classMuse = 42,			// First Job
	classHawker = 43,		// First Job
	classDealer = 44,		// First Job

	classFirstJob = 46,		// Composite Type
	classSecondJob = 47,	// Composite Type

	classSoldier2nd= 51,
	classMuse2nd = 52,
	classHawker2nd = 53,
	classDealer2nd = 54,
	
	classKnight = 61,		// Soldier 2nd job	
	classChampion = 62,	// Soldier 2nd job
	classMage = 63,		// Muse 2nd job
	classCleric = 64,		// Muse 2nd job
	classRaider = 65,		// Hawker 2nd job
	classScout = 66,		// Hawker 2nd job
	classBourgeois = 67,	// Dealer 2nd job
	classArtisan = 68,	// Dealer 2nd job
	classSoldierAndHawker = 71		// Composite Type;
};

enum eType {
	/// FK: Work in progress, adding the magic numbers to an enum. I will add them on-the-go.
	/// Item Type should match contents of STR_ITEMTYPE.stb, and values need to be 
	/// prefixed with `t` because it's a normal ENUM class so they can be used as INT
	tNone				= 0,
	tRing				= 171,
	tNecklace			= 172,
	tEarrings			= 173,

	tTwoHandedSword		= 221,
	tSpear				= 222,
	tTwoHandedAxe		= 223,
	tBow				= 231,
	tGun				= 232,
	
	tLauncher			= 233,
	tMagicStaff			= 241,
	tKatar				= 251,
	tDualSwords			= 252,
	tDualGuns			= 253,
	tCrossBow			= 271,

	tMagicItem			= 313,
	tSkillBook			= 314,
	tRepairHammer		= 315,
	tDrillItem          = 318,
	tSeperationDrill	= 319,		// FK: My gift to Rose 137 xD
	tAutoConsume		= 320,
	tGiftBox			= 322,
	tArrow				= 431,
	tBullet				= 432,
	tShell				= 433,
	
	tCartBody			= 511,
	tCartEngine			= 521,
	tCartWheels			= 531,
	tCartAccessory		= 551,
	tCartWeapon1		= 571,
	tCartWeapon2		= 572,
	tCartWeapon3		= 573,
	tCartWeapon4		= 574,
	tCastleGearBody		= 512,
	tCastleGearEngine	= 522,
	tCastleGearLeg		= 532,
	tCastleGearWeapon	= 552
	
	
		
	/* Remove from list when assigned; this is the reference values from the STB
	111	Mask
	112	Glasses
	113	Etc.
	121	Helmet
	122	Magic Hat
	123	Hat
	124	Hair Accessory
	131	Combat Uniform
	132	Magic Clothes
	133	Casual Clothes
	141	Gauntlet
	142	Magic Glove
	143	Glove
	151	Boots
	152	Magic Boots
	153	Shoes
	161	Back Armor
	162	Bag
	163	Wings
	164	Arrow Box
	165	Bullet Box
	166	Shell Box
	211	One-Handed Sword
	212	One-Handed Blunt Weapon
	242	Magic Tool
	261	Shield
	262	Support Tool
	311	Medicine
	312	Food
	313	Magic Item
	
	316	Quest Scroll
	317	Engine Fuel
	
	320	Automatic Consumption
	321	Time Coupon
	
	323	Reset Textbook
	324	% Healing Potion
	411	Jewel
	412	Work of Art
	421	Metal
	422	Otherworldly Metal
	423	Stone Material
	424	Wooden Material
	425	Leather
	426	Cloth
	427	Refining Material
	428	Chemicals
	429	Material
	430	Gathered Goods
	441	Quest Items
	442	Certification
	*/
};

enum eItemType {
	_MaxTypeCount  = 14,				// FK: Highest Possible Item Type.
	UnknownType	   = 0,					// FK: Unknown Type / Unset
	Face		   = 1,					// LISTFACEITEM.STB
	Cap            = 2,					// Hats, Caps (LIST_CAP.STB)
	Body           = 3,					// Body Armour (LIST_BODY.STB)
	Glove          = 4,					// Gloves (LIST_ARMS.STB)
	Shoe           = 5,					// Shoes (LIST_FOOT.STB)
	Back           = 6,					// Backshields, Backpacks and Wings (LIST_BACK.STB)
	Jewel          = 7,					// Ring, Earring, Necklace (LIST_JEWEL.STB)
	Weapon         = 8,					// Weapons (LIST_WEAPON.STB)
	SubWeapon      = 9,					// Offhand Items (LIST_SUBWPN.STB)
	Consumable     = 10,				// Scrolls, Food, Potions (LIST_USEITEM.STB)
	Gem            = 11,				// Gems (LIST_JEMITEM.STB)
	Materials      = 12,				// Materials (LIST_NATURAL.STB)
	QuestItem      = 13,				// Quest Items (LIST_QUESTITEM.STB)
	CartCG         = 14,				// Cart, Castle Gear (LIST_PAT.STB)
	Mount          = 15,				// NOT USED in v137.
	Money		   = 31					// Discovered that the client uses itemtype 31 to identify money type drops;
};

enum eDropType {
	DropMoney = 1,
	DropItem = 2
};

/// Packet Actions (Enum classes, they need to be casted like:    eActionModifyItem(0x02);
enum class eItemGemOP : unsigned char {
	Success = 0x01,
	NoSocket = 0x02,
	SocketUsed = 0x03,			// Already Gem in that socket.
	Invalid  = 0x12,
};

enum class eActionModifyItem {
	/// Actions performed on Items.
	AddGem				= 0x01,
	Break				= 0x02,
	// where is 0x03?
	// Where is 0x04?
	Refine				= 0x05,
	Drill				= 0x06,
	// Where is 0x07 ?
	Seperate			= 0x08,
	//SeperatePerfect		= 0x09
};

enum class eActionStorage {
	/// Actions Performed on Storage.
	DepositItem   = 0x00,
	WithdrawItem  = 0x01,
	DepositZulie  = 0x10,
	WithDrawZulie = 0x11
};

/// Slots and Storage
enum eSlots {
	UnknownSlot = 0xffff,				// Slot Unknown
	MoneySlot = 0x000,				// FK : When dropping zulie it originates from slot 0;
	/// Regular Inventory Slots

	MaskSlot = 1,				// Inventory Slot for Masks
	CapSlot = 2,				// Inventory Slot for Hats (caps)
	BodySlot = 3,				// Inventory Slot for Body (suit)
	BackSlot = 4,				// Inventory Slot for Back Items (Bags, wings)
	GloveSlot = 5,				// Inventory Slot for Gloves
	ShoeSlot = 6,				// Inventory Slot for Shoes
	WeaponSlot = 7,				// Inventory Slot for Weapon
	ShieldSlot = 8,				// Inventory Slot for Shield or Offhand item
	RingSlot = 9,				// Inventory Slot for Rings
	NecklaceSlot = 10,				// Inventory Slot for Necklace
	EarringSlot = 11,				// Inventory Slot for Earrings

	// 12..41:  = Tab 1	--> Equipment
	// 42..71   = Tab 2	--> Consumables
	// 72..111  = Tab 3	--> Materials
	// 112..131 = Tab 4	--> Cart + CG

	/// ABC  Slots
	ArrowSlot = 132,				// Inventory Slot for Loaded Arrows
	BulletSlot = 133,				// Inventory Slot for Bullets
	CannonSlot = 134,				// Inventory Slot for Cannon Balls

	/// PAT Slots
	CartFrame = 135,				// Inventory Slot for Cart Frame
	CGFrame = 135,					// Inventory Slot for CG Frame
	CartEngine = 136,				// Inventory Slot for Cart Engine
	CGEngine = 136,					// Inventory Slot for CG Engine
	CartWheels = 137,				// Inventory Slot for Cart Wheels
	CGLegs = 137,					// Inventory Slot for CG Legs
	CartWeapon = 138,				// Inventory Slot for Cart Weapon
	CGArms = 138,					// Inventory Slot for CG Arms
	CartAccessory = 138,			// Inventory Slot for Cart Accessory (hood, back seat)
	CGAccessory = 138,				// Inventory Slot for CG accessory

	/// FK: A more generic PAT approach
	patFrameSlot = 135,
	patEngineSlot = 136,
	patMovementSlot = 137,
	patWeaponSlot = 139,			// FK this one was switched 
	patAccesorySlot = 138			// FK this one was switched
};

enum class MonsterDifficulty{
	Unknown   ,
	Gray      ,
	LightBlue ,
	Blue      ,
	Green     ,
	Yellow    ,
	Orange    ,
	Red       ,
	Pink      ,
	Purple    
};

#pragma endregion

enum class eAbility {
	/// FK: Added; this is the full dump from list_ability.spd;
	None						= 0,				// FK: Custom Attribute;
	Sex							= 2, 
	Birthstone					= 3, 
	Job							= 4, 
	Faction						= 5, 
	Ranking						= 6, 
	Reputation					= 7, 
	Face						= 8, 
	Hair						= 9, 
	Strength					= 10, 
	Dexterity					= 11, 
	Intelligence				= 12, 
	Concentration				= 13, 
	Charm						= 14, 
	Sense						= 15, 
	HP							= 16, 
	MP							= 17, 
	AttackPower					= 18, 
	Defense						= 19, 
	AttackAccuracy				= 20, 
	MagicResistance				= 21, 
	DodgeRate					= 22, 
	MovementSpeed				= 23, 
	AttackSpeed					= 24, 
	InventoryCapacity			= 25, 
	Critical					= 26, 
	HPRecoveryRate				= 27, 
	MPRecoveryRate				= 28, 
	MPConsumption				= 29, 
	ExperienceRate				= 30, 
	Level						= 31, 
	Point						= 32, 
	//Tendency					= 33, 
	PKFlag						= 33,
	//PKLevel					= 34, 
	Team_No						= 34,
	HeadSize					= 35, 
	BodySize					= 36, 
	SkillPoints					= 37, 
	MaxHP						= 38, 
	MaxMP						= 39, 
	Money						= 40, 
	PassiveUnarmedATKPower		= 41, 
	PassiveOneHandedATKPower	= 42, 
	PassiveTwoHandedATKPower	= 43, 
	PassiveBowATKPower			= 44, 
	PassiveGunATKPower			= 45, 
	PassiveMagicWeaponATKPower	= 46, 
	PassiveCrossbowAtkPower		= 47, 
	PassiveKatarAtkPower		= 48, 
	PassiveBowAttackSpeed		= 49, 
	PassiveGunAttackSpeed		= 50, 
	PassiveKatarAttackSpeed		= 51, 
	PassiveMoveSpeed			= 52, 
	PassiveDefense				= 53, 
	PassiveMaxHP				= 54, 
	PassiveMaxMP				= 55, 
	PassiveHPRecovery			= 56, 
	PassiveMPRecovery			= 57, 
	PassiveBagpackCapacity		= 58, 
	PassiveSalesDiscount		= 59, 
	PassiveSalesPremium			= 60, 
	PassiveMPReduction			= 61, 
	PassiveSummonGaugeIncrease	= 62, 
	PassiveDropRateIncrease		= 63,
	ItemDropRate				= 79,			/// The amount of drops in total		
	ItemDropQuantity			= 63,			/// The amount of 'stack count' for each single drop	// FK: Bour ?
	ItemDropQuality				= 64,			/// The Quality of drops (blue drops)					// FK: Artisan ?
	ItemGrayDropIncrease		= 65,			/// Medal, Server side only
	CraftTalent					= 70,

	Race						= 71,
	FAME_G						= 73, 
	FAME_B						= 74, 
	PlanetRequirements			= 75, 
	Stamina						= 76, 
	IMMUNITY					= 78, 
	UnionPoints01				= 81, 
	UnionPoints02				= 82, 
	UnionPoints03				= 83, 
	UnionPoints04				= 84, 
	UnionPoints05				= 85, 
	UnionPoints06				= 86, 
	UnionPoints07				= 87, 
	UnionPoints08				= 88, 
	UnionPoints09				= 89, 
	UnionPoints10				= 90, 
	ClanID						= 91, 
	ClanPoints					= 92, 
	ClanRank					= 93, 
	NoStorageCharge				= 94, 
	StorageExpansion			= 95, 
	PersonalShop				= 96, 
	CartGauge					= 97, 
	PassiveMagicResistance		= 98, 
	PassiveAttackAccuracy		= 99, 
	PassiveCritical				= 100, 
	PassiveDodgeRate			= 101, 
	PassiveShieldDefense		= 102, 
	PassiveImmunity				= 103, 
	PassiveOneHandedSwordAP		= 104, 
	PassiveOneHandedMeleeAP		= 105, 
	PassiveTwoHandedSwordAP		= 106, 
	PassiveTwoHandedSpearAP		= 107, 
	PassiveTwoHandedAxeAP		= 108, 
	PassiveBowAP				= 109, 
	PassiveGunAP				= 110, 
	PassiveLauncherAP			= 111, 
	PassiveDualGunAP			= 112, 
	PassiveStaffAP				= 113, 
	PassiveWandAP				= 114, 
	PassiveKatarAP				= 115, 
	PassiveDualWieldAP			= 116, 
	PassiveCrossbowAP			= 117, 
	PassiveOneHandedSwordASPD	= 118, 
	PassiveOneHandedMeleeASPD	= 119, 
	PassiveTwoHandedSwordASPD	= 120, 
	PassiveTwoHandedSpearASPD	= 121, 
	PassiveTwoHandedAxeASPD		= 122, 
	PassiveBowASPD				= 123, 
	PassiveGunASPD				= 124, 
	PassiveLauncherASPD			= 125, 
	PassiveDualGunASPD			= 126, 
	PassiveStaffASPD			= 127, 
	PassiveWandASPD				= 128, 
	PassiveKatarASPD			= 129, 
	PassiveDualSwordASPD		= 130, 
	PassiveCrossbowASPD			= 131,

	EXPItem						= 133,				/// Should be obsoleted;
	EXPNullifier				= 150,				/// Server Side Only.
	EXPReducer					= 151,				/// Server Side Only.

	MaxSummonGauge				= 160,				/// Maximum summon gauge value
	UsedSummonGauge				= 161				/// Currently Used summon gauge value
};
#pragma region Skill related data

enum SkillTargets {
	SELF				= 0,
	PARTY_MEMBER		= 1,
	CLAN_MEMBER			= 2,
	ALLY				= 3,
	MONSTER				= 4,
	HOSTILE_CHARACTER	= 5,
	HOSTILE_TEAM_MEMBER	= 6,
	ALL_CHARACTERS		= 7,
	ALL_MEMBERS			= 8,
	FAINTED_ALLY		= 9,
	HOSTILE_MONSTER		= 10
};

enum class SkillFamily {
	Unknown			= 0,
	BasicSkills		= 1,
	ClassSkills		= 2,
	ItemMallSkills	= 3,
	UniqueSkills    = 4,
	PATSkills       = 5
};

enum class eBuffType {
	Unknown,
	Buff,
	Debuff,
	InstantBuff,
};

enum class eBuffBinaryData {
	All,
	DebuffOnly,
	BuffOnly,
};
enum class eBuffStatus : unsigned int {
///FK: The official Bitflags from the client source.
//  FLAG                      Value         // Status	Description
//  --------------------------------------------------------------------------------------------
	None					= 0x00000000,
	HPRestoration			= 0x00000001,	// 1		Restore HP over Time (dps setting)
	MPRestoration			= 0x00000002,	// 4		Restore MP over Time (dps setting)
	Poisoned				= 0x00000004,	// 7		-> Debuff; Poisoned = Flamed.
	MaxHP					= 0x00000010,	// 12		// 
	MaxMP					= 0x00000020,	// 13
	Dash					= 0x00000040,	// 14
	Slow					= 0x00000080,	// 15		-> Debuff
	HasteAttack				= 0x00000100,	// 16
	SlowAttack				= 0x00000200,	// 17		-> Debuff
	AttackPowerUp			= 0x00000400,	// 18
	AttackPowerDown			= 0x00000800,	// 19		-> Debuff
	DefenseUp				= 0x00001000,	// 20
	DefenseDown				= 0x00002000,	// 21		-> Debuff
	MDefUp					= 0x00004000,   // 22
	MDefDown				= 0x00008000,	// 23		-> Debuff
	HitRateUp				= 0x00010000,   // 24
	HitRateDown				= 0x00020000,	// 25		-> Debuff
	CritUp					= 0x00040000,   // 26
	CritDown				= 0x00080000,	// 27		-> Debuff
	DodgeUp					= 0x00100000,	// 28
	DodgeDown				= 0x00200000,	// 29 		-> Debuff
	Muted					= 0x00400000,	// 30		-> Debuff
	Fainted					= 0x00800000,	// 31		-> Debuff
	FLAG_ING_FAINTING		= 0x01000000,	// 32 		Unknown, need more research.
	Camouflage				= 0x02000000,	// 33		-> Buff
	Invisible				= 0x04000000,	// 34		transparent	// Buff
	ShieldDamage			= 0x08000000,	// 35		Shield Damage
	AdditionalDamage		= 0x10000000,	// 54		Unknown, need more research.
	HPDown					= 0x20000000,	// 43		Unknown, need more research.
	FLAG_ING_REVIVE			= 0x40000000,	// NONE		Need to look this up. Revivable maybe ?
	Taunted					= 0x80000000,	// NONE		Debuff
};

enum SkillType {
	NO_ACTION					= 0,
	BASIC_ACTION				= 1,
	CRAFTING_SKILL				= 2,
	DAMAGE_ACTION				= 3,
	WEAPON_POWER_UP_SELF		= 4,
	WEAPON_POWER_UP				= 5,
	MAGIC_ATTACK				= 6,
	AOE_MAGIC_DAMAGE			= 7,
	BUFFSELF					= 8,
	BUFF_DEBUFF_OTHER			= 9,
	MAGIC_RECOVERY_NO_TARGET	= 10,
	MAGIC_RECOVERY				= 11,
	BUFF_SELF2					= 12,
	BUFF_OTHER2					= 13,
	SUMMONING_MAGIC				= 14,
	PASSIVE						= 15,
	EMOTION						= 16,
	AOE_DAMAGE_SELF				= 17,
	TELEPORT					= 18,
	DAMAGE_ABSORBTION			= 19,
	RESSURECTION				= 20
 };

enum MonsterAnimations {
	MOB_ANI_STOP				= 0,
	MOB_ANI_MOVE				= 1,
	MOB_ANI_ATTACK				= 2,
	MOB_ANI_HIT					= 3,
	MOB_ANI_DIE					= 4,
	MOB_ANI_RUN					= 5,
	MOB_ANI_CASTING01			= 6,
	MOB_ANI_SKILL_ACTION01		= 7,
	MOB_ANI_CASTINGN02			= 8,
	MOB_ANI_SKILL_ACTION02		= 9,
	MOB_ANI_ETC					= 10,
	MAX_MOB_ANI					= 11
};

enum SkillActions {
	DMG_ACT_RESERVED			= 1,
	DMG_ACT_ATTACK				= 2,
	DMG_ACT_HIT					= 4,
	DMG_ACT_CRITICAL			= 8,
	DMG_ACT_DEAD				= 16
};



#pragma endregion

#pragma region team management
enum TeamDefaults {
	MonsterDefaultTeam				= 64,
	PlayerDefaultTeam				= 2,
};

/// Use these enums in 0x07d1 packets that we send to the client or receive from the client
enum PartyReply {							// PY is in a really strange mood today Maybe the party code is making his brain turn to mush
	PARTY_REPLY_NOT_FOUND,					//  0	Did not find the target.
	PARTY_REPLY_BUSY,						//  1	The target cannot participate in the State party. He not a US citizen yet so he doesn't have the right to participate in local politics apparently.
	PARTY_REPLY_ACCEPT_MAKE	,				//  2	Let's make a party together
	PARTY_REPLY_ACCEPT_JOIN,				//  3	Yay!! Joined a party
	PARTY_REPLY_REJECT_JOIN,				//	4	Nope. Stuff your stupid party. I'm a solo player
	PARTY_REPLY_DESTROY,					//	5	The party has been eating out. What kind of translation engine did you use? www.randombullshit.com?
	PARTY_REPLY_FULL_MEMBERS,				//	6	It is the only land in the garden. And there it is again. Or maybe there's some deep hidden meaning in these seemingly random ramblings. Is the internet becoming sentient? I knew this would happen eventually. I warned them all but would anybody listen?
	PARTY_REPLY_INVALID_LEVEL,				//	7	Too much level difference
	PARTY_REPLY_CHANGE_OWNER,				//	8	MY party now BWAHAHAHA!!! Now i'm gonna kick everybody out and keep all the loot for myself. Wait!! Where did all the experience boost go? You mean to say that I actually needed all those losers. Aww f*** it!!
	PARTY_REPLY_CHANGE_OWNERnDISCONN,		//	9	Party Owner logged off. WTF are we gonna do now? Anyone? Don't look at me. I don't wanna be the leader
	PAATY_REPLY_NO_CHARGE_TARGET,			//	10	What exactly is a PAATY? Is it one of those jamaican things with spicy minced beef? I'll take 3 please

	PARTY_REPLY_BAN						 = 128,	//  We don't like your tone so F*** off! You aren't welcome in this party
	PARTY_REPLY_DISCONNECT				 = 129,	//	Abnormal termination ... I assume this means there is a NORMAL kind of termination then.
	PARTY_REPLY_REJOIN					 = 130	//	Abnormal termination after the fold. Now apparently we learned to fold packets. What's next? Folding space like in Dune. That was an awesome and highly under-rated movie
};



#pragma endregion


#pragma region Booster Data
enum class UseScripts {
	None				= 0xffff,
	DoSkill             = 0,		// Generic Skill Handler;
	FoodAndDrinks		= 1,		// Consume HP, MP Item.
	InstantConsume		= 2,		// HP, MP, Stamina, Clanpoints ... all stuff that looks like a star.
	LearnSkill			= 3,
	Refuel				= 4,		
	SkillResets			= 5,
	Boosters			= 6,

	/// These ones are obosolete since they will be routed through startAction.
	/// The numbers above have been renumbered.
	//ReturnScroll		= 2,			// Teleportation Scrolls
	//ClanPoints		= 13,
	//CharmScroll		= 3,		// Blood Charm etc. -->Actually unused.
	//Fireworks			= 4,		// Dance Scroll | FireCrackers | Emotion
	//SpawnSummon       = 5,
	//SnowBall			= 6,
	//SelfBuffScroll	= 7,		// scrolls [damage,maxhp,...]
	//QuestHandledSkill	= 11,
	//DanceScroll		= 14,		// Dance Scrolls
	//Rocks				= 15,		/// FK : We don't have these.
	//Bombs				= 16,		/// FK : We don't have these.
	//Emotions			= 19,

};

enum class eSkillReset {
	/// Wankypanky values, but this is in the STB.
	Job    = 20,
	Unique = 40,
	IM     = 50,
	All    = 100
};

#pragma endregion

#pragma region Quests
enum class QuestActions {
	ADD_QUEST			= 1,
	DELETE_QUEST		= 2,
	DO_TRIGGER			= 3
};

enum class QuestResult {
	RESULT_QUEST_REPLY_NULL					= 0,
	RESULT_QUEST_REPLY_ADD_SUCCESS			= 1,
	RESULT_QUEST_REPLY_ADD_FAILED			= 2,
	RESULT_QUEST_REPLY_DEL_SUCCESS			= 3,
	RESULT_QUEST_REPLY_DEL_FAILED			= 4,
	RESULT_QUEST_REPLY_TRIGGER_SUCCESS		= 5,		// This packet is received, the client applies the compensation.
	RESULT_QUEST_REPLY_TRIGGER_FAILED		= 6
};

enum class MoveMode {
	MOVE_MODE_WALK							=	0x00,    // Walking Mode
	MOVE_MODE_RUN							=	0x01,    // Stance::RUNNING Mode
	MOVE_MODE_DRIVE							=	0x02,    // Driving
	MOVE_MODE_RIDEON						=	0x04     // Passenger
};

enum class MoveModeToggle {
	TOGGLE_TYPE_RUN							=	0x00,	// Toggle Run/Walk
	TOGGLE_TYPE_SIT							=	0x01,	// Toggle Sit/Stand
	TOGGLE_TYPE_DRIVE						=	0x02	// Toggle Drive on/off
};

enum class Stance {
	_NOTHING								=	0,
	_ATTACKING								=	1,
	_WALKING								=	2,
	_RUNNING								=	3,
	_DRIVING								=	4,
	_SITTING								=	5,
	_PASSENGER								=	6
};

enum class Commands {
	CMD_STOP								=	0x00,
	CMD_MOVE								=	0x01,
	CMD_ATTACK								=	0x02,
	CMD_DIE									=	0x03,
	CMD_PICK_ITEM							=	0x04,
	CMD_SKILL2SELF							=	0x06,
	CMD_SKILL2OBJ							=	0x07,
	CMD_SKILL2POS							=	0x08,
	CMD_RUNAWAY								=	0x09,			
	CMD_SIT									=	0x0a,
	CMD_STORE								=	0x0b
};

#pragma endregion
