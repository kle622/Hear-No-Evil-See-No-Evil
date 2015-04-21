#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr

using namespace glm;
using namespace std;

class GameObject {
 protected:
  //Properties
  //GLint modelHandle;
  //int indices;
  Mesh *mesh;
  Handles *handles;
  
 public:
  //Properties
  vec3 position;
  float rotation;
  float scale;
  
  vec3 direction;
  float velocity;
  
  vec3 dimensions;
  
  /* GLuint posBuffer;
  GLuint norBuffer;
  GLuint indBuffer;
  GLint posHandle;
  GLint norHandle;
  int material;*/
  
  bool alive;
  
  //Constructor
  GameObject(Mesh *mesh, Handles *handles, 
	     vec3 position, float rotation, float scale, 
	     vec3 direction, float velocity, vec3 dimensions, int indices,
	     GLuint posBuff);
  
  //Object Methods
  virtual void draw();
  virtual void move(float time);
  virtual bool collide(GameObject* object);
};

void SetModel(GLint handle, vec3 trans, float rot, vec3 sc);
bool intersect(float point1, float point2, float dim1, float dim2);
bool compareDistance(vec3 first, vec3 second, float max);
#endif
