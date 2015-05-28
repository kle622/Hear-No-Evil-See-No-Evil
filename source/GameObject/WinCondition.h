#ifndef WINCONDITION_H_
#define WINCONDITION_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"

using namespace glm;

class WinCondition : public GameObject {
protected:

public:
  //Properties

  //Constructor
  WinCondition(Mesh *mesh,
    vec3 position, vec3 scale, float rotation,
    vec3 direction, float velocity, vec3 dimensions, 
    int scanRadius, int material);

  //Object methods
  void move(float time);
  bool collide(GameObject* object);
};

#endif
