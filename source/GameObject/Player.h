#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"
#include "../Camera/Camera.h"

using namespace glm;

class Player: public GameObject {
 protected:
  Mesh *mesh;
  Handles *handles;
 public:
  //members
  int score = 0;
  
  //Constructor
  Player(Mesh *mesh, Handles *handles,
   vec3 position, float rotation, vec3 scale, 
   vec3 direction, float velocity, vec3 dimensions, 
  int scanRadius, int material);
  
  //Object methods
  void accelerate();
  void decelerate();
  bool collide(GameObject* object);
  void move(float time);
};

#endif