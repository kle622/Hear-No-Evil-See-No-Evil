#include "Player.h"
#include "Bunny.h"
#include "../Camera/Camera.h"

#define MAX_VELOCITY 15.0f
#define MIN_VELOCITY 0.0f
#define ACCELERATION 0.2f
#define DECELERATION 0.5f

Player::Player(Mesh *mesh, Handles *handles,
    vec3 position, float rotation, vec3 scale, 
           vec3 direction, float velocity, vec3 dimensions, 
           int scanRadius, int material = 0) : 
        GameObject(mesh, handles, position, rotation, scale, 
         direction, velocity, dimensions, scanRadius, material) {

}

bool Player::collide(GameObject* object) {
    float thisRadius = dimensions.x + dimensions.y + dimensions.z;
    float objectRadius = object->dimensions.x + object->dimensions.y + 
        object->dimensions.z;

    if (compareDistance(position, object->position, thisRadius + objectRadius)) {
        if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
            intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
            intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
            position = oldPosition;
            velocity = 0;
            return true;
        }
    }

    return false;
}

void Player::move(float time) {
    oldPosition = position;
    position += normalize(direction) * velocity * time;
}

void Player::accelerate() {
    velocity = std::max(MIN_VELOCITY, velocity);
    velocity += ACCELERATION;
    velocity = std::min(MAX_VELOCITY, velocity);
}

void Player::decelerate() {
    velocity = std::min(MAX_VELOCITY, velocity);
    velocity -= DECELERATION;
    velocity = std::max(MIN_VELOCITY, velocity);
}

void Player::changeDirection(vec3 direction) {
    velocity *= dot(this->direction, direction);
    this->direction = direction;
}