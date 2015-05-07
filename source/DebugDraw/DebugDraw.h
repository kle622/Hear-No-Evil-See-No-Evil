#ifndef __DEBUG_DRAW_H
#define __DEBUG_DRAW_H
#include "DebugHandles.h"
#include "../Library/OBB.h"
#include <string>

#define SUBDIVISIONS 100.0f
#define EPS 0.0000001

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
    void addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void addRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color);
    void addBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color);
    void addOBB(OBB obb, glm::vec3 color);
    void addThickLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void addThickRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color);
    void addThickBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color);
    void addThickOBB(OBB obb, glm::vec3 color);
    void drawAll();
    void clear();
    //void addPolygon(int numVertices, ...);  // not supported
  private:
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
