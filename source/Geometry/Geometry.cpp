#include "Geometry.h"
#define EPSILON 0.0001f

OBB::OBB()
{
}

// constructs OBB from AABB
OBB::OBB(glm::vec3 center, glm::vec3 dimensions)
{
  this->center = center;
  this->axes[0] = glm::vec3(1.0f, 0.0f, 0.0f);
  this->axes[1] = glm::vec3(0.0f, 1.0f, 0.0f);
  this->axes[2] = glm::vec3(0.0f, 0.0f, 1.0f);
  this->halfLengths[0] = dimensions.x * 0.5;
  this->halfLengths[1] = dimensions.y * 0.5;
  this->halfLengths[2] = dimensions.z * 0.5;
}

bool obbOutsidePlane(OBB obb, glm::vec4 plane)
{
  bool result = false;
  std::vector<glm::vec3> corners;
  corners.push_back(obb.center + obb.axes[0] * obb.halfLengths[0]  // x
                               + obb.axes[1] * obb.halfLengths[1]  // y
                               + obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center + obb.axes[0] * obb.halfLengths[0]  // x
                               + obb.axes[1] * obb.halfLengths[1]  // y
                               - obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center + obb.axes[0] * obb.halfLengths[0]  // x
                               - obb.axes[1] * obb.halfLengths[1]  // y
                               + obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center + obb.axes[0] * obb.halfLengths[0]  // x
                               - obb.axes[1] * obb.halfLengths[1]  // y
                               - obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center - obb.axes[0] * obb.halfLengths[0]  // x
                               + obb.axes[1] * obb.halfLengths[1]  // y
                               + obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center - obb.axes[0] * obb.halfLengths[0]  // x
                               + obb.axes[1] * obb.halfLengths[1]  // y
                               - obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center - obb.axes[0] * obb.halfLengths[0]  // x
                               - obb.axes[1] * obb.halfLengths[1]  // y
                               + obb.axes[2] * obb.halfLengths[2]); // z
  corners.push_back(obb.center - obb.axes[0] * obb.halfLengths[0]  // x
                               - obb.axes[1] * obb.halfLengths[1]  // y
                               - obb.axes[2] * obb.halfLengths[2]); // z

  for (auto cornerItr = corners.begin(); cornerItr != corners.end(); ++cornerItr) {
    glm::vec3 corner = *cornerItr;
    if (corner.x * plane.x + corner.y * plane.y + corner.z * plane.z + plane.w >= 0) {
      result = true;
    }
  }

  return result;
}

/* uses Kay and Kajiya's slab method for ray/box intersection, found in text p.742
 *
 * assume value pointed to by dist is unusable if this method returns false
 *
 * dist is the distance from the ray origin to the intersection point
 */
bool rayOBBIntersect(float *dist, glm::vec3 rayOrigin, glm::vec3 rayDirection, OBB obb)
{
  *dist = 0;
  float tmin = std::numeric_limits<double>::min();
  float tmax = std::numeric_limits<double>::max();
  glm::vec3 p = obb.center - rayOrigin;
  for (int i = 0; i < 3; ++i) {
    float e = glm::dot(obb.axes[i], p);
    float f = glm::dot(obb.axes[i], rayDirection);
    if (f > EPSILON || f < -1.0f * EPSILON) {
      float t1 = (e + obb.halfLengths[i]) / f;
      float t2 = (e - obb.halfLengths[i]) / f;
      if (t1 > t2) {
        // swap t1 and t2
        float temp = t1;
        t1 = t2;
        t2 = temp;
      }
      tmin = fmax(tmin, t1);
      tmax = fmin(tmax, t2);
      if (tmin > tmax || tmax < 0) {
        return false;
      }
    }
    else if (-1.0f * e - obb.halfLengths[i] > 0 || obb.halfLengths[i] - e < 0)
    {
      return false;
    }
  }
  if (tmin > 0) {
    *dist = tmin;
  }
  else {
    *dist = tmax;
  }
  return true;
}

glm::vec4 normalizePlane(glm::vec4 plane)
{
  glm::vec3 n = glm::vec3(plane);
  float l = glm::length(n);
  float d = plane.w / l;
  n = glm::normalize(n);

  return glm::vec4(n, d);
}

glm::vec3 getPlanePoint(glm::vec4 plane)
{
  return -1.0f * plane.w * glm::vec3(plane.x, plane.y, plane.z);
}

// does not assume normalized planes
// assumes planes are not parallel
// book p. 783, equation 16.59
glm::vec3 intersectPlanes(glm::vec4 plane1, glm::vec4 plane2, glm::vec4 plane3)
{
  // normalize planes
  glm::vec4 pl1 = normalizePlane(plane1);
  glm::vec4 pl2 = normalizePlane(plane2);
  glm::vec4 pl3 = normalizePlane(plane3);

  // get point on each plane
  glm::vec3 p1 = getPlanePoint(pl1);
  glm::vec3 p2 = getPlanePoint(pl2);
  glm::vec3 p3 = getPlanePoint(pl3);

  // get plane normals
  glm::vec3 n1 = glm::vec3(pl1);
  glm::vec3 n2 = glm::vec3(pl2);
  glm::vec3 n3 = glm::vec3(pl3);

  float det = glm::dot(n1, glm::cross(n2, n3));
  assert(det > EPSILON || det < -1.0f * EPSILON);

  // MATH
  glm::vec3 result = (glm::dot(p1, n1) * glm::cross(n2, n3) + glm::dot(p2, n2) * glm::cross(n3, n1) + glm::dot(p3, n3) * glm::cross(n1, n2)) / det;
  return result;
}

double clamp(double x, double min, double max)
{
  return x < min ? min : (x > max ? max : x);
}
