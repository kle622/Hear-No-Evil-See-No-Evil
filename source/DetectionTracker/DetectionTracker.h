#ifndef DETECTIONTRACKER_H_
#define DETECTIONTRACKER_H_

#include "../GameObject/GameObject.h"
#include "../GameObject/Player.h"
#include "../GameObject/Guard.h"
#include "../Camera/Camera.h"
#include "../WorldGrid/WorldGrid.h"
#include "../Deferred/Light.h"

#define CROUCH 1.0f
#define WALK 5.0f
#define RUN 16.0f
#define CROUCH_VELOCITY 2.5f
#define MIN_VELOCITY 0.0f
#define ACCELERATION 0.5f
#define DECELERATION 0.5f
#define CROUCH_SCALE 0.5f
#define MAX_STAMINA 5.0f
#define MAX_CROUCH_STAMINA 5.0f

#define NEIGHBORHOOD_OF_SUSPICION 5.0f
using namespace glm;

class DetectionTracker {
public:
  //members
  WorldGrid *grid;
  Player *player;
  float visualDetLvl;
  float soundDetLvl;
  float totalDetLvl;
  vec3 previousPlyrPos; // Should be set when a guard sees a player, or if their movement raises the detect level too much
  bool detecDanger;
  float lightDist;
  float lightRadius;
  bool isInLight;
  Light currLight;
  float guardDist;

  //Constructor
  DetectionTracker();

  //Object methods
  void reduceDetect(Player *plyr);
  void updateVisDetect(float detecPercent, Player *player);
  void getDistToCurrLight(vec3 playerPos, vec3 lightPos);
  void updateSndDetect(Player *player);
  void updateVisMeter(Player *player);
  void clamp();
};

#endif

