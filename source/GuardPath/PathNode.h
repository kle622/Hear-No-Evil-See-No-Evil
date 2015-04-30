#ifndef PATHNODE_H_
#define PATHNODE_H_

#include "../glm/glm.hpp"



using namespace std;
using namespace glm;

class PathNode {
protected:

public:
	vec3 pos;
	bool smoothTurn; // true for 'spline' turning; false for stop, turn, and go
	float moveDelay; // in seconds
	int endTurnDir; // 1 = clockwise, -1 = ccw
	bool end;

	PathNode::PathNode(vec3 pos, bool smoothTurn, float moveDelay, bool endTurnDir, bool end);
};

#endif