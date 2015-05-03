#include "DebugDraw.h"


DebugDraw::DebugDraw()
{
}

DebugDraw::DebugDraw(const std::string &vShaderName, const std::string &fShaderName)
{
  this->handles.installShaders(vShaderName, fShaderName);
}

void DebugDraw::addLine(LineSegment line)
{
}

void DebugDraw::addLine(glm::vec3 start, glm::vec3 end)
{
}

void DebugDraw::addRing(Ring3D ring)
{
}

void DebugDraw::addRing(glm::vec3 center, float radius, glm::vec3 normal)
{
}

void DebugDraw::drawAll()
{
  // Enable and bind position array for drawing
  GLSL::enableVertexAttribArray(handles.aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, this->posBufObj);
  glVertexAttribPointer(handles.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable and bind normal array for drawing
  GLSL::enableVertexAttribArray(handles.aColor);
  glBindBuffer(GL_ARRAY_BUFFER, this->colBufObj);
  glVertexAttribPointer(handles.aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
