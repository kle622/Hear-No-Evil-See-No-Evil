#include "PathNode.h"


PathNode::PathNode(vec3 pos, bool smoothTurn, float moveDelay, bool endTurnDir, bool end) {
	this->pos = pos;
	this->smoothTurn = smoothTurn;
	this->moveDelay = moveDelay;
	this->endTurnDir = endTurnDir;
	this->end = end;
}