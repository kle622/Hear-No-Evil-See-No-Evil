#ifndef GUARD_H_
#define GUARD_H_

#include <vector>
#include "GameObject.h"
#include "Player.h"
#include "Mesh.h"
#include "Handles.h"
#include "../glm/glm.hpp"


#define GUARD_VISION_RANGE 0.2

using namespace std;
using namespace glm;

class Guard : public GameObject {
protected:

public:
	//Properties
	std::vector<vec3> motionPath;
	int pathDirection;
	bool moving;
	int currentNode;
	int endpointDirection; // 1 = clockwise, -1 = ccw
	int sweepDirection; // 1 = clockwise, -1 = ccw

	//Constructor
	Guard(Mesh *mesh, Handles *handles, vec3 scale, float velocity, vec3 dimensions,
		int material, int scanRadius, vector<vec3> motionPath);

	//Object methods
	void move(float time);
	bool collide(GameObject* object);
	bool detect(Player* player);
};

#endif