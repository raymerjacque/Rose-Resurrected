#ifndef _VENDING_CATALOG_H_
#define _VENDING_CATALOG_H_

#include "worldserver.h"
#include "player.h"

enum eVendingCategory {
    VEND_CAT_WEAPONS_HIGH = 1,
    VEND_CAT_ARMOR_HIGH,
    VEND_CAT_GEMS,
    VEND_CAT_REFINE,
    VEND_CAT_MATERIALS,
    VEND_CAT_POTIONS_SCROLLS,
    VEND_CAT_AMMO,
    VEND_CAT_ACCESSORIES,
    VEND_CAT_WINGS,
    VEND_CAT_PAT,
    VEND_CAT_DUAL_KATARS,
    VEND_CAT_ZANT_STARTER,
    VEND_CAT_ZANT_WEAPONS,
    VEND_CAT_ZANT_ARMOR,
    VEND_CAT_ZANT_GEMS,
    VEND_CAT_ZANT_MATERIALS,
    VEND_CAT_ZANT_ACCESSORIES,
    VEND_CAT_ZANT_AMMO
};

struct SVendingItemDef {
    UINT itemType;
    UINT itemNum;
    UINT count;
    DWORD price;
    UINT refine;
};

bool PopulateVendingInventory( CPlayer* bot, int category );
const char* GetVendingPromoMessage( int category );

#endif
