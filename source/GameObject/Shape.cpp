#include "Shape.h"

Shape::Shape(vec3 position,
    vec3 scale, vec3 direction, float velocity, int indices, 
   GLuint posBuffer, GLuint norBuffer,int material = 0) {
  
  this->position = position;
  this->scale = scale;
  this->direction = direction;
  this->velocity = velocity;
  this->indices = indices;
  this->posBuffer = posBuffer;
  this->norBuffer = norBuffer;
  this->texBuffer = 0;
  this->idxBuffer = 0;
  this->material = material;
}


Shape::Shape(vec3 position,
    vec3 scale, vec3 direction, float velocity, int indices, 
	     GLuint posBuffer, GLuint norBuffer, GLuint idxBuffer, GLuint texBuffer, int material = 0) {
  
  this->position = position;
  this->scale = scale;
  this->direction = direction;
  this->velocity = velocity;
  this->indices = indices;
  this->posBuffer = posBuffer;
  this->norBuffer = norBuffer;
  this->idxBuffer = idxBuffer;
  this->texBuffer = texBuffer;
  this->material = material;
}

/*void Shape::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glVertexAttribPointer(handles->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, norBuffer);
    glVertexAttribPointer(handles->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    SetModel(handles->uModelMatrix, position, rotation, scale);
    glDrawArrays(GL_TRIANGLES, 0, indices);
}

void Shape::SetModel(GLint handle, vec3 trans, float rot, vec3 sc) {
  glm::mat4 Trans = glm::translate(glm::mat4(1.0f), trans);
  glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0));
  glm::mat4 Sc = glm::scale(glm::mat4(1.0f), sc);
  glm::mat4 com = Trans*RotateY*Sc;
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(com));
    }*/

void Shape::loadMipmapTexture(const std::string &filename) {
  bmp = imageLoad(filename.c_str());
  printf("loaded file in mip map texture function\n");
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  printf("before gltexImage2D call\n");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, MIP_MAP_SIZE, MIP_MAP_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp);
  printf("after gltexImage2D call\n");
  //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEX_SIZE, TEX_SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, bmp);
  glGenerateMipmap(GL_TEXTURE_2D);
  printf("after glgeneratemipmap call\n");

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  printf("after glTexParameter calls\n");
  
}

void Shape::loadTexture(const std::string &filename) {
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  bmp = imageLoad(filename.c_str());
  glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP_SIZE, BMP_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp);
 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
