#ifndef GUARD_H_
#define GUARD_H_

#include <vector>
#include "GameObject.h"
#include "Player.h"
#include "../deferred/Light.h"
#include "Mesh.h"
#include "Handles.h"
#include "../GuardPath/PathNode.h"
#include "../Camera/DetectionCamera.h"
#include "../glm/glm.hpp"
#include "../DetectionTracker/DetectionTracker.h"

#define GUARD_SPIN_SPEED 25.0f
#define GUARD_VISION_RANGE 0.2

using namespace std;
using namespace glm;
class DetectionTracker;

class Guard : public GameObject {
protected:

public:
	//Properties
	std::vector<PathNode> motionPath;
	int pathDirection;
	bool moving;

	bool staring; // whether or not we're in staring mode
	bool foundPlayer; // whether or not our view has been centered on the player or not
	float playerTimer; // how long since we last saw the player
	vec3 lastSeen;

	vector<Light> *lights;
	int lightIndex;

	int currentNode;
	int sweepDirection; // 1 = clockwise, -1 = ccw
	float waitTime;
	int originalMaterial;
	Player *playerObject;
  char* reactSnd;
  char* dismissSnd;

	//Constructor
	Guard(Mesh *mesh,vec3 scale, float velocity, vec3 dimensions,
		int scanRadius, int material, vector<PathNode> motionPath, Player *player, vector<Light> *lights, int lightIndex,
	char* reactSnd, char* dismissSnd);

	//Object methods
	void move(float time);
	bool collide(GameObject* object);
	void stare();
	void updateLight();
  glm::mat4 getModel();
	float detect(WorldGrid *world, DetectionCamera *cam, DetectionTracker *detecTrac);
};

#endif
