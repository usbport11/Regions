#ifndef levelH
#define levelH

#define LGT_NONE 0
#define LGT_DUNGEON 1
#define LGT_CAVE 2

#include <string.h>
#include "../system/random.h"

class MLevel {
protected:
	int Type;
	int TilesCount[2];
	char** Map;
	
	void FillMap(int x , int y, int Width, int Height, char Value);
	bool AllocateMap();
	bool AllocateMap(int TilesCountX, int TilesCountY);
	bool ReallocateMap(int TilesCountX, int TilesCountY);
	bool ConvertToTiles();
public:
	MLevel();
	MLevel(int TilesCountX, int TilesCountY);
	char GetValue(int x, int y);
	virtual bool Generate();
	virtual int GetType();
	void Clear();
	void Close();
};

#endif
