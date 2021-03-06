#include "DebugDraw.h"

bool DebugDraw::installShaders(const std::string &vShaderName, const std::string &fShaderName)
{
  return this->handles.installShaders(vShaderName, fShaderName);
}

void DebugDraw::addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color, bool thick)
{
  if (thick) {
    this->posBufThick.push_back(start.x);
    this->posBufThick.push_back(start.y);
    this->posBufThick.push_back(start.z);
    this->posBufThick.push_back(end.x);
    this->posBufThick.push_back(end.y);
    this->posBufThick.push_back(end.z);
    this->colBufThick.push_back(color.x);
    this->colBufThick.push_back(color.y);
    this->colBufThick.push_back(color.z);
    this->colBufThick.push_back(color.x);
    this->colBufThick.push_back(color.y);
    this->colBufThick.push_back(color.z);
  }
  else {
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
}

// assumes normal is not <0, 0, 0>
void DebugDraw::addRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color, bool thick)
{
  normal = glm::normalize(normal);
  glm::vec4 out;
  glm::vec4 prevOut;
  if (normal.x < EPS && normal.x > -1.0f * EPS) {
    out = glm::vec4(radius, 0.0f, 0.0f, 1.0f);
  }
  else if (normal.y < EPS && normal.y > -1.0f * EPS) {
    out = glm::vec4(0.0f, radius, 0.0f, 1.0f);
  }
  else if (normal.z < EPS && normal.z > -1.0f * EPS) {
    out = glm::vec4(0.0f, 0.0f, radius, 1.0f);
  }
  else {
    out = radius * glm::vec4(glm::normalize(glm::vec3(normal.x, -1.0f * normal.y, 0.0f)), 1.0f);
  }
  for (int i = 0;  i <= SUBDIVISIONS; ++i) {
    prevOut = out;
    out = glm::rotate(glm::mat4(1.0f), 360.0f / SUBDIVISIONS, normal) * out;
    this->addLine(center + glm::vec3(prevOut), center + glm::vec3(out), color, thick);
  }
}

void DebugDraw::addOBB(OBB obb, glm::vec3 color, bool thick, bool cross)
{
  glm::vec3 topleftfront = obb.center + obb.axes[0] * obb.halfLengths[0]  // x
    - obb.axes[1] * obb.halfLengths[1]  // y
    - obb.axes[2] * obb.halfLengths[2]; // z
  glm::vec3 topleftback = obb.center + obb.axes[0] * obb.halfLengths[0]
    - obb.axes[1] * obb.halfLengths[1]
    + obb.axes[2] * obb.halfLengths[2];
  glm::vec3 toprightfront = obb.center + obb.axes[0] * obb.halfLengths[0]
    + obb.axes[1] * obb.halfLengths[1]
    - obb.axes[2] * obb.halfLengths[2];
  glm::vec3 toprightback = obb.center + obb.axes[0] * obb.halfLengths[0]
    + obb.axes[1] * obb.halfLengths[1]
    + obb.axes[2] * obb.halfLengths[2];
  glm::vec3 botleftfront = obb.center - obb.axes[0] * obb.halfLengths[0]
    - obb.axes[1] * obb.halfLengths[1]
    - obb.axes[2] * obb.halfLengths[2];
  glm::vec3 botleftback = obb.center - obb.axes[0] * obb.halfLengths[0]
    - obb.axes[1] * obb.halfLengths[1]
    + obb.axes[2] * obb.halfLengths[2];
  glm::vec3 botrightfront = obb.center - obb.axes[0] * obb.halfLengths[0]
    + obb.axes[1] * obb.halfLengths[1]
    - obb.axes[2] * obb.halfLengths[2];
  glm::vec3 botrightback = obb.center - obb.axes[0] * obb.halfLengths[0]
    + obb.axes[1] * obb.halfLengths[1]
    + obb.axes[2] * obb.halfLengths[2];

  this->addBox(topleftfront,
               topleftback,
               toprightfront,
               toprightback,
               botleftfront,
               botleftback,
               botrightfront,
               botrightback,
               color,
               thick,
               cross);
}

void DebugDraw::addBox(glm::vec3 topleftfront,
                       glm::vec3 topleftback,
                       glm::vec3 toprightfront,
                       glm::vec3 toprightback,
                       glm::vec3 botleftfront,
                       glm::vec3 botleftback,
                       glm::vec3 botrightfront,
                       glm::vec3 botrightback,
                       glm::vec3 color,
                       bool thick, bool cross)
{
  // main wireframe lines
  this->addLine(topleftfront, topleftback, color, thick);
  this->addLine(topleftfront, toprightfront, color, thick);
  this->addLine(topleftfront, botleftfront, color, thick);
  this->addLine(topleftback, toprightback, color, thick);
  this->addLine(topleftback, botleftback, color, thick);
  this->addLine(toprightfront, botrightfront, color, thick);
  this->addLine(toprightfront, toprightback, color, thick);
  this->addLine(toprightback, botrightback, color, thick);
  this->addLine(botleftfront, botrightfront, color, thick);
  this->addLine(botleftfront, botleftback, color, thick);
  this->addLine(botrightfront, botrightback, color, thick);
  this->addLine(botleftback, botrightback, color, thick);

  // diagonals
  if (cross) {
    this->addLine(topleftfront, toprightback, color, thick);  // top face x
    this->addLine(toprightfront, topleftback, color, thick);
    this->addLine(topleftfront, botleftback, color, thick);   // left face x
    this->addLine(topleftback, botleftfront, color, thick);
    this->addLine(topleftback, botrightback, color, thick);   // back face x
    this->addLine(botleftback, toprightback, color, thick);
    this->addLine(toprightfront, botrightback, color, thick);   // right face x
    this->addLine(toprightback, botrightfront, color, thick);
    this->addLine(toprightfront, botleftfront, color, thick);   // front face x
    this->addLine(topleftfront, botrightfront, color, thick);
    this->addLine(botrightfront, botleftback, color, thick);   // bottom face x
    this->addLine(botrightback, botleftfront, color, thick);
  }
}

