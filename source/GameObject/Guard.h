#ifndef GUARD_H_
#define GUARD_H_

#include <vector>
#include "GameObject.h"
#include "Player.h"
#include "Mesh.h"
#include "Handles.h"
#include "../GuardPath/PathNode.h"
#include "../glm/glm.hpp"

#define GUARD_SPIN_SPEED 75.0f
#define GUARD_VISION_RANGE 0.2

using namespace std;
using namespace glm;

class Guard : public GameObject {
protected:

public:
	//Properties
	std::vector<PathNode> motionPath;
	int pathDirection;
	bool moving;
	int currentNode;
	int sweepDirection; // 1 = clockwise, -1 = ccw
	float waitTime;
	int originalMaterial;

	//Constructor
	Guard(Mesh *mesh, Handles *handles, vec3 scale, float velocity, vec3 dimensions,
		int scanRadius, int material, vector<PathNode> motionPath);

	//Object methods
	void move(float time);
	bool collide(GameObject* object);
	bool detect(Player* player);
};

#endif