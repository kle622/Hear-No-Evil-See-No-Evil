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
		direction, WALK, dimensions, scanRadius, material, GameObject::ObjectType::PLAYER) {
    maxVelocity = WALK;
    crouch = false;
    standingScale = scale.y;
}

bool Player::collide(GameObject* object) {
	if (GameObject::collide(object)) {
		// specific stuff
		return true;
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
        crouchStamina -= 1.0f * time;
        crouchStamina = std::max(crouchStamina, 0.0f);
        if (crouchStamina == 0.0f)
            crouch = false;
    }
    else {
        if (crouchStamina <= 0) {
            maxVelocity = WALK;
        }
        scale.y = std::min(scale.y + 0.01f, standingScale);
        position.y = yPos;
        crouchStamina += 0.5f * time;
        crouchStamina = std::min(crouchStamina, MAX_CROUCH_STAMINA);
    }
    printf("crouchStamina: %f\n", crouchStamina);
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
