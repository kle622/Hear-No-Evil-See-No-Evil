#ifndef WALL_H_
#define WALL_H_

#include "GameObject.h"

using namespace glm;

class Wall : public GameObject {
protected:

public:
  //Properties

  //Constructor
  Wall(GLint modelHandle,
    vec3 position, float rotation, vec3 scale,
    vec3 direction, float velocity, vec3 dimensions, int indices,
    GLuint posBuffer, GLuint norBuffer, GLuint indBuffer,
    GLint posHandle, GLint norHandle, int material);

  //Object methods
  void move(float time);
  bool collide(GameObject* object);
};

#endif