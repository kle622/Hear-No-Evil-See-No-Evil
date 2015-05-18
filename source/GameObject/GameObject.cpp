#include "GameObject.h"
#include "Player.h"
#define DECELERATION 0.45f

GameObject::GameObject(Mesh *mesh,
		       vec3 position, float rotation, vec3 scale, 
		       vec3 direction, float velocity, vec3 dimensions,
           int scanRadius, int material, ObjectType type) {
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
  this->type = type;
}

/*void GameObject::draw() {
  //SetModel(handles->uModelMatrix, this->position, this->rotation, this->scale);
  this->mesh->drawObject(this->handles);
}*/

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
            
			// Pushing Pushables
			if (object->type == ObjectType::PUSHABLE &&
					((type == ObjectType::PLAYER && velocity > WALK)
						|| type != ObjectType::PLAYER)) {
				object->direction = (object->position - position + direction) * 0.5f;
				object->direction.y = 0.0f;
				object->direction = normalize(object->direction);
				object->velocity = velocity * dot(object->direction, direction);
            }

			// Smooth Sliding Collisions
			vec3 dir = position - object->position; // w2p
			dir.y = 0.0f;
			dir = normalize(dir);

			vec3 thisCorner = position;
			thisCorner.x -= sign(dir.x) * dimensions.x / 2;
			thisCorner.y = 0;
			thisCorner.z -= sign(dir.z) * dimensions.z / 2;

			vec3 posZnegX(-object->dimensions.x, 0, object->dimensions.z);
			vec3 negZnegX(-object->dimensions.x, 0, -object->dimensions.z);

			float cross1 = cross(posZnegX, dir).y;
			float cross2 = cross(negZnegX, dir).y;

			int signx = sign(dir.x);
			int signz = sign(dir.z);

			// Which axis can we not move on?
			if (cross1 * cross2 < 0) {
				// restrict x
				printf("RESTRICT X!\n");
				position.x = object->position.x + signx *
					(object->dimensions.x + dimensions.x) / 2;
			}
			else {
				// restrict z
				printf("RESTRICT Z!\n");
				position.z = object->position.z + signz *
					(object->dimensions.z + dimensions.z) / 2;
			}
            return true;
        }
    }

    return false;
}

/*void SetModel(GLint handle, vec3 trans, float rot, vec3 sc) {
    glm::mat4 Trans = glm::translate(glm::mat4(1.0f), trans);
    glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
    glm::mat4 Sc = glm::scale(glm::mat4(1.0f), sc);
    glm::mat4 com = Trans*RotateY*Sc;
    if (handle >= 0)
        glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(com));
	}*/
