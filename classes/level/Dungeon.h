#ifndef dungeonH
#define dungeonH

#include "Level.h"
#include "Leaf.h"
#include "../delaunay/delaunay.h"
#include <vector>
#include <map>
#include <list>

struct stVec2Compare {
	bool operator() (const glm::vec2& a, const glm::vec2& b) {
		return (a.x < b.x) || (a.x == b.x && a.y < b.y);
	}
};

class MDungeon: public MLevel {
private:
	int MinLeafSize;
	int MaxLeafSize;
	int MinRoomSize;
	std::list<TNode<stLeaf>* > Tree;
	std::map<glm::vec2, TNode<NRectangle2>*, stVec2Compare> NodesCenters;
	std::vector<MEdge> MST;
	
	bool CreateRooms();
	bool Triangulate();
	bool CreateHalls();
public:
	MDungeon();
	MDungeon(int TilesCountX, int TilesCountY, int inMinLeafSize, int inMaxLeafSize, int inMinRoomSize);
	bool Generate();
	bool Clear();
	bool Close();
	int GetType();
};

#endif
