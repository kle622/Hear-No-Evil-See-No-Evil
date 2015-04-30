#include "Wall.h"
#include "Player.h"

Wall::Wall(Mesh *mesh, Handles *handles,
  vec3 position, float rotation, vec3 scale,
  vec3 direction, float velocity, vec3 dimensions, 
  int scanRadius, int material = 0) :
  GameObject(mesh, handles, position, rotation, scale,
  direction, velocity, dimensions, scanRadius, material) {

}

void Wall::move(float time) {
  if (position.x >= 29.0f) {
    position.x = 29.0f;
    direction.x = -direction.x;
  }
  if (position.x <= -29.0f) {
    position.x = -29.0f;
    direction.x = -direction.x;
  }
  if (position.z >= 29.0f) {
    position.z = 29.0f;
    direction.z = -direction.z;
  }
  if (position.z <= -29.0f) {
    position.z = -29.0f;
    direction.z = -direction.z;
  }

  position += normalize(direction) * velocity * time;
  rotation = 90 + (atan2f(direction.x, direction.z) * 180 / M_PI);
}

bool Wall::collide(GameObject* object) {
  /**float thisRadius = dimensions.x + dimensions.y + dimensions.z;
  float objectRadius = object->dimensions.x + object->dimensions.y +
    object->dimensions.z;

  if (compareDistance(position, object->position, thisRadius + objectRadius)) {
    if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
      intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
      intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
      return true;
    }
  }*/
  return false;
}
