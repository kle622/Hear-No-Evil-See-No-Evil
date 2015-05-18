#include "GameObject.h"
#include "Player.h"
#define DECELERATION 0.45f

GameObject::GameObject(Mesh *mesh,
		       vec3 position, float rotation, vec3 scale, 
		       vec3 direction, float velocity, vec3 dimensions,
           int scanRadius, int material = 0, bool pushable = false) {
  this->mesh = mesh;
  this->position = position;
  this->oldPosition = position;
  this->rotation = rotation;
  this->scale = scale;
  this->direction = normalize(direction);
  this->velocity = velocity;
  this->dimensions = dimensions;
  this->scanRadius = scanRadius;
  this->material = material;
  this->alive = true;
  this->pushable = pushable;
}

void GameObject::move(float time) {
    oldPosition = position;
    position += normalize(direction) * velocity * time;
    if (velocity > 0) {
      velocity -= DECELERATION;
    }
#ifdef WIN32
    velocity = max(velocity, 0.0f);
#else
    velocity = std::max(velocity, 0.0f);
#endif
}

bool compareDistance(vec3 first, vec3 second, float max) {
    float deltaX = first.x - second.x;
    float deltaY = first.y - second.y;
    float deltaZ = first.z - second.z;

    float dist = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;

    return dist < max * max;
}

bool intersect(float point1, float point2, float dim1, float dim2) {
    return abs(point2 - point1) < ((dim1 + dim2) / 2);
}

bool GameObject::collide(GameObject* object) {
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
            position = oldPosition;
            return true;
        }
    }

    return false;
}





