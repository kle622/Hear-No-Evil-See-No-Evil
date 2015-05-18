#ifndef __GEOMETRY_H
#define __GEOMETRY_H

// grab bag of "useful" geometry functions
#include <vector>
#include <limits>
#include "../glm/glm.hpp"

class OBB
{
  public:
    OBB();
    // constructs OBB from AABB
    OBB(glm::vec3 center, glm::vec3 dimensions);
    glm::vec3 center;
    glm::vec3 axes[3];
    float halfLengths[3];
};

glm::vec4 normalizePlane(glm::vec4 plane);
glm::vec3 getPlanePoint(glm::vec4 plane);
glm::vec3 intersectPlanes(glm::vec4 plane1, glm::vec4 plane2, glm::vec4 plane3);
bool rayOBBIntersect(float *dist, glm::vec3 rayOrigin, glm::vec3 rayDirection, OBB obb);
bool obbOutsidePlane(OBB obb, glm::vec4 plane);
double clamp(double x, double min, double max);

#endif
