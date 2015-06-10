#include "DetectionTracker.h"

DetectionTracker::DetectionTracker() {
  this->visualDetLvl = 0.0f;
  this->soundDetLvl = 0.0f;
  this->totalDetLvl = 0.0f;
  this->detecDanger = false;
  this->lightRadius = 1.7f;
}

/* Keep our totalDetLvl (total detection level) between 0.0 and 1.0.
   This is important for color bleeding and the detection bar. 
*/
void DetectionTracker::clamp() {
  if (this->totalDetLvl > 1.0) {
    this->totalDetLvl = 1.0;
  }
  if (this->totalDetLvl < 0) {
    this->totalDetLvl = 0.0;
  }
}

/* Calculate the current distance from the most recent light to the player*/
void DetectionTracker::getDistToCurrLight(vec3 playerPos, vec3 lightPos) {
  float deltaX = playerPos.x - lightPos.x;
  float deltaZ = playerPos.z - lightPos.z;

  float dist = deltaX * deltaX + deltaZ * deltaZ;

  this->lightDist = dist;
}

/*
When detected by a guard, update visual detection.
This means that if you're being seen by multiple guards over and over you'll be detected very quickly.

If the player remains undetected and they have left the neighborhood of suspicion, decrement very quickly

The player's distance to the closest light will influence how fast detection is raised.
*/
void DetectionTracker::updateVisDetect(float detecPercent, Player *player) {
  float lightDetecMult = 0.0f;
  float playerRadius = player->dimensions.x + player->dimensions.z;
  getDistToCurrLight(player->position, this->currLight.position);
  float maxRadius = lightRadius + playerRadius;
  maxRadius = maxRadius * maxRadius;
  this->previousPlyrPos = player->position;
  float darkObsucrement = .07;

  if (detecDanger) {
	  printf("DANGER 1!\n");
    if (this->lightDist < maxRadius) {
      lightDetecMult = 1/sqrt(this->lightDist);
    }
    else {
		lightDetecMult = (1 / sqrt(this->guardDist)) * 1 / sqrt(this->lightDist);
    }
    this->totalDetLvl += detecPercent * lightDetecMult;
    this->previousPlyrPos = player->position;
  }
  else if (this->totalDetLvl > 0) {
  }
  if (this->totalDetLvl > 1.0f) {
    this->totalDetLvl = 0.0f;
    player->lose();
  }
  clamp();
  //printf("Post CLAMP DetecPercent: %f * lightMult: %f  = TOTAL: %f\n\n", detecPercent, lightDetecMult, this->totalDetLvl);
}

// For now I'm assuming CROUCH does not contribute to the total detection level, 
// but it also doesn't outright lower it
void DetectionTracker::updateSndDetect(Player *player) {
	/*if (this->detecDanger == true) {
		float dist = glm::distance(this->previousPlyrPos, player->position);
		if (dist > NEIGHBORHOOD_OF_SUSPICION) {
			this->detecDanger = false;
		}
	}*/
	if (detecDanger)
		printf("DANGER!\n");
	if (player->velocity <= 0.0 && totalDetLvl > 0 && !detecDanger) {
		printf("out of view\n");
		this->totalDetLvl -= .01;
	}
	else if (player->velocity > 0.0) {
		if (player->maxVelocity == WALK) {
			//this->totalDetLvl -= .005;
		}
		else if (player->maxVelocity == CROUCH && !detecDanger) {
			printf("detracting 2\n");
			this->totalDetLvl -= .005;
		}
		else if (player->maxVelocity == RUN) {
			this->totalDetLvl += .01;
		}
	}
	if (this->totalDetLvl <= 0) {
		//this->detecDanger = false;
	}
	if (this->totalDetLvl > 1.0f) {
		//printf("you lose\n");
		this->detecDanger = false;
		this->totalDetLvl = 0.0f;
		player->lose();
	}
	clamp();
}
