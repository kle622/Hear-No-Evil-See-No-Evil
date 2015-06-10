#ifndef SHAPE_H_
#define SHAPE_H_
#define MIP_MAP_SIZE 512
#define BMP_SIZE 1024

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
 public:
  vec3 position;
  vec3 scale;
  vec3 direction;
  float velocity;
  int indices;
  GLuint posBuffer;
  GLuint norBuffer;
  GLuint texBuffer;
  GLuint idxBuffer;
  GLint posHandle;
  GLint norHandle;
  int material;
  GLuint texId;
  int nump_mipmaps = 5;
  char* bmp;
  //Constructor
  Shape(vec3 position,
	vec3 scale, vec3 direction, float velocity, int indices, 
	GLuint posBuffer, GLuint norBuffer, GLuint idxBuffer, GLuint texBuffer, int material);
  Shape(vec3 position,
        vec3 scale, vec3 direction, float velocity, int indices,
        GLuint posBuffer, GLuint norBuffer, int material);

  void loadTexture(const std::string &filename);
  void loadMipmapTexture(const std::string &filename, int size);
  virtual glm::mat4 getModel();
  //Object methods
  //void draw();
  //void SetModel(GLint handle, vec3 trans, float rot, vec3 sc);
};

#endif
