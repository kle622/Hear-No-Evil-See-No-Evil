#ifndef __DEBUG_DRAW_H
#define __DEBUG_DRAW_H
#include "DebugHandles.h"
#include <string>

typedef struct LineSegment
{
  glm::vec3 start;
  glm::vec3 end;
} LineSegment;

typedef struct
{
  glm::vec3 center;
  float radius;
  glm::vec3 normal;
} Ring3D;

class DebugDraw
{
  public:
    DebugHandles handles;
    DebugDraw();
    DebugDraw(const std::string &vShaderName, const std::string &fShaderName);
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    void addLine(LineSegment line);
    void addLine(glm::vec3 start, glm::vec3 end);
    void addRing(Ring3D ring);
    void addRing(glm::vec3 center, float radius, glm::vec3 normal);
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
