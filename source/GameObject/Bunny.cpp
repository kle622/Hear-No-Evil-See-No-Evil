#include "Bunny.h"
#include "Player.h"

Bunny::Bunny(GLint modelHandle, 
    vec3 position, float rotation, float scale, 
    vec3 direction, float velocity, vec3 dimensions, 
    float scanRadius, int indices,
    GLuint posBuffer, GLuint norBuffer, GLuint indBuffer,
    GLint posHandle, GLint norHandle, int material = 0) : 
    GameObject(modelHandle, position, rotation, scale, 
    direction, velocity, dimensions, 
    scanRadius, indices, posBuffer, norBuffer,
    indBuffer, posHandle, norHandle, material) {

}

void Bunny::move(float time) {
    if (position.x >= 29.0f) {
        position.x = 29.0f;
        direction.x = -direction.x;
    }
    if (position.x <= -29.0f) {
        position.x = -29.0f;
        direction.x = -direction.x;
    }
    if (position.z >= 29.0f) {
        position.z = 29.0f;
        direction.z = -direction.z;
    }
    if (position.z <= -29.0f) {
        position.z = -29.0f;
        direction.z = -direction.z;
    }

    position += normalize(direction) * velocity * time;
    rotation = 90 + (atan2f(direction.x, direction.z) * 180 / M_PI);
}

bool Bunny::collide(GameObject* object) {
    float thisRadius = dimensions.x + dimensions.y + dimensions.z;
    float objectRadius = object->dimensions.x + object->dimensions.y + 
        object->dimensions.z;

    if (compareDistance(position, object->position, thisRadius + objectRadius)) {
        if (intersect(position.x, object->position.x, dimensions.x, object->dimensions.x) &&
            intersect(position.y, object->position.y, dimensions.y, object->dimensions.y) &&
            intersect(position.z, object->position.z, dimensions.z, object->dimensions.z) &&
            dot(object->position - position, direction) > 0) {
            if (dynamic_cast<Player*>(object)) {
                alive = false;
                material = 0;
                dynamic_cast<Player*>(object)->score++;
            } 
            direction = -direction;
            return true;
        }
    }

    return false;
}
