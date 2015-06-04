#include "Clue.h"
#include "Player.h"
#include "../WorldGrid/WorldGrid.h"

Clue::Clue(Mesh *mesh,
  vec3 position, vec3 scale,
  vec3 direction, float velocity, vec3 dimensions, 
  int scanRadius, int material = 0) :
  GameObject(mesh, position, scale, 0,
  direction, velocity, dimensions, scanRadius, material, ObjectType::COLLECTABLE) {

}

void Clue::move(float time) {
	rotation += ROTATE_SPEED * time;
}

bool Clue::collide(GameObject* object) {
  float thisRadius = dimensions.x + dimensions.y + dimensions.z;
  float objectRadius = object->dimensions.x + object->dimensions.y +
    object->dimensions.z;

  if (compareDistance(position, object->position, thisRadius + objectRadius)) {
    if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
      intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
      intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
      //play the fucking sound
      return true;
    }
  }
  return false;
}
