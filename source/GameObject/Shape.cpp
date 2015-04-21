#include "Shape.h"

Shape::Shape(Handles *handles, vec3 position, float rotation, 
    vec3 scale, vec3 direction, float velocity, int indices, 
   GLuint posBuffer, GLuint norBuffer,int material = 0) {
  
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

    SetModel(handles->uModelMatrix, position, rotation, scale);
    glDrawArrays(GL_TRIANGLES, 0, indices);
}

void SetModel(GLint handle, vec3 trans, float rot, vec3 sc) {
  glm::mat4 Trans = glm::translate(glm::mat4(1.0f), trans);
  glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
  glm::mat4 Sc = glm::scale(glm::mat4(1.0f), sc);
  glm::mat4 com = Trans*RotateY*Sc;
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(com));
}

