#include "Shape.h"

Shape::Shape(Handles *handles, vec3 position, float rotation, 
    vec3 scale, vec3 direction, float velocity, int indices, GLuint posBuffer, GLuint norBuffer,
    GLint posHandle, GLint norHandle, int material = 0) {
  
  this->handles = handles;
  this->position = position;
  this->rotation = rotation;
  this->scale = scale;
  this->direction = direction;
  this->velocity = velocity;
  this->indices = indices;
  this->posBuffer = posBuffer;
  this->norBuffer = norBuffer;
  this->material = material;
}

void Shape::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glVertexAttribPointer(handles->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, norBuffer);
    glVertexAttribPointer(handles->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    SetModel(modelHandle, position, rotation, scale);
    glDrawArrays(GL_TRIANGLES, 0, indices);
}