void DebugDraw::addBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color, bool thick, bool cross)
{
  glm::vec3 halfDims = dimensions * 0.5f;
  glm::vec3 topleftfront = center + glm::vec3(-1.0f * halfDims.x, halfDims.y, -1.0f * halfDims.z);
  glm::vec3 topleftback = center + glm::vec3(-1.0f * halfDims.x, halfDims.y, halfDims.z);
  glm::vec3 toprightfront = center + glm::vec3(halfDims.x, halfDims.y, -1.0f * halfDims.z);
  glm::vec3 toprightback = center + glm::vec3(halfDims.x, halfDims.y, halfDims.z);
  glm::vec3 botleftfront = center + glm::vec3(-1.0f * halfDims.x, -1.0f * halfDims.y, -1.0f * halfDims.z);
  glm::vec3 botleftback = center + glm::vec3(-1.0f * halfDims.x, -1.0f * halfDims.y, halfDims.z);
  glm::vec3 botrightfront = center + glm::vec3(halfDims.x, -1.0f * halfDims.y, -1.0f * halfDims.z);
  glm::vec3 botrightback = center + glm::vec3(halfDims.x, -1.0f * halfDims.y, halfDims.z);

  this->addBox(topleftfront,
               topleftback,
               toprightfront,
               toprightback,
               botleftfront,
               botleftback,
               botrightfront,
               botrightback,
               color,
               thick,
               cross);
}

// assumes planes are not parallel
void DebugDraw::addBox(glm::vec4 left, glm::vec4 right, glm::vec4 bottom, glm::vec4 top, glm::vec4 front, glm::vec4 back, glm::vec3 color, bool thick, bool cross)
{
  glm::vec3 topleftfront = intersectPlanes(top, left, front);
  glm::vec3 topleftback = intersectPlanes(top, left, back);
  glm::vec3 toprightfront = intersectPlanes(top, right, front);
  glm::vec3 toprightback = intersectPlanes(top, right, back);
  glm::vec3 botleftfront = intersectPlanes(bottom, left, front);
  glm::vec3 botleftback = intersectPlanes(bottom, left, back);
  glm::vec3 botrightfront = intersectPlanes(bottom, right, front);
  glm::vec3 botrightback = intersectPlanes(bottom, right, back);

  this->addBox(topleftfront,
               topleftback,
               toprightfront,
               toprightback,
               botleftfront,
               botleftback,
               botrightfront,
               botrightback,
               color,
               thick,
               cross);
}

void DebugDraw::clear()
{
  this->posBuf.clear();
  this->colBuf.clear();
  this->posBufThick.clear();
  this->colBufThick.clear();
}

void DebugDraw::drawAll()
{
  glUseProgram(handles.prog);

  if (this->posBuf.size() > 0 && this->colBuf.size() > 0) {
    // send view/projection
    glUniformMatrix4fv(this->handles.uViewMatrix, 1, GL_FALSE, glm::value_ptr(this->view));
    glUniformMatrix4fv(this->handles.uProjMatrix, 1, GL_FALSE, glm::value_ptr(this->projection));

    // Send the index array to the GPU
    glGenBuffers(1, &this->posBufObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->posBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->posBuf.size()*sizeof(float), &this->posBuf[0], GL_STATIC_DRAW);

    // Send the color array to the GPU
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

    // draw
    glLineWidth(2);
    glDrawArrays(GL_LINES, 0, this->posBuf.size());
  }

  if (this->posBufThick.size() > 0 && this->colBufThick.size() > 0) {
    // Send the index array to the GPU (thick)
    glGenBuffers(1, &this->posBufObjThick);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->posBufObjThick);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->posBufThick.size()*sizeof(float), &this->posBufThick[0], GL_STATIC_DRAW);

    // Send the color array to the GPU (thick)
    glGenBuffers(1, &this->colBufObjThick);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->colBufObjThick);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->colBufThick.size()*sizeof(float), &this->colBufThick[0], GL_STATIC_DRAW);

    // Enable and bind position array for drawing (thick)
    GLSL::enableVertexAttribArray(handles.aPosition);
    glBindBuffer(GL_ARRAY_BUFFER, this->posBufObjThick);
    glVertexAttribPointer(handles.aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable and bind color array for drawing (thick)
    GLSL::enableVertexAttribArray(handles.aColor);
    glBindBuffer(GL_ARRAY_BUFFER, this->colBufObjThick);
    glVertexAttribPointer(handles.aColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // draw (thick)
    glLineWidth(6);
    glDrawArrays(GL_LINES, 0, this->posBufThick.size());

    // Disable and unbind
    GLSL::disableVertexAttribArray(this->handles.aPosition);
    GLSL::disableVertexAttribArray(this->handles.aColor);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  glUseProgram(0);
}
