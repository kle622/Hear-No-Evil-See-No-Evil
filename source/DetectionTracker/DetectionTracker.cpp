#include "DetectionTracker.h"

DetectionTracker::DetectionTracker() {
  this->visualDetLvl = 0.0f;
  this->soundDetLvl = 0.0f;
  this->totalDetLvl = 0.0f;
  this->detecDanger = false;
}

float clamp(float val) {
  if (val > 1)
    val = 1.0;

  return val;
}

/* 
   When detected by a guard, update visual detection.
   This means that if you're being seen by multiple guards over and over you be detected very quickly.

   Idea, if guard detected a player (partially) and they stop detecting the player, they keep calling this method for several more frames,
   if the player remains undetected and they have left the neighborhood of suspicion, decrement very quickly

   Other, lesser, idea number of guards who detected player inhibits how fast detection goes down
*/
void DetectionTracker::updateVisDetect(float detecPercent, Player *player) {
  // Note detecDanger 
  if (detecDanger == true) {
    this->totalDetLvl += 1*detecPercent *.01;
    this->previousPlyrPos = player->position;
  }
  else if(this->totalDetLvl > 0) {
    //this->totalDetLvl -= .005;
    this->totalDetLvl = clamp(this->totalDetLvl);    
  }
}

// For now I'm assuming CROUCH does not contribute to the total detection level, 
// but it also doesn't lower it
void DetectionTracker::updateSndDetect(Player *player) {
  if (this->detecDanger == true) {
    float dist = glm::distance(this->previousPlyrPos, player->position);
    if (dist < NEIGHBORHOOD_OF_SUSPICION) {
      this->detecDanger == false;
    }
  }

  if (player->velocity <= 0.0 && this->totalDetLvl > 0) {
    //this->totalDetLvl -= .02;
    this->totalDetLvl = clamp(this->totalDetLvl);
  }
  else if (player->velocity > 0.0){
    if (player->maxVelocity == WALK) {
      this->totalDetLvl += .02;
    }
    else if (player->maxVelocity == RUN) {
      this->totalDetLvl += .05;
    }
  }
  this->previousPlyrPos = player->position;
}

void DetectionTracker::updateVisMeter(Player *player) {
  this->visMeter->position = vec3(player->position.x, player->position.y + 1, player->position.z);
  this->visMeter->scale = vec3(this->totalDetLvl, this->totalDetLvl, this->totalDetLvl);
}


