#ifndef VISUALMETER_H_
#define VISUALMETER_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"
#include "../WorldGrid/WorldGrid.h"

using namespace glm;

class VisualMeter : public GameObject {
protected:

public:
  //Properties

  //Constructor
  VisualMeter(Mesh *mesh,
    vec3 position, float rotation, vec3 scale,
    vec3 direction, float velocity, vec3 dimensions,
    int scanRadius, int material);

  //Object methods
  void move(float time);
  bool collide(GameObject* object);
};

#endif
