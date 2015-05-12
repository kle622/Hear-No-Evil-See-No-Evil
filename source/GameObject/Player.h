#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"
#include "../Camera/Camera.h"

using namespace glm;
#define CROUCH 1.0f
#define WALK 5.0f
#define RUN 8.0f
#define CROUCH_VELOCITY 2.5f
#define MIN_VELOCITY 0.0f
#define ACCELERATION 0.5f
#define DECELERATION 0.5f
#define CROUCH_SCALE 0.5f
#define MAX_STAMINA 300
#define MAX_CROUCH_STAMINA 300

class Player: public GameObject {
 protected:
  Mesh *mesh;
  float maxVelocity;
  float standingScale;

  public:
  //members
  bool crouch;
  int stamina = MAX_STAMINA;
  int crouchStamina = MAX_CROUCH_STAMINA;
  int score = 0;
  
  //Constructor
  Player(Mesh *mesh,
   vec3 position, float rotation, vec3 scale, 
   vec3 direction, vec3 dimensions, 
  int scanRadius, int material);
  
  //Object methods
  void accelerate();
  void decelerate();
  void changeDirection(vec3 direction);
  bool collide(GameObject* object);
  void move(float time);
  void SetMotion(float motion);
};

#endif
