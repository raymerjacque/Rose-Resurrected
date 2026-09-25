#pragma once
/*---------------------------------------------------------------------------------------------------------
	Poject 137 Header File:

	File		: Support.h
	NameSpace	: Rose

	Purpose		: Defines a number of support functions for global use;
	Code File	: -none-

	This is a helper class with static methods only; used for supportive operations.

	Revision	:  WHO	  | WHEN		  | WHAT

				:  FK		2018-11-06	  Created; added Password Validation;

	Comments	: uses regex lib! (VC11 or above)

---------------------------------------------------------------------------------------------------------*/
#include "TypeDef.h"
#include <regex>
#include <string>


namespace Rose {
	class Support {
	public:

		enum class ParserOptions {
			ItemNo,
			ItemType
		};

		static DWORD ParseItemCode(const DWORD itemcode, ParserOptions ParseWhat) {

			DWORD itemtype = 0;
			DWORD itemnum = 0;

			/// This type of encoding makes me wonder if we're able to support > 999 items per itemtype...
			
			if (itemcode < 99999) {
				itemtype = itemcode / 1000;
				if (itemtype >= 1 && itemtype <= 14) {
					itemnum = itemcode % 1000;
					if (ParseWhat == ParserOptions::ItemType) {
						return itemtype;
					}
					return itemnum;
				}
			} else {
				itemtype = itemcode / 100000;
				if (itemtype >= 1 && itemtype <= 14) {
					itemnum = itemcode % 100000;
					if (ParseWhat == ParserOptions::ItemType) {
						return itemtype;
					}
					return itemnum;
				}
			}
			return 0;

		} // End ParseItemCode;

		/// Random Number (one that works for UINT32);
		static uint32 Random(uint32 min, uint32 max) {
			// Now this function is truly random, and handles randoms like a charm.
			uint32 range = 0;
			uint32 result = 0;
			uint32 largenum = 0;
			uint32 smallnum = 0;

			range = (max - min) + 1;

			if (min >= 0 && max > min) {
				if (range < RAND_MAX) {
					/// FK : Regular INT numbers;
					return (rand() % range) + min;
				} else {
					/// BIG NUMBERS; shift the bits to the left to make some room...
					largenum = RAND_MAX * rand() + rand();
					smallnum = rand() & 0xf;

					result = (largenum << 1) + smallnum;

					return (result % range) + min;
				}
			} else {
				return 0;
			}
		}
		
	}; // End Support Class

}