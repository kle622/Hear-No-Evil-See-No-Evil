#include "Player.h"
#include "Bunny.h"

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

void Player::move(float time) {
    //empty for now
}
