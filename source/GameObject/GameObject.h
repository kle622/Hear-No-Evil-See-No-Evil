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
#include "Mesh.h"
#include "Handles.h"
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
  //  Mesh *mesh;
  
 public:
  //Properties
  vec3 position;
  vec3 oldPosition;
  vec3 scale;
  
  vec3 direction;
  float velocity;
  float rotation;
  
  vec3 dimensions;
  int scanRadius;
  
  int material;
  
  bool alive;
  bool pushable;

  Mesh* mesh;
  
  //Constructor
  GameObject(Mesh *mesh, 
	     vec3 position, vec3 scale, float rotation,
	     vec3 direction, float velocity, vec3 dimensions, 
       int scanRadius, int material, bool pushable);
  
  //Object Methods
  virtual void move(float time);
  virtual bool collide(GameObject* object);
  //virtual int* findRestrictedMovement(Camera* camera, double deltaTime, GameObject* object);
};

bool intersect(float point1, float point2, float dim1, float dim2);
bool compareDistance(vec3 first, vec3 second, float max);
#endif
