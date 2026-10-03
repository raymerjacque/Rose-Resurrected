#include "VendingCatalog.h"
#include <vector>
#include <algorithm>
#include <cstdlib>

bool PopulateVendingInventory( CPlayer* bot, int category )
{
    if ( !bot || !bot->Shop ) return false;

    std::vector<SVendingItemDef> stock;

    switch ( category )
    {
        case VEND_CAT_WEAPONS_HIGH:
            // High-Tier Weapons (Map 2: Trader_Jin)
            stock = {
                { WEAPON, 101, 1,  80000, 4 }, // Haedong Sword +4
                { WEAPON, 103, 1, 150000, 5 }, // Sword of Hardship +5
                { WEAPON, 121, 1, 250000, 6 }, // Caliburn +6
                { WEAPON, 123, 1, 180000, 4 }, // Bastard Sword +4
                { WEAPON,  23, 1, 220000, 5 }, // Blade of Saint Kreuz +5
                { WEAPON,  27, 1, 320000, 7 }, // Death Bringer +7
                { WEAPON, 221, 1, 140000, 5 }, // Centauri Bow +5
                { WEAPON, 223, 1, 260000, 6 }, // Infernal Bow +6
                { WEAPON, 241, 1, 200000, 4 }, // Justice Cannon +4
                { WEAPON, 243, 1, 280000, 6 }, // Piercing Gun +6
                { WEAPON, 303, 1, 120000, 4 }, // Animal Rod +4
                { WEAPON, 305, 1, 180000, 5 }, // White Staff +5
                { WEAPON,  45, 1, 160000, 4 }, // Great Hammer +4
                { WEAPON, 188, 1, 240000, 5 }  // Fury Spantun +5
            };
            break;

        case VEND_CAT_ARMOR_HIGH:
            // High-Tier Armor & Shields (Map 2: Armorer_Gale)
            stock = {
                { BODY,       29, 1,  45000, 3 }, // Nomadic Suit +3
                { BODY,       34, 1, 110000, 4 }, // Knight Plate +4
                { CAP,        34, 1,  50000, 4 }, // Knight Helm +4
                { GLOVE,      34, 1,  35000, 4 }, // Knight Gauntlets +4
                { SHOE,       34, 1,  35000, 4 }, // Knight Boots +4
                { BODY,       37, 1, 180000, 6 }, // Holy Body +6
                { CAP,        37, 1,  85000, 5 }, // Holy Helm +5
                { GLOVE,      37, 1,  60000, 5 }, // Holy Gauntlets +5
                { SHOE,       37, 1,  60000, 5 }, // Holy Boots +5
                { BODY,       66, 1,  95000, 4 }, // Mage Robe +4
                { BODY,       71, 1, 140000, 5 }, // Cleric Robe +5
                { BODY,      126, 1, 160000, 5 }, // Raider Armor +5
                { SUBWEAPON,   7, 1,  55000, 3 }, // Kite Shield +3
                { SUBWEAPON,  10, 1, 125000, 5 }  // Plate Shield +5
            };
            break;

        case VEND_CAT_GEMS:
            // High-Tier Cut Gems (Map 2: Merchant_Koji)
            stock = {
                { JEM, 314, 999,  50000, 0 }, // Ruby [4]
                { JEM, 315, 999, 120000, 0 }, // Ruby [5]
                { JEM, 316, 999, 300000, 0 }, // Ruby [6]
                { JEM, 317, 999, 650000, 0 }, // Ruby [7]
                { JEM, 324, 999,  50000, 0 }, // Sapphire [4]
                { JEM, 325, 999, 120000, 0 }, // Sapphire [5]
                { JEM, 326, 999, 300000, 0 }, // Sapphire [6]
                { JEM, 327, 999, 650000, 0 }, // Sapphire [7]
                { JEM, 334, 999,  50000, 0 }, // Topaz [4]
                { JEM, 335, 999, 120000, 0 }, // Topaz [5]
                { JEM, 336, 999, 300000, 0 }, // Topaz [6]
                { JEM, 337, 999, 650000, 0 }, // Topaz [7]
                { JEM, 354, 999,  50000, 0 }, // Peridot [4]
                { JEM, 355, 999, 120000, 0 }, // Peridot [5]
                { JEM, 356, 999, 300000, 0 }, // Peridot [6]
                { JEM, 357, 999, 650000, 0 }, // Peridot [7]
                { JEM, 364, 999,  75000, 0 }, // Diamond [4]
                { JEM, 365, 999, 180000, 0 }, // Diamond [5]
                { JEM, 366, 999, 450000, 0 }, // Diamond [6]
                { JEM, 367, 999, 950000, 0 }  // Diamond [7]
            };
            break;

        case VEND_CAT_REFINE:
            // Refining Catalysts & Planetary Runes (Map 2: Refiner_Orin)
            stock = {
                { NATURAL,  74, 999,  15000, 0 }, // 4th Grade Talisman
                { NATURAL,  75, 999,  30000, 0 }, // 5th Grade Talisman
                { NATURAL,  76, 999,  60000, 0 }, // 6th Grade Talisman
                { NATURAL,  77, 999, 120000, 0 }, // 7th Grade Talisman
                { NATURAL,  78, 999, 250000, 0 }, // 8th Grade Talisman
                { NATURAL,  79, 999, 500000, 0 }, // 9th Grade Talisman
                { NATURAL,  80, 999, 990000, 0 }, // 10th Grade Talisman
                { NATURAL,  84, 999,  10000, 0 }, // 4th Grade Bindrune
                { NATURAL,  85, 999,  20000, 0 }, // 5th Grade Bindrune
                { NATURAL,  86, 999,  40000, 0 }, // 6th Grade Bindrune
                { NATURAL,  87, 999,  80000, 0 }, // 7th Grade Bindrune
                { NATURAL,  88, 999, 160000, 0 }, // 8th Grade Bindrune
                { NATURAL, 375, 999,  30000, 0 }, // 5th Grade Apotrope
                { NATURAL, 376, 999,  60000, 0 }, // 6th Grade Apotrope
                { NATURAL, 445, 999,  80000, 0 }, // Venurune
                { NATURAL, 446, 999, 120000, 0 }, // Mercurune
                { NATURAL, 456, 999, 150000, 0 }, // Nepturune
                { NATURAL, 448, 999, 200000, 0 }, // Jupiterune
                { NATURAL, 449, 999, 250000, 0 }, // Saturune
                { NATURAL, 457, 999, 300000, 0 }, // Plutorune
                { NATURAL, 392, 999, 180000, 0 }, // Ancient Scarab
                { NATURAL, 550, 999, 450000, 0 }  // Arua Fate
            };
            break;

        case VEND_CAT_MATERIALS:
            // Crafting Materials & Monster Drops (Map 2: Artisan_Bax)
            stock = {
                { NATURAL,   5, 999,  1500, 0 }, // Iron
                { NATURAL,   6, 999,  3500, 0 }, // Silver Iron
                { NATURAL,   7, 999,  6000, 0 }, // Steel
                { NATURAL,  10, 999, 15000, 0 }, // Damascus
                { NATURAL,  15, 999, 25000, 0 }, // Mithril
                { NATURAL,  43, 999,  2000, 0 }, // Smooth Leather
                { NATURAL,  44, 999,  3000, 0 }, // Rough Leather
                { NATURAL,  45, 999,  5000, 0 }, // Soft Leather
                { NATURAL, 112, 999,  3000, 0 }, // Silver Thread
                { NATURAL, 113, 999,  6000, 0 }, // Gold Thread
                { NATURAL, 114, 999, 12000, 0 }, // Platinum Thread
                { NATURAL, 196, 999,  1000, 0 }, // Bird Feather
                { NATURAL, 197, 999,  4000, 0 }, // Fairy Powder
                { NATURAL, 198, 999,  8000, 0 }, // Baby Angel Feather
                { NATURAL, 199, 999,  8000, 0 }, // Baby Devil Feather
                { NATURAL, 201, 999,  3000, 0 }, // Steam Oil
                { NATURAL, 205, 999,  5000, 0 }, // Mana Oil
                { NATURAL, 261, 999,  2500, 0 }, // Wolf Claw
                { NATURAL, 264, 999, 15000, 0 }, // Giant Heart
                { NATURAL, 275, 999,  3500, 0 }, // Spider Web
                { NATURAL, 295, 999, 20000, 0 }, // Water Spirit Stone
                { NATURAL, 296, 999, 20000, 0 }, // Wind Spirit Stone
                { NATURAL, 297, 999, 20000, 0 }, // Earth Spirit Stone
                { NATURAL, 298, 999, 20000, 0 }, // Fire Spirit Stone
                { NATURAL, 299, 999, 50000, 0 }  // Arua Spirit Stone
            };
            break;

        case VEND_CAT_POTIONS_SCROLLS:
            // Consumables & Scrolls (Map 2: Shop_Milo)
            stock = {
                { CONSUMIBLE,   5, 999,   600, 0 }, // Health Bottle (M)
                { CONSUMIBLE,   6, 999,  1200, 0 }, // Health Bottle (L)
                { CONSUMIBLE,  12, 999,  2500, 0 }, // Vital Water (L)
                { CONSUMIBLE,  14, 999,   800, 0 }, // Red Potion
                { CONSUMIBLE,  25, 999,   800, 0 }, // Mana Bottle (M)
                { CONSUMIBLE,  32, 999,  3500, 0 }, // Spiritual Water (XL)
                { CONSUMIBLE,  33, 999,  1000, 0 }, // Blue Potion
                { CONSUMIBLE,  81, 999,  5000, 0 }, // Max MP Potion
                { CONSUMIBLE,  82, 999,  3000, 0 }, // Purify Potion
                { CONSUMIBLE,  90, 999,  8000, 0 }, // Attack Speed Potion
                { CONSUMIBLE, 351, 999,  1000, 0 }, // Zant Return Scroll
                { CONSUMIBLE, 352, 999,  1500, 0 }, // Junon Polis Return Scroll
                { CONSUMIBLE, 353, 999,  3000, 0 }, // Eucar Return Scroll
                { CONSUMIBLE, 354, 999,  3000, 0 }, // Xita Refuge Return Scroll
                { CONSUMIBLE, 313, 999, 15000, 0 }, // Advanced HP Scroll
                { CONSUMIBLE, 314, 999, 15000, 0 }, // Advanced MP Scroll
                { CONSUMIBLE, 317, 999, 20000, 0 }, // Advanced Defense Scroll
                { CONSUMIBLE, 319, 999, 25000, 0 }, // Advanced Damage Scroll
                { CONSUMIBLE, 963, 999, 18000, 0 }  // Speed Booster (Extra Max)
            };
            break;

        case VEND_CAT_AMMO:
            // High-Tier Ammunition (Map 2: Quartermaster_Rook)
            stock = {
                { NATURAL, 305, 999,  5, 0 }, // Metal Arrow
                { NATURAL, 306, 999,  8, 0 }, // Gilded Arrow
                { NATURAL, 307, 999, 12, 0 }, // Platinum Arrow
                { NATURAL, 311, 999, 15, 0 }, // Fire Arrow
                { NATURAL, 316, 999, 20, 0 }, // Lightning Arrow
                { NATURAL, 317, 999, 25, 0 }, // Storm Arrow
                { NATURAL, 322, 999,  5, 0 }, // Gun Bullet
                { NATURAL, 323, 999,  8, 0 }, // Lead Bullet
                { NATURAL, 325, 999, 12, 0 }, // Silver Bullet
                { NATURAL, 326, 999, 16, 0 }, // Gold Bullet
                { NATURAL, 331, 999, 20, 0 }, // Fire Bullet
                { NATURAL, 341, 999, 15, 0 }, // Cannon Shell
                { NATURAL, 342, 999, 25, 0 }, // Lead Shell
                { NATURAL, 344, 999, 40, 0 }, // Bumper Shell
                { NATURAL, 345, 999, 50, 0 }  // Fire Shell
            };
            break;

        case VEND_CAT_ACCESSORIES:
            // Accessories & Jewelry (Map 2: Jeweler_Serena)
            stock = {
                { JEWEL, 11, 1, 45000, 0 }, // Ring of Strength (+5 STR)
                { JEWEL, 12, 1, 55000, 0 }, // Ring of Dexterity (+5 DEX)
                { JEWEL, 13, 1, 50000, 0 }, // Ring of Wisdom (+5 INT)
                { JEWEL, 14, 1, 40000, 0 }, // Ring of Health (+5 CON)
                { JEWEL, 15, 1, 60000, 0 }, // Ring of Critical (+5 SEN)
                { JEWEL, 22, 1, 75000, 0 }, // Ruby Necklace
                { JEWEL, 23, 1, 75000, 0 }, // Sapphire Necklace
                { JEWEL, 24, 1, 75000, 0 }, // Emerald Necklace
                { JEWEL, 25, 1, 90000, 0 }, // Diamond Necklace
                { JEWEL, 32, 1, 40000, 0 }, // Silver Earrings
                { JEWEL, 33, 1, 70000, 0 }, // Gold Earrings
                { JEWEL, 34, 1, 120000, 0 } // Platinum Earrings
            };
            break;

        case VEND_CAT_WINGS:
            // Wings & Back Items (Map 2: WingMaster_Aero)
            stock = {
                { BACK, 221, 1, 150000, 0 }, // Fairy Wings
                { BACK, 222, 1, 180000, 0 }, // Nymph Wings
                { BACK, 223, 1, 220000, 0 }, // Butterfly Wings
                { BACK, 224, 1, 280000, 0 }, // Little Angel Wings
                { BACK, 225, 1, 280000, 0 }, // Little Devil Wings
                { BACK, 227, 1, 450000, 0 }, // Angel Wings
                { BACK, 228, 1, 450000, 0 }, // Devil Wings
                { BACK, 231, 1, 550000, 0 }, // Worm Dragon Wings
                { BACK, 241, 1, 120000, 0 }, // Turtle Backshield
                { BACK, 244, 1, 160000, 0 }, // Guard Backshield
                { BACK, 245, 1, 250000, 0 }, // Lion Backshield
                { BACK, 202, 1,  35000, 0 }, // Leather Bag
                { BACK, 206, 1,  50000, 0 }, // Cutie Bag
                { BACK, 210, 1,  80000, 0 }, // Tamiya Bag
                { BACK, 211, 1, 110000, 0 }  // Ferrell Bag
            };
            break;

        case VEND_CAT_PAT:
            // Cart & Castle Gear Parts (Map 2: Mechanic_Torque)
            stock = {
                { PAT,   1, 1,  60000, 0 }, // Woodlive Frame
                { PAT,   2, 1, 120000, 0 }, // Wood Carbon Frame
                { PAT,   3, 1, 200000, 0 }, // Corman Di Frame
                { PAT,   5, 1, 350000, 0 }, // Sporty Frame
                { PAT,  31, 1,  40000, 0 }, // Wood Stock Core
                { PAT,  32, 1,  80000, 0 }, // Hard Stock Core
                { PAT,  33, 1, 150000, 0 }, // Basilisk Core
                { PAT,  35, 1, 280000, 0 }, // Saint Core
                { PAT, 101, 1,  50000, 0 }, // First Engine
                { PAT, 102, 1, 110000, 0 }, // Second Engine
                { PAT, 103, 1, 220000, 0 }  // Dural Engine
            };
            break;

        case VEND_CAT_DUAL_KATARS:
            // Katars & Dual Blades (Map 2: Dealer_Vance)
            stock = {
                { WEAPON, 403, 1,  65000, 4 }, // Katar +4
                { WEAPON, 409, 1, 120000, 5 }, // Dual Patar +5
                { WEAPON, 411, 1, 190000, 6 }, // Assassin Katar +6
                { WEAPON, 433, 1, 140000, 4 }, // Dual Bushido +4
                { WEAPON, 436, 1, 180000, 5 }, // Dual Ocean Swords +5
                { WEAPON, 439, 1, 240000, 5 }, // Dual Katana +5
                { WEAPON, 441, 1, 300000, 6 }, // Dual Flare Swords +6
                { WEAPON, 448, 1, 380000, 7 }, // Dual Panther Blades +7
                { WEAPON, 449, 1, 450000, 7 }  // Dual Blades of Saint Kreuz +7
            };
            break;

        case VEND_CAT_ZANT_STARTER:
            // Zant Starter Consumables (Map 1: Vendor_Pippin)
            stock = {
                { CONSUMIBLE,   1, 999,  100, 0 }, // Health Vial (S)
                { CONSUMIBLE,   2, 999,  250, 0 }, // Health Vial (M)
                { CONSUMIBLE,   3, 999,  500, 0 }, // Health Vial (L)
                { CONSUMIBLE,   7, 999,  150, 0 }, // Herbal Medicine (S)
                { CONSUMIBLE,  21, 999,  150, 0 }, // Mana Vial (S)
                { CONSUMIBLE,  22, 999,  350, 0 }, // Mana Vial (M)
                { CONSUMIBLE,  23, 999,  700, 0 }, // Mana Vial (L)
                { CONSUMIBLE, 350, 999,  400, 0 }, // Adventure Plains Return Scroll
                { CONSUMIBLE, 351, 999,  600, 0 }, // Zant Return Scroll
                { CONSUMIBLE, 306, 999, 3000, 0 }, // HP Scroll (Solo)
                { CONSUMIBLE, 307, 999, 3000, 0 }, // MP Scroll (Solo)
                { CONSUMIBLE, 308, 999, 4000, 0 }, // Dexterity Scroll (Solo)
                { CONSUMIBLE, 309, 999, 4000, 0 }  // Strength Scroll (Solo)
            };
            break;

        case VEND_CAT_ZANT_WEAPONS:
            // Zant Weapons (Map 1: Smith_Brant)
            stock = {
                { WEAPON,     1, 1,  1000, 0 }, // Wooden Sword
                { WEAPON,     2, 1,  3500, 1 }, // Short Sword +1
                { WEAPON,     3, 1,  8000, 2 }, // Rapier +2
                { WEAPON,     4, 1, 15000, 3 }, // Khukuri +3
                { WEAPON,     5, 1, 25000, 4 }, // Long Sword +4
                { WEAPON,   201, 1,  1500, 0 }, // Toy Bow
                { WEAPON,   202, 1,  5000, 1 }, // Short Bow +1
                { WEAPON,   203, 1, 12000, 2 }, // Long Bow +2
                { WEAPON,   301, 1,  2000, 0 }, // Baobab Rod
                { WEAPON,   302, 1,  6000, 1 }, // Lemmings Rod +1
                { WEAPON,   303, 1, 14000, 2 }, // Animal Rod +2
                { WEAPON,    41, 1,  4000, 1 }, // Pony Hammer +1
                { SUBWEAPON,  1, 1,  1500, 0 }, // Wooden Shield
                { SUBWEAPON,  3, 1,  6000, 2 }  // Round Shield +2
            };
            break;

        case VEND_CAT_ZANT_ARMOR:
            // Zant Armor & Sets (Map 1: Tailor_Lydia)
            stock = {
                { CAP,       31, 1, 1500, 0 }, // Novice Cap
                { BODY,      31, 1, 3000, 0 }, // Novice Suit
                { GLOVE,     31, 1, 1200, 0 }, // Novice Gloves
                { SHOE,      31, 1, 1200, 0 }, // Novice Shoes
                { CAP,       32, 1, 4000, 2 }, // Leather Cap +2
                { BODY,      32, 1, 8000, 2 }, // Leather Armor +2
                { GLOVE,     32, 1, 3000, 2 }, // Leather Gloves +2
                { SHOE,      32, 1, 3000, 2 }, // Leather Boots +2
                { SUBWEAPON,  1, 1, 1500, 0 }, // Wooden Shield
                { SUBWEAPON,  3, 1, 6000, 2 }  // Round Shield +2
            };
            break;

        case VEND_CAT_ZANT_GEMS:
            // Zant Cut Gems & Talismans (Map 1: GemTrader_Ruby)
            stock = {
                { JEM,     301, 999,  2000, 0 }, // Garnet [1]
                { JEM,     302, 999,  5000, 0 }, // Garnet [2]
                { JEM,     303, 999, 12000, 0 }, // Garnet [3]
                { JEM,     311, 999,  3000, 0 }, // Ruby [1]
                { JEM,     312, 999,  7000, 0 }, // Ruby [2]
                { JEM,     313, 999, 15000, 0 }, // Ruby [3]
                { JEM,     321, 999,  3000, 0 }, // Sapphire [1]
                { JEM,     322, 999,  7000, 0 }, // Sapphire [2]
                { JEM,     323, 999, 15000, 0 }, // Sapphire [3]
                { JEM,     331, 999,  3000, 0 }, // Topaz [1]
                { JEM,     332, 999,  7000, 0 }, // Topaz [2]
                { JEM,     333, 999, 15000, 0 }, // Topaz [3]
                { NATURAL,  71, 999,  2000, 0 }, // 1st Grade Talisman
                { NATURAL,  72, 999,  5000, 0 }, // 2nd Grade Talisman
                { NATURAL,  73, 999, 10000, 0 }, // 3rd Grade Talisman
                { NATURAL,  81, 999,  1500, 0 }, // 1st Grade Bindrune
                { NATURAL,  82, 999,  3500, 0 }, // 2nd Grade Bindrune
                { NATURAL,  83, 999,  7000, 0 }  // 3rd Grade Bindrune
            };
            break;

        case VEND_CAT_ZANT_MATERIALS:
            // Zant Crafting Materials (Map 1: Peddler_Toby)
            stock = {
                { NATURAL,   1, 999,   300, 0 }, // Rusted Iron
                { NATURAL,   5, 999,  1200, 0 }, // Iron
                { NATURAL,  41, 999,   400, 0 }, // Old Leather
                { NATURAL,  42, 999,   900, 0 }, // Thin Leather
                { NATURAL, 111, 999,   800, 0 }, // Iron Thread
                { NATURAL, 181, 999,   300, 0 }, // Flower Pollen
                { NATURAL, 196, 999,   500, 0 }, // Bird Feather
                { NATURAL, 201, 999,  2500, 0 }, // Steam Oil
                { NATURAL, 261, 999,  1500, 0 }, // Wolf Claw
                { NATURAL, 275, 999,  2000, 0 }  // Spider Web
            };
            break;

        case VEND_CAT_ZANT_ACCESSORIES:
            // Zant Accessories & Bags (Map 1: Collector_Felix)
            stock = {
                { JEWEL,   1, 1,  5000, 0 }, // Sparkling Ring
                { JEWEL,   2, 1,  8000, 0 }, // Heavy Ring
                { JEWEL,   3, 1,  4000, 0 }, // Dim Ring
                { JEWEL,   4, 1,  6000, 0 }, // Simple Ring
                { JEWEL,  10, 1, 15000, 0 }, // Sturdy Ring
                { BACK,  202, 1, 25000, 0 }, // Leather Bag
                { BACK,  203, 1, 35000, 0 }, // Wooden Chest
                { BACK,  206, 1, 45000, 0 }  // Cutie Bag
            };
            break;

        case VEND_CAT_ZANT_AMMO:
            // Zant Starter Ammunition (Map 1: Scout_Robin)
            stock = {
                { NATURAL, 301, 999,  1, 0 }, // Wooden Arrow
                { NATURAL, 302, 999,  2, 0 }, // Bone Arrow
                { NATURAL, 303, 999,  3, 0 }, // Stone Arrow
                { NATURAL, 305, 999,  5, 0 }, // Metal Arrow
                { NATURAL, 321, 999,  1, 0 }, // Bean Bullet
                { NATURAL, 322, 999,  3, 0 }, // Gun Bullet
                { NATURAL, 323, 999,  5, 0 }, // Lead Bullet
                { NATURAL, 341, 999, 10, 0 }, // Cannon Shell
                { NATURAL, 342, 999, 18, 0 }  // Lead Shell
            };
            break;

        default:
            return false;
    }

    if ( stock.empty() ) return false;

    // Populate inventory slots 12 to 12 + stock.size()
    UINT numItems = (UINT)stock.size();
    if ( numItems > 30 ) numItems = 30;

    bot->Shop->Selling = numItems;
    bot->Shop->Buying = 0;
    bot->Shop->open = true;
    bot->Shop->ShopType = 0;

    for ( UINT i = 0; i < numItems; i++ )
    {
        const SVendingItemDef& def = stock[i];
        UINT slot = 12 + i;

        bot->items[slot].Clear();
        bot->items[slot].itemtype   = def.itemType;
        bot->items[slot].itemnum    = def.itemNum;
        bot->items[slot].count      = def.count;
        bot->items[slot].durability = 50;
        bot->items[slot].lifespan   = 100;
        bot->items[slot].appraised  = true;
        bot->items[slot].refine     = def.refine;
        bot->items[slot].socketed   = ( def.refine >= 7 );

        bot->Shop->SellingList[i].slot  = slot;
        bot->Shop->SellingList[i].count = def.count;
        bot->Shop->SellingList[i].price = def.price;
    }

    return true;
}

