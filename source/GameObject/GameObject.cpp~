#include "GameObject.h"

GameObject::GameObject(GLint modelHandle, 
    vec3 position, float rotation, float scale, 
    vec3 direction, float velocity, vec3 dimensions, int indices, 
    GLuint posBuffer, GLuint norBuffer, GLuint indBuffer,
    GLint posHandle, GLint norHandle, int material = 0) {
    this->modelHandle = modelHandle;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    this->direction = normalize(direction);
    this->velocity = velocity;
    this->dimensions = dimensions;
    this->indices = indices;
    this->posBuffer = posBuffer;
    this->norBuffer = norBuffer;
    this->indBuffer = indBuffer;
    this->posHandle = posHandle;
    this->norHandle = norHandle;
    this->material = material;
    this->alive = true;
}

void GameObject::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glVertexAttribPointer(posHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, norBuffer);
    glVertexAttribPointer(norHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffer);

    SetModel(modelHandle, position, rotation, scale);
    glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
}

void GameObject::move(float time) {
    vec3 oldPosition = position;
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
    //WRITE YOUR OWN DAMN COLLIDE
    return true;
}

void SetModel(GLint handle, vec3 trans, float rot, float sc) {
    glm::mat4 Trans = glm::translate(glm::mat4(1.0f), trans);
    glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
    glm::mat4 Sc = glm::scale(glm::mat4(1.0f), vec3(sc));
    glm::mat4 com = Trans*RotateY*Sc;
    if (handle >= 0)
        glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(com));
}





