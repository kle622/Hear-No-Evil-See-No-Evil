#ifndef __DEBUG_DRAW_H
#define __DEBUG_DRAW_H
#include "DebugHandles.h"
#include "../Library/OBB.h"
#include <string>
#include <cassert>

#define SUBDIVISIONS 100.0f
#define EPS 0.000001

/*
 * Basic usage:
 * Add a line or ring to the instance of DebugDraw in main
 * All geometry added in this way will be drawn during the call to drawAll(), which is already done in main
 * Debug geometry is currenty only drawn in debug mode, which switches to a free-floating camera, but this can be changed
 */

class DebugDraw
{
  public:
    DebugHandles handles;
    glm::mat4 view;
    glm::mat4 projection;
    void addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color, bool thick);
    void addRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color, bool thick);
    void addBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color, bool thick);
    void addBox(glm::vec4 left,
                glm::vec4 right,
                glm::vec4 bottom,
                glm::vec4 top,
                glm::vec4 near,
                glm::vec4 far,
                glm::vec3 color,
                bool thick);
    void addBox(glm::vec3 topleftfront,
                glm::vec3 topleftback,
                glm::vec3 toprightfront,
                glm::vec3 toprightback,
                glm::vec3 botleftfront,
                glm::vec3 botleftback,
                glm::vec3 botrightfront,
                glm::vec3 botrightback,
                glm::vec3 color,
                bool thick);
    void addOBB(OBB obb, glm::vec3 color, bool thick);
    void drawAll();
    void clear();
  private:
    glm::vec3 intersectPlanes(glm::vec4 plane1, glm::vec4 plane2, glm::vec4 plane3);
    glm::vec3 getPlanePoint(glm::vec4 plane);
    glm::vec4 normalizePlane(glm::vec4 plane);
    std::vector<float> posBuf;
    std::vector<float> colBuf;
    GLuint posBufObj;
    GLuint colBufObj;
    std::vector<float> posBufThick;
    std::vector<float> colBufThick;
    GLuint posBufObjThick;
    GLuint colBufObjThick;
};
#endif