const char* GetVendingPromoMessage( int category )
{
    switch ( category )
    {
        case VEND_CAT_WEAPONS_HIGH:
            return "Finest quality refined swords, bows and guns for sale! Check my wares!";
        case VEND_CAT_ARMOR_HIGH:
            return "Solid Knight plates, holy armor sets and shields available right here!";
        case VEND_CAT_GEMS:
            return "High-tier cut gems! T4-T7 Diamonds, Rubies & Sapphires in bulk!";
        case VEND_CAT_REFINE:
            return "Grade 4-10 Talismans, Bindrunes and Planetary Runes for all your refines!";
        case VEND_CAT_MATERIALS:
            return "Top-grade crafting ores, leathers, spirit stones & mob drops!";
        case VEND_CAT_POTIONS_SCROLLS:
            return "Health & Mana bottles, town return scrolls and advanced stat scrolls!";
        case VEND_CAT_AMMO:
            return "Heavy ammunition supplies! Elemental arrows, bullets & cannon shells!";
        case VEND_CAT_ACCESSORIES:
            return "Stat boost rings, necklaces and earrings! Boost your attributes!";
        case VEND_CAT_WINGS:
            return "Fairy, Angel & Devil wings with speed boosts! Backbags & shields!";
        case VEND_CAT_PAT:
            return "Cart frames, high-output engines and durable cores in stock!";
        case VEND_CAT_DUAL_KATARS:
            return "Assassin Katars and dual weapons! High critical & attack speed!";
        case VEND_CAT_ZANT_STARTER:
            return "Starter potions, heal vials and return scrolls for new adventurers!";
        case VEND_CAT_ZANT_WEAPONS:
            return "Affordable beginner weapons, swords, rods and bows!";
        case VEND_CAT_ZANT_ARMOR:
            return "Novice and leather armor sets! Stay protected out there!";
        case VEND_CAT_ZANT_GEMS:
            return "Cut starter gems and early talismans for gear upgrades!";
        case VEND_CAT_ZANT_MATERIALS:
            return "Fresh mob drops, leathers and iron pieces ready for crafting!";
        case VEND_CAT_ZANT_ACCESSORIES:
            return "Sparkling rings and leather bags! Extra carrying capacity!";
        case VEND_CAT_ZANT_AMMO:
            return "Cheap hunting arrows and bullets for young Hawkers and Dealers!";
        default:
            return "Welcome! Take a look at my shop!";
    }
}

