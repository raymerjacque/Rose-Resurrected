#pragma once
#include "Compat.h"
#include "Enums.h"
#include <string>
#include <vector>
#include <map>

/// FK New Typename Convention.
// Normal coding convention. I opted to keep it (almost) the SAME as .Net;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef float				float32;
typedef double				float64;


/// Composite Datatypes;

// -----------------------------------------------------------------------------------------
// A 3d point, for positions
// -----------------------------------------------------------------------------------------

class ColorRGB {
public:
	uint8  Red   = 0x0;
	uint8  Green = 0x0;
	uint8  Blue  = 0x0;
	uint32 Value = 0;

	ColorRGB(uint8 Red, uint8 Green, uint8 Blue) {
		this->Red   = Red;
		this->Green = Green;
		this->Blue  = Blue;

		this->Value = Red * 256 * 256 + Green * 256 + Blue;
	}

	ColorRGB(DefinedColors Color) {
		uint32 rMask = 0xff0000;
		uint32 gMask = 0xff00;
		uint32 bMask = 0xff; 

		this->Value = uint32(Color);
		this->Red   = (this->Value & rMask) >> 16;
		this->Green = (this->Value & gMask) >> 8;
		this->Blue  = (this->Value & bMask);
	}
};

class ValueRange {
public:
	int32 min = 0;
	int32 max = 0;

	ValueRange() {
		this->min	= 0;
		this->max   = 0;
	}

	ValueRange(int32 min, int32 max) {
		this->min = min;
		this->max = max;
	}
	
	~ValueRange() {}
	 
	bool isEmpty() {
		if (this->min == 0 && this->max== 0) {
			return true;
		}
		return false;
	}

	void Clear() {
		this->min = 0;
		this->max = 0;
	}
};

class ValuePair {
public:
	uint16 id	 = 0;
	int32  value = 0;

	ValuePair() {
		this->id	= 0;
		this->value = 0;
	}

	ValuePair(uint16 id, int32 value) {
		this->id	= id;
		this->value = value;
	}

	ValuePair(uint16 id) {
		this->id	= id;
		this->value = 0;
	}

	~ValuePair() {}

	void Set(uint16 id, int32 value) {
		this->id	= id;
		this->value = value;

		// Now Validate; if id=0, the value can't be used. 
		if (this->id == 0) {
			this->value = 0;
		}
	}

	bool isEmpty() {
		if (this->id == 0 && this->value == 0) {
			return true;
		}
		return false;
	}

	void Clear() {
		this->id = 0;
		this->value = 0;
	}
};
class fPoint {
	/// FK: Converted this thing to a class and added constructors to it.

public:
	float32 x;
	float32 y;
	float32 z;		// Mostly unused. Where used at all it is always zero

	fPoint() {
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	/// Float32 interface
	fPoint(float32 X, float32 Y) {
		this->x = X;
		this->y = Y;
		this->z = 0.0f;
	}

	fPoint(float32 X, float32 Y, float32 Z) {
		this->x = X;
		this->y = Y;
		this->z = Z;
	}

	bool empty ( ) {
		return ( this->x == 0 && this->y == 0 && this->z == 0 );
	}

	bool compare ( fPoint* Coordinate ) {
		return ( ( this->x = Coordinate->x ) && ( this->y == Coordinate->y ) && ( this->z == Coordinate->z ) );
	}
	
	void clear ( ) {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	std::string toString() {
		char buffer[200];
		sprintf(buffer, "[%.0f,%.0f]", this->x, this->y);
		return std::string(buffer);
	}
};

/// FK: Kill, destroy wipe and replace every occurence of this below !
// Just some defs for easier use
//typedef char					SBYTE;					// Checked, replaced and removed (FK)
#ifdef _WIN32
  typedef unsigned char			BYTE;
  typedef short					SWORD;
  typedef unsigned short		WORD;
  typedef long					SDWORD;
  typedef unsigned long			DWORD;
  typedef long long				SQWORD;
  typedef unsigned long long	QWORD;

/// Keep this for now!
//For QSD; still used there;
  typedef unsigned __int8       byte;
  typedef unsigned __int16      word;
  typedef unsigned __int32      dword;
//typedef unsigned __int64      qword;					// Checked, replaced and removed (FK)
//typedef char*                 strings;				// Checked, replaced and removed (FK)
#endif