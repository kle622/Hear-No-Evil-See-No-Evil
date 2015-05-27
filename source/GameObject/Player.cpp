#include "Player.h"
#include "Bunny.h"
#include "../Camera/Camera.h"
#include "../MySound/MySound.h"
#include "WinCondition.h"
#include <algorithm>
MySound* playrSoundObj = new MySound();

Player::Player(Mesh *mesh,
    vec3 position, float rotation, vec3 scale, 
           vec3 direction, vec3 dimensions, 
           int scanRadius, int material = 0) : 
        GameObject(mesh, position, rotation, scale, 
         direction, WALK, dimensions, scanRadius, material, true) {
    maxVelocity = WALK;
    crouch = false;
    standingScale = scale.y;
}

bool Player::collide(GameObject* object) {
    float thisRadius = dimensions.x + dimensions.y + dimensions.z;
    float objectRadius = object->dimensions.x + object->dimensions.y + 
        object->dimensions.z;

    if (compareDistance(position, object->position, thisRadius + objectRadius)) {
        if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
            intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
            intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
            if (object->pushable && this->velocity > WALK) {
                object->velocity = this->velocity;
                object->direction = this->direction;
            }
            if (dynamic_cast<WinCondition*>(object)) {
                //do win stuff here
                playrSoundObj->winSnd = playrSoundObj->startSound(playrSoundObj->winSnd, "../dependencies/irrKlang/media/victory_music.wav");
                printf("I WIN\n");
            }
            if (dynamic_cast<VisualMeter*>(object)) {
              return false;
            }
            position = oldPosition; // TODO implement a better system
            return true;
        }
    }

    return false;
}

void Player::move(float time) {
    oldPosition = position;
    static float yPos = position.y;
    position += normalize(direction) * velocity * time;
    if (crouch) {
        if (scale.y > CROUCH_SCALE) {
            scale.y -= 0.1f * (standingScale - CROUCH_SCALE);
            position.y -= 0.05 * standingScale;
        }
    }
    else {
        scale.y = std::min(scale.y + 0.01f, standingScale);
        position.y = yPos;
    }
}

void Player::accelerate() {
    velocity = std::max(MIN_VELOCITY, velocity);
    velocity += ACCELERATION;
    velocity = std::min(maxVelocity, velocity);
    if (maxVelocity == WALK) {
      playrSoundObj->footSndPlayr = playrSoundObj->startSound(playrSoundObj->footSndPlayr, "../dependencies/irrKlang/media/footstepsWalk2.wav");
    }
    else if (maxVelocity == RUN) {
      playrSoundObj->footSndPlayr = playrSoundObj->startSound(playrSoundObj->footSndPlayr, "../dependencies/irrKlang/media/fastWalk.wav");
    }
    else if (maxVelocity == CROUCH) {
      playrSoundObj->footSndPlayr = playrSoundObj->startSound(playrSoundObj->footSndPlayr, "../dependencies/irrKlang/media/crouchWalk.wav");
    }
}

void Player::decelerate() {
    velocity = std::min(maxVelocity, velocity);
    velocity -= DECELERATION;
    velocity = std::max(MIN_VELOCITY, velocity);
}

void Player::changeDirection(vec3 direction) {
    velocity *= dot(this->direction, direction);
    this->direction = direction;
}

void Player::SetMotion(float motion) {
    maxVelocity = motion;
}
