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
    std::vector<glm::vec3>* getCorners();
    glm::vec3 center;
    glm::vec3 axes[3];
    float halfLengths[3];
};

class SplineCurve
{
  public:
    glm::vec3 getLocation(double dist);
    double getMaxDist();
    void addPoint(glm::vec3 point);
    void addPoint(float x, float y, float z);
  private:
    double maxDist = -1;
    glm::vec4 f0 = glm::vec4(0.0f, -0.5f, 1.0f, -0.5f);
    glm::vec4 f1 = glm::vec4(1.0f, 0.0f, -2.5f, 1.5f);
    glm::vec4 f2 = glm::vec4(0.0f, 0.5f, 2.0f, -1.5f);
    glm::vec4 f3 = glm::vec4(0.0f, 0.0f, -0.5f, 0.5f);
    std::vector<glm::vec3> points;
};

glm::vec4 normalizePlane(glm::vec4 plane);
glm::vec3 getPlanePoint(glm::vec4 plane);

// does not assume normalized planes
// assumes planes are not parallel
// book p. 783, equation 16.59
glm::vec3 intersectPlanes(glm::vec4 plane1, glm::vec4 plane2, glm::vec4 plane3);

// the plane normals are not normalized, and point to the inside of the view frustum
// plane equations obtained from http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
// a plane is represented by a vec4 as <a, b, c, d>
std::vector<glm::vec4> *getViewFrustum(glm::mat4 VP);

/* uses Kay and Kajiya's slab method for ray/box intersection, found in text p.742
 *
 * assume value pointed to by dist is unusable if this method returns false
 *
 * assume rayDirection is normalized
 *
 * dist is the distance from the ray origin to the intersection point
 */
bool rayOBBIntersect(float *dist, glm::vec3 rayOrigin, glm::vec3 rayDirection, OBB obb);

// "outside" is defined as in the positive half-space of the plane, as defined by the plane normal
bool pointOutsidePlane(glm::vec3 point, glm::vec4 plane);

bool obbOutsidePlane(OBB obb, glm::vec4 plane);
double clamp(double x, double min, double max);

#endif
