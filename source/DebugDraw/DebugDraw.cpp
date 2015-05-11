#include "DebugDraw.h"

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

void DebugDraw::addOBB(OBB obb, glm::vec3 color, bool thick)
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
               thick);
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
                       bool thick)
{
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
}

void DebugDraw::addBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color, bool thick)
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
               thick);
}

// assumes planes are not parallel
void DebugDraw::addBox(glm::vec4 left, glm::vec4 right, glm::vec4 bottom, glm::vec4 top, glm::vec4 front, glm::vec4 back, glm::vec3 color, bool thick)
{
  glm::vec3 topleftfront = intersectPlanes(top, left, front);
  //std::cout << "topleftfront: <" << topleftfront.x << ", " << topleftfront.y << ", " << topleftfront.z << ">" << std::endl;
  glm::vec3 topleftback = intersectPlanes(top, left, back);
  //std::cout << "topleftback: <" << topleftback.x << ", " << topleftback.y << ", " << topleftback.z << ">" << std::endl;
  glm::vec3 toprightfront = intersectPlanes(top, right, front);
  //std::cout << "toprightfront: <" << toprightfront.x << ", " << toprightfront.y << ", " << toprightfront.z << ">" << std::endl;
  glm::vec3 toprightback = intersectPlanes(top, right, back);
  //std::cout << "toprightback: <" << toprightback.x << ", " << toprightback.y << ", " << toprightback.z << ">" << std::endl;
  glm::vec3 botleftfront = intersectPlanes(bottom, left, front);
  //std::cout << "botleftfront: <" << botleftfront.x << ", " << botleftfront.y << ", " << botleftfront.z << ">" << std::endl;
  glm::vec3 botleftback = intersectPlanes(bottom, left, back);
  //std::cout << "botleftback: <" << botleftback.x << ", " << botleftback.y << ", " << botleftback.z << ">" << std::endl;
  glm::vec3 botrightfront = intersectPlanes(bottom, right, front);
  //std::cout << "botrightfront: <" << botrightfront.x << ", " << botrightfront.y << ", " << botrightfront.z << ">" << std::endl;
  glm::vec3 botrightback = intersectPlanes(bottom, right, back);
  //std::cout << "botrightback: <" << botrightback.x << ", " << botrightback.y << ", " << botrightback.z << ">" << std::endl;

  this->addBox(topleftfront,
               topleftback,
               toprightfront,
               toprightback,
               botleftfront,
               botleftback,
               botrightfront,
               botrightback,
               color,
               thick);
}

glm::vec4 DebugDraw::normalizePlane(glm::vec4 plane)
{
  //std::cout << "plane: <" << plane.x << ", " << plane.y << ", " << plane.z << ", " << plane.w << ">" << std::endl;
  glm::vec3 n = glm::vec3(plane);
  //std::cout << "n: <" << n.x << ", " << n.y << ", " << n.z << ">" << std::endl;
  float l = glm::length(n);
  //std::cout << "l: " << l << std::endl;
  float d = plane.w / l;
  //std::cout << "d: " << d << std::endl;
  n = glm::normalize(n);
  //std::cout << "n: <" << n.x << ", " << n.y << ", " << n.z << ">" << std::endl;

  return glm::vec4(n, d);
}

glm::vec3 DebugDraw::getPlanePoint(glm::vec4 plane)
{
  return -1.0f * plane.w * glm::vec3(plane.x, plane.y, plane.z);
}

// does not assume normalized planes
// assumes planes are not parallel
// book p. 783, equation 16.59
glm::vec3 DebugDraw::intersectPlanes(glm::vec4 plane1, glm::vec4 plane2, glm::vec4 plane3)
{
  // normalize planes
  glm::vec4 pl1 = this->normalizePlane(plane1);
  glm::vec4 pl2 = this->normalizePlane(plane2);
  glm::vec4 pl3 = this->normalizePlane(plane3);

  // get point on each plane
  glm::vec3 p1 = this->getPlanePoint(pl1);
  glm::vec3 p2 = this->getPlanePoint(pl2);
  glm::vec3 p3 = this->getPlanePoint(pl3);

  /*glm::vec3 p1 = this->getPlanePoint(plane1);
  glm::vec3 p2 = this->getPlanePoint(plane2);
  glm::vec3 p3 = this->getPlanePoint(plane3);*/

  // get plane normals
  glm::vec3 n1 = glm::vec3(pl1);
  glm::vec3 n2 = glm::vec3(pl2);
  glm::vec3 n3 = glm::vec3(pl3);

  /*std::cout << "plane1: <" << plane1.x << ", " << plane1.y << ", " << plane1.z << ", " << plane1.w << ">" << std::endl;
  std::cout << "pl1: <" << pl1.x << ", " << pl1.y << ", " << pl1.z << ", " << pl1.w << ">" << std::endl;
  std::cout << "p1: <" << p1.x << ", " << p1.y << ", " << p1.z << ">" << std::endl;
  std::cout << "n1: <" << n1.x << ", " << n1.y << ", " << n1.z << ">" << std::endl;

  std::cout << "plane2: <" << plane2.x << ", " << plane2.y << ", " << plane2.z << ", " << plane2.w << ">" << std::endl;
  std::cout << "pl2: <" << pl2.x << ", " << pl2.y << ", " << pl2.z << ", " << pl2.w << ">" << std::endl;
  std::cout << "p2: <" << p2.x << ", " << p2.y << ", " << p2.z << ">" << std::endl;
  std::cout << "n2: <" << n2.x << ", " << n2.y << ", " << n2.z << ">" << std::endl;

  std::cout << "plane3: <" << plane3.x << ", " << plane3.y << ", " << plane3.z << ", " << plane3.w << ">" << std::endl;
  std::cout << "pl3: <" << pl3.x << ", " << pl3.y << ", " << pl3.z << ", " << pl3.w << ">" << std::endl;
  std::cout << "p3: <" << p3.x << ", " << p3.y << ", " << p3.z << ">" << std::endl;
  std::cout << "n3: <" << n3.x << ", " << n3.y << ", " << n3.z << ">" << std::endl;*/

  /*glm::vec3 n1 = glm::vec3(plane1);
  glm::vec3 n2 = glm::vec3(plane2);
  glm::vec3 n3 = glm::vec3(plane3);*/

  //glm::mat3 matrix = glm::mat3(n1, n2, n3);
  // get determinant, assert nonzero
  //float det = glm::determinant(matrix);
  float det = glm::dot(n1, glm::cross(n2, n3));
  //printf("%f\n", det);
  assert(det > EPS || det < EPS);

  // MATH
  glm::vec3 result = (glm::dot(p1, n1) * glm::cross(n2, n3) + glm::dot(p2, n2) * glm::cross(n3, n1) + glm::dot(p3, n3) * glm::cross(n1, n2)) / det;
  return result;
  //return (glm::dot(p1, n1) * glm::cross(n2, n3) + glm::dot(p2, n2) * glm::cross(n3, n1) + glm::dot(p3, n3) * glm::cross(n1, n2));
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
  glLineWidth(1);
  glDrawArrays(GL_LINES, 0, this->posBuf.size());

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
  glLineWidth(5);
  glDrawArrays(GL_LINES, 0, this->posBufThick.size());

  // Disable and unbind
  GLSL::disableVertexAttribArray(this->handles.aPosition);
  GLSL::disableVertexAttribArray(this->handles.aColor);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glUseProgram(0);
}
