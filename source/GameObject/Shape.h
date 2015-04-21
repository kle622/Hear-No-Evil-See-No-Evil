#ifndef SHAPE_H_
#define SHAPE_H_

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include "Mesh.h"
#include "Handles.h"
#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc                                                                                           
#include "../glm/gtc/type_ptr.hpp" //value_ptr

using namespace glm;

class Shape {
 protected:
  Handles *handles;
  vec3 position;
  float rotation;
  vec3 scale;
  vec3 direction;
  float velocity;
  int indices;
  GLuint posBuffer;
  GLuint norBuffer;
  GLint posHandle;
  GLint norHandle;
  int material;
 public:
  //Constructor
  Shape(Handles *handles, vec3 position, float rotation, 
	vec3 scale, vec3 direction, float velocity, int indices, 
	GLuint posBuffer, GLuint norBuffer, int material);
  
  //Object methods
  void draw();
};

#endif
