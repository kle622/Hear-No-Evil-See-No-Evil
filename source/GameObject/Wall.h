#ifndef WALL_H_
#define WALL_H_

#include "GameObject.h"
#include "Mesh.h"
#include "Handles.h"
#include "../WorldGrid/WorldGrid.h"

using namespace glm;

class Wall : public GameObject {
protected:
	void genTexCoords(float mult);
	void sendTexCoords();
public:
  //Properties
	std::vector<float> texBuf;
	GLuint texBufObj;

  //Constructor
  Wall(Mesh *mesh,
    vec3 position, vec3 scale,
    vec3 direction, float velocity, vec3 dimensions, 
    int scanRadius, int material);

  //Object methods
  void move(float time);
  bool collide(GameObject* object);
  //void initWalls(WorldGrid* gameObjects);
};

#endif
