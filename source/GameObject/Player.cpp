#include "Player.h"
#include "Bunny.h"
#include "../Camera/Camera.h"

#define CAMERA_SPEED 10.0f

Player::Player(Mesh *mesh, Handles *handles,
    vec3 position, float rotation, vec3 scale, 
	       vec3 direction, float velocity, vec3 dimensions, 
           int scanRadius, int material = 0) : 
  GameObject(mesh, handles, position, rotation, scale, 
	     direction, velocity, dimensions, scanRadius, material) {

}

bool Player::collide(GameObject* object) {
    printf("checking collide\n");
    float thisRadius = dimensions.x + dimensions.y + dimensions.z;
    float objectRadius = object->dimensions.x + object->dimensions.y + 
        object->dimensions.z;

    if (compareDistance(position, object->position, thisRadius + objectRadius)) {
        if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
            intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
            intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
            printf("colliding with something\n");
            return true;
        }
    }

    return false;
}

int* Player::findRestrictedMovement(Camera* camera, double deltaTime, GameObject* object) {
  float forwardYVelocity = 0;
  float sideYVelocity = 0;
  int restrictDir[4] = { 0 };

  // Check if there is a collision in each direction 
  // A KEY : Left
  vec3 tempPostition = position;
  vec3 tempVelocity = glm::vec3(camera->rightV.x * CAMERA_SPEED * deltaTime,
      sideYVelocity, camera->rightV.z * CAMERA_SPEED * deltaTime);
  position = camera->position - tempVelocity;
  if (collide(object)) {
    restrictDir[0] = 1;
  }

  // D KEY : Right
  tempVelocity = glm::vec3(camera->rightV.x * CAMERA_SPEED * deltaTime,
      sideYVelocity, camera->rightV.z * CAMERA_SPEED * deltaTime);
  position = camera->position + tempVelocity;
  if (collide(object)) {
    restrictDir[1] = 1;
  }

  // W KEY : Forward
  tempVelocity = glm::vec3(camera->direction.x * CAMERA_SPEED * deltaTime,
      forwardYVelocity, camera->direction.z * CAMERA_SPEED * deltaTime);
  position = camera->position + tempVelocity;
  if (collide(object)) {
    restrictDir[2] = 1;
  }

  // S KEY : Backward
  tempVelocity = glm::vec3(camera->direction.x * CAMERA_SPEED * deltaTime,
      forwardYVelocity, camera->direction.z * CAMERA_SPEED * deltaTime);
  position = camera->position - tempVelocity;
  if (collide(object)) {
    restrictDir[3] = 1;
  }

  // Set player's positon back to the orignal 
  position = tempPostition;
  return restrictDir;
}

void Player::move(float time) {
    //empty for now
}
