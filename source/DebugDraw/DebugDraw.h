#ifndef __DEBUG_DRAW_H
#define __DEBUG_DRAW_H
#include "DebugHandles.h"
#include <string>

#define SUBDIVISIONS 100.0f

/*
 * Basic usage:
 * Add a line or ring to the instance of DebugDraw in main
 * All geometry added in this way will be drawn during the call to drawAll(), which is already written
 * Debug geometry is currenty only drawn in debug mode, which switches to a free-floating camera, but this can be changed
 */

class DebugDraw
{
  public:
    DebugHandles handles;
    DebugDraw();
    glm::mat4 view;
    glm::mat4 projection;
    void addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void addRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color); // not supported
    void addBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color);  // not supported
    void addThickLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void addThickRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color); // not supported
    void addThickBox(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color);  // not supported
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
