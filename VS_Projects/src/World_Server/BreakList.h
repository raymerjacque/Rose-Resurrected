#pragma once
/*---------------------------------------------------------------------------------------------------------
	Poject 137 Header File:

	File		: BreakList.h

	Purpose		: Class that handles LIST_BREAK.STB
	Code File	: BreakList.cpp


	Revision	:  WHO	  | WHEN			| WHAT

				:   FK		2018-11-08		Isolated cItem class to seperate files;
				
	Comments	: 

---------------------------------------------------------------------------------------------------------*/
//STDAFX	#include <vector>
//STDAFX	#include "TypeDef.h"
//STDAFX	#include "cLogServer.h"
#include "WorldServer.h"				// Include this, instead of STBHandler.h

//extern CLogServer* Logging;

class CBreakItem {

private:

public:
	/// Public Interface
	uint8  itemtype;            // Item Type
	uint32 itemid;              // Item ID
	uint32 minCount;            // Item Minimal
	uint32 maxCount;            // Item Maximum
	uint32 chance;              // Item Return Chance (in %)
	bool forced;                // Indicates if this item should always be returned, or not.
	
	/// Constructor
	CBreakItem() {		
		this->itemtype = 0;					
		this->itemid   = 0;					
		this->minCount = 0;
		this->maxCount = 0;
		this->chance   = 0;
		this->forced   = false;
	}

	/// Deconstructor
	~CBreakItem() {};	
};

class CBreakRecord {
private:
public:
	uint32					 id;             // Breaklist ID
	std::vector<CBreakItem*> Items;          // Vector that stores all items.
	uint32					 totalChance;
	uint32					 disMin;         // Minimal return amount
	uint32					 disMax;         // Maximum return amount;
	bool					 isValid;        // Is the record valid, or  not ?

	CBreakRecord() {
		this->id		  = 0;
		this->totalChance = 0;
		this->disMin	  = 0;
		this->disMax	  = 0;
		this->isValid	  = false;
		this->Items.clear();
	}
};