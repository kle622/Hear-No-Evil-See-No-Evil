#include "Player.h"
#include "Bunny.h"
#include "../Camera/Camera.h"

#define CAMERA_SPEED 10.0f
#define MAX_VELOCITY 15.0f
#define ACCELERATION 0.5f
#define DECELERATION 0.7f

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

            /**
            vec3 direction = position - object->position; // w2p
            direction.y = 0.0f;
            direction = normalize(direction);
            //printf("x: %f, y: %f, z: %f\n", direction.x, direction.y, direction.z);

            vec3 thisCorner = position;
            thisCorner.x -= sign(direction.x) * dimensions.x / 2;
            thisCorner.y = 0;
            thisCorner.z -= sign(direction.z) * dimensions.z / 2;

            //direction = thisCorner - object->position;

            vec3 posZnegX(-object->dimensions.x, 0, object->dimensions.z);
            vec3 negZnegX(-object->dimensions.x, 0, -object->dimensions.z);

            float cross1 = cross(posZnegX, direction).y;
            float cross2 = cross(negZnegX, direction).y;

            int signx = sign(direction.x);
            int signz = sign(direction.z);

            if (cross1 * cross2 < 0) {
                // restrict x
                printf("RESTRICT X!\n");
                position.x = object->position.x + signx *
                    (object->dimensions.x + dimensions.x) / 2;
            }
            else {
                // restrict z
                printf("RESTRICT Z!\n");
                position.z = object->position.z + signz *
                    (object->dimensions.z + dimensions.z) / 2;
            }
            **/
        }
    }

    return false;
}

void Player::move(float time) {
    oldPosition = position;
    position += normalize(direction) * velocity * time;
}

void Player::accelerate() {
    if (velocity < MAX_VELOCITY) {
        velocity += ACCELERATION;
    }
    printf("accelerating\n");
}

void Player::decelerate() {
    velocity -= DECELERATION;
        if (velocity < 0) {
            velocity = 0;
        }
    printf("decelerating\n");
}