static void GetVendingBuyPool( int category, std::vector<SBuyItemDef>& popular, std::vector<SBuyItemDef>& variablePool )
{
    popular.clear( );
    variablePool.clear( );

    switch ( category )
    {
        case VEND_CAT_WEAPONS_HIGH:
            popular = {
                { WEAPON, 101, 1,  65000 }, // Haedong Sword
                { WEAPON, 103, 1, 120000 }, // Sword of Hardship
                { WEAPON, 121, 1, 200000 }, // Caliburn
                { WEAPON, 221, 1, 110000 }, // Centauri Bow
                { WEAPON, 241, 1, 160000 }, // Justice Cannon
                { WEAPON, 305, 1, 140000 }  // White Staff
            };
            variablePool = {
                { WEAPON, 123, 1, 140000 }, // Bastard Sword
                { WEAPON,  23, 1, 175000 }, // Blade of Saint Kreuz
                { WEAPON,  27, 1, 250000 }, // Death Bringer
                { WEAPON, 223, 1, 210000 }, // Infernal Bow
                { WEAPON, 243, 1, 220000 }, // Piercing Gun
                { WEAPON, 303, 1,  90000 }, // Animal Rod
                { WEAPON,  45, 1, 130000 }, // Great Hammer
                { WEAPON, 188, 1, 190000 }  // Fury Spantun
            };
            break;

        case VEND_CAT_ARMOR_HIGH:
            popular = {
                { BODY,       29, 1,  35000 }, // Nomadic Suit
                { BODY,       34, 1,  85000 }, // Knight Plate
                { BODY,       37, 1, 140000 }, // Holy Body
                { BODY,       66, 1,  75000 }, // Mage Robe
                { BODY,       71, 1, 110000 }, // Cleric Robe
                { BODY,      126, 1, 125000 }, // Raider Armor
                { SUBWEAPON,  10, 1,  95000 }  // Plate Shield
            };
            variablePool = {
                { CAP,        34, 1,  38000 }, // Knight Helm
                { GLOVE,      34, 1,  28000 }, // Knight Gauntlets
                { SHOE,       34, 1,  28000 }, // Knight Boots
                { CAP,        37, 1,  65000 }, // Holy Helm
                { GLOVE,      37, 1,  48000 }, // Holy Gauntlets
                { SHOE,       37, 1,  48000 }, // Holy Boots
                { SUBWEAPON,   7, 1,  42000 }  // Kite Shield
            };
            break;

        case VEND_CAT_GEMS:
            popular = {
                { JEM, 314, 100,  38000 }, // Ruby [4]
                { JEM, 315,  50,  90000 }, // Ruby [5]
                { JEM, 324, 100,  38000 }, // Sapphire [4]
                { JEM, 325,  50,  90000 }, // Sapphire [5]
                { JEM, 334, 100,  38000 }, // Topaz [4]
                { JEM, 335,  50,  90000 }, // Topaz [5]
                { JEM, 364,  50,  58000 }, // Diamond [4]
                { JEM, 365,  25, 140000 }  // Diamond [5]
            };
            variablePool = {
                { JEM, 316,  20, 230000 }, // Ruby [6]
                { JEM, 326,  20, 230000 }, // Sapphire [6]
                { JEM, 336,  20, 230000 }, // Topaz [6]
                { JEM, 354, 100,  38000 }, // Peridot [4]
                { JEM, 355,  50,  90000 }, // Peridot [5]
                { JEM, 366,  15, 350000 }  // Diamond [6]
            };
            break;

        case VEND_CAT_REFINE:
            popular = {
                { NATURAL,  74, 200,  11000 }, // 4th Grade Talisman
                { NATURAL,  75, 100,  22000 }, // 5th Grade Talisman
                { NATURAL,  76,  50,  45000 }, // 6th Grade Talisman
                { NATURAL,  84, 200,   7500 }, // 4th Grade Bindrune
                { NATURAL,  85, 100,  15000 }, // 5th Grade Bindrune
                { NATURAL, 445,  50,  60000 }, // Venurune
                { NATURAL, 446,  50,  90000 }  // Mercurune
            };
            variablePool = {
                { NATURAL,  77,  30,  90000 }, // 7th Grade Talisman
                { NATURAL,  78,  15, 180000 }, // 8th Grade Talisman
                { NATURAL,  86,  50,  30000 }, // 6th Grade Bindrune
                { NATURAL,  87,  30,  60000 }, // 7th Grade Bindrune
                { NATURAL, 375,  50,  22000 }, // 5th Grade Apotrope
                { NATURAL, 376,  30,  45000 }, // 6th Grade Apotrope
                { NATURAL, 456,  30, 110000 }, // Nepturune
                { NATURAL, 448,  20, 150000 }, // Jupiterune
                { NATURAL, 449,  15, 190000 }  // Saturune
            };
            break;

        case VEND_CAT_MATERIALS:
            popular = {
                { NATURAL,   5, 500,   1100 }, // Iron
                { NATURAL,   7, 300,   4500 }, // Steel
                { NATURAL,  43, 500,   1500 }, // Smooth Leather
                { NATURAL,  44, 300,   2200 }, // Rough Leather
                { NATURAL, 112, 300,   2200 }, // Silver Thread
                { NATURAL, 113, 200,   4500 }, // Gold Thread
                { NATURAL, 261, 500,   1800 }, // Wolf Claw
                { NATURAL, 295, 100,  15000 }, // Water Spirit Stone
                { NATURAL, 298, 100,  15000 }  // Fire Spirit Stone
            };
            variablePool = {
                { NATURAL,   6, 400,   2600 }, // Silver Iron
                { NATURAL,  10, 150,  11000 }, // Damascus
                { NATURAL,  45, 200,   3800 }, // Soft Leather
                { NATURAL, 114, 100,   9000 }, // Platinum Thread
                { NATURAL, 196, 500,    750 }, // Bird Feather
                { NATURAL, 197, 300,   3000 }, // Fairy Powder
                { NATURAL, 201, 300,   2200 }, // Steam Oil
                { NATURAL, 205, 200,   3800 }, // Mana Oil
                { NATURAL, 275, 300,   2600 }, // Spider Web
                { NATURAL, 297, 100,  15000 }, // Earth Spirit Stone
                { NATURAL, 296, 100,  15000 }  // Wind Spirit Stone
            };
            break;

        case VEND_CAT_POTIONS_SCROLLS:
            popular = {
                { CONSUMIBLE,   5, 999,    450 }, // Health Bottle (M)
                { CONSUMIBLE,   6, 999,    900 }, // Health Bottle (L)
                { CONSUMIBLE,  25, 999,    600 }, // Mana Bottle (M)
                { CONSUMIBLE,  14, 999,    600 }, // Red Potion
                { CONSUMIBLE,  33, 999,    750 }, // Blue Potion
                { CONSUMIBLE, 351, 200,    750 }, // Zant Return Scroll
                { CONSUMIBLE, 352, 200,   1100 }, // Junon Polis Return Scroll
                { CONSUMIBLE, 963, 100,  13500 }  // Speed Booster
            };
            variablePool = {
                { CONSUMIBLE,  12, 300,   1800 }, // Vital Water (L)
                { CONSUMIBLE,  32, 200,   2600 }, // Spiritual Water (XL)
                { CONSUMIBLE,  81, 150,   3800 }, // Max MP Potion
                { CONSUMIBLE,  82, 200,   2200 }, // Purify Potion
                { CONSUMIBLE,  90, 100,   6000 }, // Attack Speed Potion
                { CONSUMIBLE, 353, 100,   2200 }, // Eucar Return Scroll
                { CONSUMIBLE, 354, 100,   2200 }, // Xita Return Scroll
                { CONSUMIBLE, 313,  50,  11000 }, // Advanced HP Scroll
                { CONSUMIBLE, 314,  50,  11000 }  // Advanced MP Scroll
            };
            break;

        case VEND_CAT_AMMO:
            popular = {
                { NATURAL, 305, 999,   4 }, // Metal Arrow
                { NATURAL, 306, 999,   6 }, // Gilded Arrow
                { NATURAL, 323, 999,   6 }, // Lead Bullet
                { NATURAL, 325, 999,   9 }, // Silver Bullet
                { NATURAL, 341, 999,  11 }, // Cannon Shell
                { NATURAL, 342, 999,  19 }  // Lead Shell
            };
            variablePool = {
                { NATURAL, 307, 999,   9 }, // Platinum Arrow
                { NATURAL, 311, 999,  11 }, // Fire Arrow
                { NATURAL, 316, 999,  15 }, // Lightning Arrow
                { NATURAL, 322, 999,   4 }, // Gun Bullet
                { NATURAL, 326, 999,  12 }, // Gold Bullet
                { NATURAL, 331, 999,  15 }, // Fire Bullet
                { NATURAL, 344, 999,  30 }, // Bumper Shell
                { NATURAL, 345, 999,  38 }  // Fire Shell
            };
            break;

        case VEND_CAT_ACCESSORIES:
            popular = {
                { JEWEL, 11, 1, 33000 }, // Ring of Strength
                { JEWEL, 12, 1, 42000 }, // Ring of Dexterity
                { JEWEL, 13, 1, 38000 }, // Ring of Wisdom
                { JEWEL, 22, 1, 58000 }, // Ruby Necklace
                { JEWEL, 33, 1, 52000 }  // Gold Earrings
            };
            variablePool = {
                { JEWEL, 14, 1, 30000 }, // Ring of Health
                { JEWEL, 15, 1, 45000 }, // Ring of Critical
                { JEWEL, 23, 1, 58000 }, // Sapphire Necklace
                { JEWEL, 24, 1, 58000 }, // Emerald Necklace
                { JEWEL, 25, 1, 70000 }, // Diamond Necklace
                { JEWEL, 32, 1, 30000 }, // Silver Earrings
                { JEWEL, 34, 1, 90000 }  // Platinum Earrings
            };
            break;

        case VEND_CAT_WINGS:
            popular = {
                { BACK, 221, 1, 110000 }, // Fairy Wings
                { BACK, 222, 1, 135000 }, // Nymph Wings
                { BACK, 227, 1, 340000 }, // Angel Wings
                { BACK, 228, 1, 340000 }, // Devil Wings
                { BACK, 206, 1,  38000 }  // Cutie Bag
            };
            variablePool = {
                { BACK, 223, 1, 165000 }, // Butterfly Wings
                { BACK, 224, 1, 210000 }, // Little Angel Wings
                { BACK, 225, 1, 210000 }, // Little Devil Wings
                { BACK, 231, 1, 420000 }, // Worm Dragon Wings
                { BACK, 241, 1,  90000 }, // Turtle Backshield
                { BACK, 244, 1, 120000 }, // Guard Backshield
                { BACK, 211, 1,  85000 }  // Ferrell Bag
            };
            break;

        case VEND_CAT_PAT:
            popular = {
                { PAT,   1, 1,  45000 }, // Woodlive Frame
                { PAT,   5, 1, 260000 }, // Sporty Frame
                { PAT,  31, 1,  30000 }, // Wood Stock Core
                { PAT, 101, 1,  38000 }, // First Engine
                { PAT, 102, 1,  85000 }  // Second Engine
            };
            variablePool = {
                { PAT,   2, 1,  90000 }, // Wood Carbon Frame
                { PAT,   3, 1, 150000 }, // Corman Di Frame
                { PAT,  32, 1,  60000 }, // Hard Stock Core
                { PAT,  33, 1, 110000 }, // Basilisk Core
                { PAT,  35, 1, 210000 }, // Saint Core
                { PAT, 103, 1, 165000 }  // Dural Engine
            };
            break;

        case VEND_CAT_DUAL_KATARS:
            popular = {
                { WEAPON, 403, 1,  50000 }, // Katar
                { WEAPON, 411, 1, 140000 }, // Assassin Katar
                { WEAPON, 433, 1, 105000 }, // Dual Bushido
                { WEAPON, 439, 1, 180000 }, // Dual Katana
                { WEAPON, 448, 1, 280000 }  // Dual Panther Blades
            };
            variablePool = {
                { WEAPON, 409, 1,  90000 }, // Dual Patar
                { WEAPON, 436, 1, 135000 }, // Dual Ocean Swords
                { WEAPON, 441, 1, 220000 }, // Dual Flare Swords
                { WEAPON, 449, 1, 340000 }  // Dual Blades of Saint Kreuz
            };
            break;

        case VEND_CAT_ZANT_STARTER:
            popular = {
                { CONSUMIBLE,   1, 999,   75 }, // Health Vial (S)
                { CONSUMIBLE,   2, 999,  180 }, // Health Vial (M)
                { CONSUMIBLE,   3, 999,  380 }, // Health Vial (L)
                { CONSUMIBLE,  21, 999,  110 }, // Mana Vial (S)
                { CONSUMIBLE,  22, 999,  260 }, // Mana Vial (M)
                { CONSUMIBLE, 351, 200,  450 }  // Zant Return Scroll
            };
            variablePool = {
                { CONSUMIBLE,   7, 300,  110 }, // Herbal Medicine (S)
                { CONSUMIBLE,  23, 500,  520 }, // Mana Vial (L)
                { CONSUMIBLE, 350, 100,  300 }, // Adventure Plains Return Scroll
                { CONSUMIBLE, 306,  50, 2200 }, // HP Scroll (Solo)
                { CONSUMIBLE, 307,  50, 2200 }, // MP Scroll (Solo)
                { CONSUMIBLE, 308,  50, 3000 }, // Dexterity Scroll (Solo)
                { CONSUMIBLE, 309,  50, 3000 }  // Strength Scroll (Solo)
            };
            break;

        case VEND_CAT_ZANT_WEAPONS:
            popular = {
                { WEAPON,     1, 1,   750 }, // Wooden Sword
                { WEAPON,     2, 1,  2600 }, // Short Sword
                { WEAPON,     5, 1, 18000 }, // Long Sword
                { WEAPON,   202, 1,  3800 }, // Short Bow
                { WEAPON,   301, 1,  1500 }, // Baobab Rod
                { SUBWEAPON,  3, 1,  4500 }  // Round Shield
            };
            variablePool = {
                { WEAPON,     3, 1,  6000 }, // Rapier
                { WEAPON,     4, 1, 11000 }, // Khukuri
                { WEAPON,   201, 1,  1100 }, // Toy Bow
                { WEAPON,   203, 1,  9000 }, // Long Bow
                { WEAPON,   302, 1,  4500 }, // Lemmings Rod
                { WEAPON,   303, 1, 10000 }, // Animal Rod
                { WEAPON,    41, 1,  3000 }, // Pony Hammer
                { SUBWEAPON,  1, 1,  1100 }  // Wooden Shield
            };
            break;

        case VEND_CAT_ZANT_ARMOR:
            popular = {
                { CAP,       31, 1, 1100 }, // Novice Cap
                { BODY,      31, 1, 2200 }, // Novice Suit
                { CAP,       32, 1, 3000 }, // Leather Cap
                { BODY,      32, 1, 6000 }  // Leather Armor
            };
            variablePool = {
                { GLOVE,     31, 1,  900 }, // Novice Gloves
                { SHOE,      31, 1,  900 }, // Novice Shoes
                { GLOVE,     32, 1, 2200 }, // Leather Gloves
                { SHOE,      32, 1, 2200 }, // Leather Boots
                { SUBWEAPON,  3, 1, 4500 }  // Round Shield
            };
            break;

        case VEND_CAT_ZANT_GEMS:
            popular = {
                { JEM,     301, 100,  1500 }, // Garnet [1]
                { JEM,     302,  50,  3800 }, // Garnet [2]
                { JEM,     311, 100,  2200 }, // Ruby [1]
                { JEM,     312,  50,  5200 }, // Ruby [2]
                { JEM,     321, 100,  2200 }, // Sapphire [1]
                { JEM,     322,  50,  5200 }, // Sapphire [2]
                { NATURAL,  71, 200,  1500 }, // 1st Grade Talisman
                { NATURAL,  72, 100,  3800 }  // 2nd Grade Talisman
            };
            variablePool = {
                { JEM,     303,  25,  9000 }, // Garnet [3]
                { JEM,     313,  25, 11000 }, // Ruby [3]
                { JEM,     323,  25, 11000 }, // Sapphire [3]
                { JEM,     331, 100,  2200 }, // Topaz [1]
                { JEM,     332,  50,  5200 }, // Topaz [2]
                { JEM,     333,  25, 11000 }, // Topaz [3]
                { NATURAL,  73,  50,  7500 }, // 3rd Grade Talisman
                { NATURAL,  81, 200,  1100 }, // 1st Grade Bindrune
                { NATURAL,  82, 100,  2600 }, // 2nd Grade Bindrune
                { NATURAL,  83,  50,  5200 }  // 3rd Grade Bindrune
            };
            break;

        case VEND_CAT_ZANT_MATERIALS:
            popular = {
                { NATURAL,   1, 500,   220 }, // Rusted Iron
                { NATURAL,   5, 500,   900 }, // Iron
                { NATURAL,  41, 500,   300 }, // Old Leather
                { NATURAL,  42, 300,   650 }, // Thin Leather
                { NATURAL, 111, 300,   600 }, // Iron Thread
                { NATURAL, 261, 500,  1100 }  // Wolf Claw
            };
            variablePool = {
                { NATURAL, 181, 500,   220 }, // Flower Pollen
                { NATURAL, 196, 500,   380 }, // Bird Feather
                { NATURAL, 201, 200,  1800 }, // Steam Oil
                { NATURAL, 275, 300,  1500 }  // Spider Web
            };
            break;

        case VEND_CAT_ZANT_ACCESSORIES:
            popular = {
                { JEWEL,   1, 1,  3800 }, // Sparkling Ring
                { JEWEL,   2, 1,  6000 }, // Heavy Ring
                { BACK,  202, 1, 18000 }, // Leather Bag
                { BACK,  206, 1, 34000 }  // Cutie Bag
            };
            variablePool = {
                { JEWEL,   3, 1,  3000 }, // Dim Ring
                { JEWEL,   4, 1,  4500 }, // Simple Ring
                { JEWEL,  10, 1, 11000 }, // Sturdy Ring
                { BACK,  203, 1, 26000 }  // Wooden Chest
            };
            break;

        case VEND_CAT_ZANT_AMMO:
            popular = {
                { NATURAL, 301, 999,   1 }, // Wooden Arrow
                { NATURAL, 302, 999,   1 }, // Bone Arrow
                { NATURAL, 303, 999,   2 }, // Stone Arrow
                { NATURAL, 321, 999,   1 }, // Bean Bullet
                { NATURAL, 322, 999,   2 }, // Gun Bullet
                { NATURAL, 341, 999,   7 }  // Cannon Shell
            };
            variablePool = {
                { NATURAL, 305, 999,   4 }, // Metal Arrow
                { NATURAL, 323, 999,   4 }, // Lead Bullet
                { NATURAL, 342, 999,  13 }  // Lead Shell
            };
            break;

        default:
            break;
    }
}

