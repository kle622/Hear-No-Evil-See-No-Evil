#include "Guard.h"
#include "Player.h"
#include <stdlib.h>
#include "../glm/glm.hpp"
#include "../glm/ext.hpp"
#include "../glm/core/_swizzle_func.hpp"

Guard::Guard(Mesh *mesh, vec3 scale, float velocity, vec3 dimensions,
	int scanRadius, int material, vector<PathNode> motionPath) :
	GameObject(mesh,
	motionPath[0].pos, 0, scale,
	normalize(motionPath[1].pos - motionPath[0].pos), velocity, dimensions,
	scanRadius, material) {
	this->motionPath = motionPath;
	currentNode = 0;
	pathDirection = sweepDirection = 1;
	moving = true;
	waitTime = 0.0f;
	originalMaterial = material;
}

void Guard::move(float time) {
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
	else if (waitTime < motionPath[currentNode].moveDelay)
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
	rotation = (atan2f(direction.x, direction.z) * 180 / 3.14159f);
}

bool Guard::collide(GameObject* object) {
	// moved old code from here down to detect. Should improve performance drastically.
	return false;
}

bool Guard::detect(Player* player) {
	// if object is within guard's cone of vision, return true
	if (dot(normalize(player->position - position), direction) > (1 - GUARD_VISION_RANGE)) {
		material = 3;
		//velocity = 0;
		return true;
	}
	else {
		material = originalMaterial;
		return false;
	}
	return false;
}
