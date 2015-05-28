#include "DetectionTracker.h"

DetectionTracker::DetectionTracker() {
  this->visualDetLvl = 0.0f;
  this->soundDetLvl = 0.0f;
  this->totalDetLvl = 0.0f;
  this->detecDanger = false;
}

void DetectionTracker::clamp() {
  if (this->totalDetLvl > 1.0) {
    this->totalDetLvl = 1.0;
  }
  if (this->totalDetLvl < 0) {
    this->totalDetLvl = 0.0;
  }
}

/* 
   When detected by a guard, update visual detection.
This means that if you're being seen by multiple guards over and over you'll be detected very quickly.

If the player remains undetected and they have left the neighborhood of suspicion, decrement very quickly

   Other, lesser, idea number of guards who detected player inhibits how fast detection goes down
*/
void DetectionTracker::updateVisDetect(float detecPercent, Player *player) {
  if (this->detecDanger == true) {
    this->totalDetLvl += detecPercent *.0001;
    this->previousPlyrPos = player->position;
    printf("Danger!\n");
  }
  else if (this->totalDetLvl > 0) {
    this->totalDetLvl -= .05;
    this->totalDetLvl = this->totalDetLvl;
    //printf("Not Danger!\n");
  }
  clamp();
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
    this->totalDetLvl -= .02;
    this->totalDetLvl = this->totalDetLvl;
  }
  else if (player->velocity > 0.0){
    if (player->maxVelocity == WALK) {
      this->totalDetLvl += .0001;
    }
    else if (player->maxVelocity == RUN) {
      this->totalDetLvl += .002;
    }
    }
  if (this->totalDetLvl == 0){
    this->detecDanger = false;
  }
  this->previousPlyrPos = player->position;
  clamp();
}
