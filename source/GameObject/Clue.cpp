#include "Clue.h"
#include "Player.h"
#include "../WorldGrid/WorldGrid.h"

Clue::Clue(Mesh *mesh,
  vec3 position, vec3 checkpoint, vec3 scale,
  vec3 direction, float velocity, vec3 dimensions, 
  int scanRadius, int material = 0, char* soundPath = "THIS SHOULD NEVER HAPPEN") :
  GameObject(mesh, position, scale, 0,
  direction, velocity, dimensions, scanRadius, material, ObjectType::COLLECTABLE) {
  	this->soundPath = soundPath;
  	this->checkpoint = checkpoint;
    this->isCollected = false;
}

void Clue::move(float time) {
	this->rotation += time * ROTATE_SPEED;
}
