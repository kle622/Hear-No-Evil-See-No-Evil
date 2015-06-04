#ifndef CLUE_H_
#define CLUE_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"
#include "../WorldGrid/WorldGrid.h"

#define ROTATE_SPEED 50.0f

using namespace glm;

class Clue : public GameObject {
protected:

public:
  //Properties
  char* soundPath;

  //Constructor
  Clue(Mesh *mesh,
    vec3 position, vec3 scale,
    vec3 direction, float velocity, vec3 dimensions, 
    int scanRadius, int material, char* soundPath);

  //Object methods
  void move(float time);
};

#endif
