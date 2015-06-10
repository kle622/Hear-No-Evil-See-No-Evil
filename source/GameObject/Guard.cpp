#include "Guard.h"
#include "Player.h"
#include <stdlib.h>
#include "../glm/glm.hpp"
#include "../glm/ext.hpp"
#include "../glm/core/_swizzle_func.hpp"

Guard::Guard(Mesh *mesh, vec3 scale, float velocity, vec3 dimensions,
	int scanRadius, int material, vector<PathNode> motionPath, Player *player) :
	GameObject(mesh,
	motionPath[0].pos, scale, 0,
	normalize(motionPath[1].pos - motionPath[0].pos), velocity, dimensions,
	scanRadius, material, GameObject::ObjectType::GUARD) {
	this->motionPath = motionPath;
	currentNode = 0;
	pathDirection = sweepDirection = 1;
	moving = true;
	waitTime = 0.0f;
	originalMaterial = material;
	staring = false;
	playerObject = player;
	foundPlayer = false;
	playerTimer = 0.0f;
}
#define GUARD_INTEREST 3.0f
void Guard::move(float time) {
	if (staring) { // the guard has become aware of the player, turn to investigate
		vec3 gtop = lastSeen - position; // guard to player vector
		gtop.y = 0;
		gtop = normalize(gtop);
		int stareTurnDir = (int)sign(cross(direction, gtop).y);
		if (!foundPlayer) { // turn to initially center player in view
			direction = vec3(glm::rotateY(vec4(direction, 0), GUARD_SPIN_SPEED * velocity * time * stareTurnDir));
			gtop = playerObject->position - position;
			float cross_y = cross(direction, gtop).y;
			if (cross_y * stareTurnDir < 0) { // we see the player, now just set the direction to be the vector to the player
				foundPlayer = true;
				//lastSeen = vec3(playerObject->position.x, 0, playerObject->position.z);
			}
		}
		if (foundPlayer) { // check to see if we should turn to follow the player
			// we somehow want to stop following the player if we lose sight
			gtop = lastSeen - position; // guard to player vector
			gtop.y = 0;
			gtop = normalize(gtop);
			direction = gtop;
			playerTimer += time;
			if (playerTimer > GUARD_INTEREST) { // the guard has been staring at this spot long enough to forget about the player, go back to moving normally
				playerTimer = 0.0f;
				staring = false;
			}
		}
	}
	if (!staring) { // we are following the guardPath normally
		oldPosition = position;
		if (moving) { // moving between path nodes
			//set movement direction
			direction = normalize(motionPath[currentNode + pathDirection].pos - position);
			position += direction * velocity * time;

			//check if we're at next node now
			if (dot(direction, normalize(motionPath[currentNode + pathDirection].pos - position)) < 0) {
				moving = false;
				currentNode += pathDirection;
				position = motionPath[currentNode].pos;
				if (motionPath[currentNode].end) { // endpoint, switch pathDirection
					pathDirection *= -1;
				}
				else {
					sweepDirection = (int)sign(cross(direction, motionPath[currentNode + pathDirection].pos - motionPath[currentNode].pos).y);
				}
			}
		}
		else if (waitTime < motionPath[currentNode].moveDelay) // delay until we continue to next node
		{
			waitTime += time;
		}
		else { // at a path node, re-orienting to face next path node
			int rotateDirection = (motionPath[currentNode].end) ? motionPath[currentNode].endTurnDir : sweepDirection;
			direction = vec3(glm::rotateY(vec4(direction, 0), GUARD_SPIN_SPEED * velocity * time * rotateDirection));

			// check to see if we are now oriented correctly
			float cross_y = cross(direction, motionPath[currentNode + pathDirection].pos - motionPath[currentNode].pos).y;
			//printf("ROTATING: cross_y = %.3f, sweepDirection = %d\n", cross_y, rotateDirection);
			if (cross_y * rotateDirection < 0) { // positive value means sweepDirection was clockwise
				moving = true;
				waitTime = 0.0f;
			}

			if (motionPath[currentNode].smoothTurn) {
				position += direction * velocity * time / 3.0f;
			}
		}
	}
}

bool Guard::collide(GameObject* object) {
    float thisRadius = dimensions.x + dimensions.y + dimensions.z;
    float objectRadius = object->dimensions.x + object->dimensions.y + 
        object->dimensions.z;

    if (compareDistance(position, object->position, thisRadius + objectRadius)) {
        if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
            intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
            intersect(position.z, object->position.z, dimensions.z, object->dimensions.z)) {
            /**if (object->pushable) {
                object->velocity = this->velocity;
                object->direction = this->direction;
            }*/
            //position = oldPosition;
            return true;
        }
    }

    return false;
}

float getDistToGuard(vec3 playerPos, vec3 guardPos) {
  float deltaX = playerPos.x - guardPos.x;
  float deltaZ = playerPos.z - guardPos.z;

  float dist = deltaX * deltaX + deltaZ * deltaZ;

  return dist;
}

float Guard::detect(WorldGrid *world, DetectionCamera *cam, DetectionTracker *detecTrac) {
  float viewPercent;
  cam->update(this);
  viewPercent = cam->percentInView(world, this, playerObject);
  // if object is within guard's cone of vision, return true
  if (viewPercent > 0) {
    material = 3;
    //velocity = 0;
  }
  else {
    material = originalMaterial;
  }
  detecTrac->guardDist = getDistToGuard(playerObject->position, this->position);

  return viewPercent;
}

glm::mat4 Guard::getModel()
{
  glm::mat4 Trans = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 Rot = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(direction.x, direction.y, -1.0f * direction.z), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 Scale = glm::scale(glm::mat4(1.0f), scale);
  glm::mat4 com = Trans*Rot*Scale;

  return com;
}

void Guard::stare() {
	//face towards the player
	//	set a flag for move method to lerp to face player
	staring = true;
	playerTimer = 0.0f;
	lastSeen = vec3(playerObject->position.x, 0, playerObject->position.z);
}