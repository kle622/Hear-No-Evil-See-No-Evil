#include "DebugDraw.h"

DebugDraw::DebugDraw()
{
}

// TODO not supported
void DebugDraw::addLine(LineSegment line)
{
}

// TODO not supported
void DebugDraw::addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color)
{
  this->posBuf.push_back(start.x);
  this->posBuf.push_back(start.y);
  this->posBuf.push_back(start.z);
  this->posBuf.push_back(end.x);
  this->posBuf.push_back(end.y);
  this->posBuf.push_back(end.z);
  this->colBuf.push_back(color.x);
  this->colBuf.push_back(color.y);
  this->colBuf.push_back(color.z);
  this->colBuf.push_back(color.x);
  this->colBuf.push_back(color.y);
  this->colBuf.push_back(color.z);
}

// TODO not supported
void DebugDraw::addRing(Ring3D ring)
{
}

// TODO not supported
void DebugDraw::addRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color)
{
}

// TODO not supported
void DebugDraw::drawAll()
{
  glUseProgram(handles.prog);

  // Send the index array to the GPU
  glGenBuffers(1, &this->posBufObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->posBufObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->posBuf.size()*sizeof(float), &this->posBuf[0], GL_STATIC_DRAW);

  // Send the index array to the GPU
  glGenBuffers(1, &this->colBufObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->colBufObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->colBuf.size()*sizeof(float), &this->colBuf[0], GL_STATIC_DRAW);

  // Enable and bind position array for drawing
  GLSL::enableVertexAttribArray(handles.aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
  glVertexAttribPointer(handles.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable and bind color array for drawing
  GLSL::enableVertexAttribArray(handles.aColor);
  glBindBuffer(GL_ARRAY_BUFFER, this->colBufObj);
  glVertexAttribPointer(handles.aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // send view/projection
  glUniformMatrix4fv(this->handles.uViewMatrix, 1, GL_FALSE, glm::value_ptr(this->view));
  glUniformMatrix4fv(this->handles.uProjMatrix, 1, GL_FALSE, glm::value_ptr(this->projection));

  glDrawElements(GL_LINES, this->posBuf.size(), GL_UNSIGNED_INT, 0);

  // Disable and unbind
  GLSL::disableVertexAttribArray(this->handles.aPosition);
  GLSL::disableVertexAttribArray(this->handles.aColor);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glUseProgram(0);
}
