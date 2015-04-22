#include "Guard.h"
#include "Player.h"
#include <stdlib.h>
#include "../glm/glm.hpp"
#include "../glm/ext.hpp"
#include "../glm/core/_swizzle_func.hpp"

Guard::Guard(Mesh *mesh, Handles *handles, vec3 scale, float velocity, vec3 dimensions,
	int material, int scanRadius, vector<vec3> motionPath) :
	GameObject(mesh, handles,
	motionPath[0], 0, scale,
	normalize(motionPath[1] - motionPath[0]), velocity, dimensions,
	material, scanRadius) {
	this->motionPath = motionPath;
	currentNode = 0;
	endpointDirection = pathDirection = sweepDirection = 1;
	moving = true;
}

void Guard::move(float time) {
	if (moving) { // moving between path nodes
		//set movement direction
		direction = normalize(motionPath[currentNode + pathDirection] - position);
		position += direction * velocity * time;

		//check if we're at next node now
		if (dot(direction, normalize(motionPath[currentNode + pathDirection] - position)) < 0) {
			printf("Advanced to next node, current = %d, next = %d\n", currentNode, currentNode + pathDirection);
			moving = false;
			currentNode += pathDirection;
			position = motionPath[currentNode];
			if (currentNode == 0 || currentNode == motionPath.size() - 1) { // endpoint, switch pathDirection
				pathDirection *= -1;
			}
			else {
				printf("Using sweepDirection...\n");
				sweepDirection = (int)sign(cross(direction, motionPath[currentNode + pathDirection] - motionPath[currentNode]).y);
			}
		}
	}
	else { // at a path node, re-orienting to face next path node
		//printf("Endpoint? %s\n", (currentNode == 0 || currentNode == motionPath.size() - 1) ? "yes" : "no");
		int rotateDirection = (currentNode == 0 || currentNode == motionPath.size() - 1) ? endpointDirection : sweepDirection;
		direction = vec3(glm::rotateY(vec4(direction, 0), 10.0f * velocity * time * rotateDirection));

		// check to see if we are now oriented correctly
		float cross_y = cross(direction, motionPath[currentNode + pathDirection] - motionPath[currentNode]).y;
		//printf("ROTATING: cross_y = %.3f, sweepDirection = %d\n", cross_y, rotateDirection);
		if (cross_y * rotateDirection < 0) { // positive value means sweepDirection was clockwise
			if (currentNode == 0 || currentNode == motionPath.size() - 1)
				endpointDirection *= -1;
			moving = true;
		}
	}
	rotation = (atan2f(direction.x, direction.z) * 180 / 3.14159f);
}

bool Guard::collide(GameObject* object) {
	// if object is within guard's cone of vision, return true
	if (dot(normalize(object->position - position), direction) > (1 - GUARD_VISION_RANGE)) {
		material = 3;
		velocity = 0;
		return true;
	}
	else {
		material = 1;
		return false;
	}
	return false;
}
