#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"

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
	 vec3 position, float rotation, float scale, 
	 vec3 direction, float velocity, vec3 dimensions);
  
  //Object methods
  bool collide(GameObject* object);
};

#endif
