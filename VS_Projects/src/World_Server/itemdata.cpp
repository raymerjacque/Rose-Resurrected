// Added By Raven0123

#include "datatypes.h"

unsigned CItem::GetVisInfo( )
{
	if ( count == 0 ) Clear( );
	return ( ( refine >> 4 ) << 20 ) | ( socketed << 19 ) | ( stats << 10 ) | itemnum;
}

unsigned short CItem::GetPakHeader( )
{
	if ( count == 0 ) Clear( );
	return ( ( itemnum << 5 ) | ( itemtype ) );
}

unsigned CItem::GetPakData( )
{
	unsigned retval = 0;
	if ( count == 0 ) Clear( );
	if ( itemtype >= 10 && itemtype <= 13 || itemtype == 0x1f ) {
		retval = count;
	} else {
		retval |= (refine>>4) << 28;
		retval |= (appraised&1) << 27;
		retval |= (socketed&1) << 26;
		retval |= (lifespan*10) << 16;
		retval |= durability << 9;
		retval |= stats;
	}
	return retval;
}

void CItem::Clear( )
{
	itemnum = 0;
	itemtype = 0;
	count = 0;
	refine = 0;
	appraised = 0;
	socketed = 0;
	lifespan = 0;
	durability = 0;
	stats = 0;
}

bool CItem::isValid () const {
	/// A couple simple checks that are telling us if the item is valid:
	// FK: Default Return value is true until we prove otherwise below.
	if (this->count <= 0) { return false; }
	if (this->usetype == 0) 
	{
		// this really should force an error, but man.... so many items created without type !
	}

	// Item type checks 
	// Watch out: Money is 0x1f (31);so check for this first, if not money, the use normal checks!
	if (this->itemtype != eItemType::Money) 
	{
		if (this->itemtype <= eItemType::UnknownType || this->itemtype > eItemType::_MaxTypeCount) { return false; }
	}

	// Count vs Stackable check
	if (this->isStackable ()) 
	{
		if (this->count > this->MAX_STACK_SIZE) { return false; }
	}
	else 
	{
		if (this->count > 1) { return false; }
	}

	// Well if we made it here, it must be true ;-)
	return true;
}

/// Public Method Interface.
bool CItem::isStackable () const {
	/// FK: Tells you if the Item is stackable or not.
	return (this->itemtype == eItemType::Consumable ||
		this->itemtype == eItemType::Gem ||
		this->itemtype == eItemType::Materials ||
		this->itemtype == eItemType::QuestItem
		);
};

void CItem::_Init () 
{
	/// FK: Hidden method; it's used by the class internally to make sure all properties are initialized;
	this->itemtype		= eItemType::UnknownType;
	this->itemnum		= 0;
	this->restriction	= 0;
	this->usetype		= 0;
	this->isCreated		= 0;
	this->gem			= 0;
	this->durability	= 0;
	this->lifespan		= 99;
	this->socketed		= false;
	this->appraised		= false;
	this->weight		= 0;
	this->quality		= 0;
	this->breakid		= 0;
	this->craftskill	= 0;
	this->craftlevel	= 0;
	this->craftrecipe	= 0;
	this->craftdiff		= 0;
	this->craftlevel	= 0;
	this->craftdiff		= 0;
	this->count			= 0;
	this->stats			= 0;
	this->refine		= 0;
	this->itemgrade		= 0;
	this->durabLeft		= 0;
	this->sp_value		= 0;
	this->last_sp_value = 0;
	this->raretype		= 0;
	this->refineitem	= 0;
	this->pricerate		= 0;
	this->dodge			= 0;
	this->Stackable		= false;
}

void CItem::_Refresh () 
{
	if (this->_initphase) return;

	this->Stackable = this->isStackable ();
	this->InventoryTabID = this->GetInventoryTab ();
	this->Family = this->_getFamily ();
}

ItemFamily CItem::_getFamily () 
{
	switch (this->itemtype) 
	{
	case eItemType::Face:
	case eItemType::Cap:
	case eItemType::Body:
	case eItemType::Glove:
	case eItemType::Shoe:
	case eItemType::Back:
	case eItemType::Weapon:
	case eItemType::SubWeapon:
		return ItemFamily::Equipment;

	case eItemType::Jewel:
		return ItemFamily::Jewelry;

	case eItemType::Consumable:	// Consumables
		return ItemFamily::Consumables;

	case eItemType::Mount:
		return ItemFamily::Unknown;

	case eItemType::Gem:
		return ItemFamily::Gems;

	case eItemType::Materials:
		return ItemFamily::Materials;

	case eItemType::CartCG:
		return ItemFamily::Pat;

	case eItemType::Money:
		return ItemFamily::Money;

	default:	// No Clue...
		return ItemFamily::Unknown;
		break;
	}
}

CItem::InventoryTab CItem::GetInventoryTab () {

	switch (this->itemtype) {
	case eItemType::Face:
	case eItemType::Cap:
	case eItemType::Body:
	case eItemType::Glove:
	case eItemType::Shoe:
	case eItemType::Back:
	case eItemType::Jewel:
	case eItemType::Weapon:
	case eItemType::SubWeapon:
		return InventoryTab::EquipmentTab;

	case eItemType::Consumable:	// Consumables
	case eItemType::Mount:
		return InventoryTab::ConsumableTab;

	case eItemType::Gem:
	case eItemType::Materials:
		return InventoryTab::MaterialsTab;

	case eItemType::CartCG:
		return InventoryTab::PATCGTab;

	default:	// No Clue...
		return InventoryTab::UnknownTab;
		break;
	}

	return CItem::InventoryTab::UnknownTab;
}




