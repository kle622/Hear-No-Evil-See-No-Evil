
#ifndef __DEBUG_DRAW_H
#define __DEBUG_DRAW_H
#include "../GameObject/Handles.h"

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
    Handles *handles;
    DebugDraw(Handles *handles);
    void addLine(LineSegment line);
    void addRing(Ring3D ring);
    void addPolygon(int numVertices, ...);
    void drawAll();
  private:
    std::vector<LineSegment> lines;
    std::vector<Ring3D> rings;
};
#endif
