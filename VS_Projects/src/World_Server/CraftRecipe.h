#pragma once
#include "../Common/Enums.h"
#include "../Common/TypeDef.h"
#include "datatypes.h"

#define NULL 0		//wasn't defined for some reason

//forward declare CItem
//class CItem;

//class CCraftIngredient 
//{
//public:
//	uint32 ItemID		= 0;							// Item ID including item type and item id
//	uint16 Count		= 0;
//	uint16 Type			= eType::tNone;
//	uint16 ItemType		= 0;							// Taken from ItemID
//	uint32 ItemNum		= 0;							// Taken from ItemID
//	bool   isGeneric	= false;						// Ingredient can be anything of a specific type;
//	bool   isSpecific	= false;
//
//	/// Constructor + Deconstructor
//	CCraftIngredient();
//	CCraftIngredient(uint32 TypeOrItem, uint16 count);
//
//	~CCraftIngredient() {};
//
//	/// Public Methods;
//	bool Validate	(CItem* Item) const;				// Check if an item matches this ingredient.
//	bool isValid    ();									// Check if the ingredient is valid;
//	bool PopulateFromItemID ();
//};
//
//class CCraftRecipe 
//{
//public:
//	uint16 Id		= 0;
//	uint16 Family[4];
//	uint16 NumberOfIngredients = 0;
//	CCraftIngredient*	ingredients[4];
//
//	CCraftRecipe(WORD ID) 
//	{
//		this->Id = ID;
//	};
//
//	CCraftRecipe ();
//	~CCraftRecipe() ;
//
//public:
//	bool isValid          ( );
//	bool CheckIngredients (CItem* Item1 = NULL, CItem* Item2 = NULL, CItem* Item3 = NULL, CItem* Item4 = NULL);			// FK: Validate the materials given agains the recipe.
//	CItem* GetItemFromID (uint32 itemId);
//};
