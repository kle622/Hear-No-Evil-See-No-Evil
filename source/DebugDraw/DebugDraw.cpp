#include "DebugDraw.h"

DebugDraw::DebugDraw()
{
}

DebugDraw::DebugDraw(const std::string &vShaderName, const std::string &fShaderName)
{
  this->handles.installShaders(vShaderName, fShaderName);
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

  // Enable and bind position array for drawing
  GLSL::enableVertexAttribArray(handles.aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
  glVertexAttribPointer(handles.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable and bind color array for drawing
  GLSL::enableVertexAttribArray(handles.aColor);
  glBindBuffer(GL_ARRAY_BUFFER, this->colBufObj);
  glVertexAttribPointer(handles.aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
