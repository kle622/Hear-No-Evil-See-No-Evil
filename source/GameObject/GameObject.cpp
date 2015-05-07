#include "GameObject.h"

GameObject::GameObject(Mesh *mesh, Handles *handles,
		       vec3 position, float rotation, vec3 scale, 
		       vec3 direction, float velocity, vec3 dimensions,
           int scanRadius, int material = 0) {
  this->mesh = mesh;
  this->handles = handles;
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
}

void GameObject::draw() {
  SetModel(handles->uModelMatrix, this->position, this->rotation, this->scale);
  this->mesh->drawObject(this->handles);
}

void GameObject::move(float time) {
    position += normalize(direction) * velocity * time;
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
	printf("STOP CALLING GAMEOBJECT'S COLLIDE METHOD!\n");
    return false;
}

void SetModel(GLint handle, vec3 trans, float rot, vec3 sc) {
    glm::mat4 Trans = glm::translate(glm::mat4(1.0f), trans);
    glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
    glm::mat4 Sc = glm::scale(glm::mat4(1.0f), sc);
    glm::mat4 com = Trans*RotateY*Sc;
    if (handle >= 0)
        glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(com));
}





