
#include "datatypes.h"
#include "../Common/Support.h"

#pragma region "----- Crafting Ingredient -------"

CCraftIngredient::CCraftIngredient() {
	this->Count		= 0;
	this->Type		= 0;
	this->ItemID	= 0;
	this->ItemType  = 0;
	this->ItemNum	= 0;
}


CCraftIngredient::CCraftIngredient(uint32 TypeOrItem, uint16 count) {
	if (TypeOrItem == 0) return;

	if (TypeOrItem < 1000) 
	{
		this->Type = TypeOrItem;
		this->isGeneric = true;
		this->isSpecific = false;
	} 
	else 
	{
		this->ItemNum = TypeOrItem;
		this->isGeneric = false;
		this->isSpecific = true;
	}

	this->Count = count;
}

bool CCraftIngredient::isValid() 
{
	return this->Count > 0 && (this->Type != 0 || this->ItemID != 0);
}

bool CCraftIngredient::PopulateFromItemID ()
{
	if (this->ItemID == 0)
		return false;
	this->ItemType	= Rose::Support::ParseItemCode (this->ItemID, Rose::Support::ParserOptions::ItemType);
	this->ItemNum	= Rose::Support::ParseItemCode (this->ItemID, Rose::Support::ParserOptions::ItemNo);
	this->Count		= 1;		//defaults to 1 on object creation. Changed later in startup loading function

	return true;
}

bool CCraftIngredient::Validate(CItem* Item ) const 
{
	if (Item == NULL) return false;
	if (Item->count < this->Count) return false;

	if (this->Type != 0) 
	{
		if (Item->usetype == this->Type) return true;
	} 
	else 
	{
		WORD itemType = Rose::Support::ParseItemCode(this->ItemID, Rose::Support::ParserOptions::ItemType);
		WORD itemNum = Rose::Support::ParseItemCode(this->ItemID, Rose::Support::ParserOptions::ItemNo);
		
		if (Item->itemnum == itemNum && Item->itemtype == itemType) return true;
	}

	return false;
}
#pragma endregion


#pragma region "----- Crafting Recipe -------"

//constructor
CCraftRecipe::CCraftRecipe ()
{
	this->Id = 0;
	this->NumberOfIngredients = 0;
	for (int i = 0; i < 4; i++)
	{ 
		this->ingredients[i].ItemID		= 0;
		this->ingredients[i].ItemNum	= 0;
		this->ingredients[i].ItemType	= 0;
	}
	for (int i = 0; i < 4; i++)
		this->Family[i] = 0;
}
/// Deconstructor;
CCraftRecipe::~CCraftRecipe() 
{
	this->Id = 0;
	this->NumberOfIngredients = 0;
	for (int i = 0; i < 4; i++)
	{
		this->ingredients[i].ItemID		= 0;
		this->ingredients[i].ItemNum	= 0;
		this->ingredients[i].ItemType	= 0;
	}
	for (int i = 0; i < 4; i++)
		this->Family[i] = 0;
}

bool CCraftRecipe::CheckIngredients(CItem* Item1, CItem* Item2, CItem* Item3, CItem* Item4) 
{
	WORD counter = 0;
	if (this->NumberOfIngredients > 0)													//Check the first item in the recipe
	{
		if (this->ingredients[0].ItemNum != Item1->itemnum)		{ return false; }		//not the right item
		if (this->ingredients[0].ItemType != Item1->itemtype)	{ return false; }		//not the right item
		if (this->ingredients[0].Count > Item1->count)			{ return false; }		//not enough ingredients
	} 
	if (this->NumberOfIngredients > 1)													//Check the second item in the recipe
	{
		if (this->ingredients[1].ItemNum != Item2->itemnum)		{ return false; }		//not the right item
		if (this->ingredients[1].ItemType != Item2->itemtype)	{ return false; }		//not the right item
		if (this->ingredients[1].Count > Item2->count)			{ return false; }		//not enough ingredients
	}
	if (this->NumberOfIngredients > 2)													//Check the third item in the recipe
	{
		if (this->ingredients[2].ItemNum != Item3->itemnum)		{ return false; }		//not the right item
		if (this->ingredients[2].ItemType != Item3->itemtype)	{ return false; }		//not the right item
		if (this->ingredients[2].Count > Item3->count)			{ return false; }		//not enough ingredients
	}
	if (this->NumberOfIngredients > 3)													//Check the fourth item in the recipe
	{
		if (this->ingredients[3].ItemNum != Item4->itemnum)		{ return false; }		//not the right item
		if (this->ingredients[3].ItemType != Item4->itemtype)	{ return false; }		//not the right item
		if (this->ingredients[3].Count > Item4->count)			{ return false; }		//not enough ingredients
	}

	// I guess this must mean that all the relevent ingredients match what is in the slots sent from the client so return true.
	return true;
}

bool CCraftRecipe::isValid() 
{
	return (!this->NumberOfIngredients == 0);	//returns true if there are more than 0 ingredients in the recipe
}



#pragma endregion