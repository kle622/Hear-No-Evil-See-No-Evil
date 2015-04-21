#ifndef WALL_H_
#define WALL_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"

using namespace glm;

class Wall : public GameObject {
protected:

public:
  //Properties

  //Constructor
  Wall(Mesh *mesh, Handles *handles,
    vec3 position, float rotation, vec3 scale,
    vec3 direction, float velocity, vec3 dimensions, int material);

  //Object methods
  void move(float time);
  bool collide(GameObject* object);
};

#endif
