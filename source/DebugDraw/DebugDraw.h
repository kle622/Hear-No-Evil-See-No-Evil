#ifndef __DEBUG_DRAW_H
#define __DEBUG_DRAW_H
#include "DebugHandles.h"
#include <string>

typedef struct LineSegment
{
  glm::vec3 start;
  glm::vec3 end;
  glm::vec3 color;
} LineSegment;

typedef struct
{
  glm::vec3 center;
  float radius;
  glm::vec3 normal;
  glm::vec3 color;
} Ring3D;

class DebugDraw
{
  public:
    DebugHandles handles;
    DebugDraw();
    glm::mat4 view;
    glm::mat4 projection;
    void addLine(LineSegment line);
    void addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void addRing(Ring3D ring);
    void addRing(glm::vec3 center, float radius, glm::vec3 normal, glm::vec3 color);
    void drawAll();
    //void addPolygon(int numVertices, ...);
  private:
    std::vector<LineSegment> lines;
    std::vector<Ring3D> rings;
    std::vector<float> posBuf;
    std::vector<float> colBuf;
    GLuint posBufObj;
    GLuint colBufObj;
};
#endif