bool PopulateVendingBuyList( CPlayer* bot, int category )
{
    if ( !bot || !bot->Shop ) return false;

    for ( int i = 0; i < 30; i++ )
    {
        bot->Shop->BuyingList[i].slot = 0;
        bot->Shop->BuyingList[i].count = 0;
        bot->Shop->BuyingList[i].price = 0;
        bot->Shop->BuyingList[i].item.Clear( );
    }

    std::vector<SBuyItemDef> popular;
    std::vector<SBuyItemDef> variablePool;

    GetVendingBuyPool( category, popular, variablePool );

    if ( popular.empty( ) && variablePool.empty( ) ) return false;

    std::vector<SBuyItemDef> selected;
    for ( size_t i = 0; i < popular.size( ); i++ )
    {
        selected.push_back( popular[i] );
    }

    if ( !variablePool.empty( ) )
    {
        std::vector<SBuyItemDef> pool = variablePool;
        std::random_shuffle( pool.begin( ), pool.end( ) );
        size_t toAdd = 3 + ( rand( ) % 3 ); // Pick 3, 4, or 5 variable items
        if ( toAdd > pool.size( ) ) toAdd = pool.size( );
        for ( size_t i = 0; i < toAdd; i++ )
        {
            selected.push_back( pool[i] );
        }
    }

    UINT numBuy = (UINT)selected.size( );
    if ( numBuy > 30 ) numBuy = 30;

    bot->Shop->Buying = numBuy;
    bot->CharInfo->Zulies = 500000000; // Give bot 500M Zulies to purchase items from players

    for ( UINT i = 0; i < numBuy; i++ )
    {
        const SBuyItemDef& def = selected[i];
        bot->Shop->BuyingList[i].slot  = i;
        bot->Shop->BuyingList[i].count = def.count;
        bot->Shop->BuyingList[i].price = def.price;

        bot->Shop->BuyingList[i].item.Clear( );
        bot->Shop->BuyingList[i].item.itemtype   = def.itemType;
        bot->Shop->BuyingList[i].item.itemnum    = def.itemNum;
        bot->Shop->BuyingList[i].item.count      = def.count;
        bot->Shop->BuyingList[i].item.durability = 50;
        bot->Shop->BuyingList[i].item.lifespan   = 100;
        bot->Shop->BuyingList[i].item.appraised  = true;
    }

    return true;
}
