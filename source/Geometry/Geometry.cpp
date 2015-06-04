#include "Geometry.h"
#define EPSILON 0.0001f
#define DEBUG

OBB::OBB()
{
}

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

std::vector<glm::vec3>* OBB::getCorners()
{
  std::vector<glm::vec3> *corners = new std::vector<glm::vec3>();

  corners->push_back(center + axes[0] * halfLengths[0]  // x
                               + axes[1] * halfLengths[1]  // y
                               + axes[2] * halfLengths[2]); // z
  corners->push_back(center + axes[0] * halfLengths[0]  // x
                               + axes[1] * halfLengths[1]  // y
                               - axes[2] * halfLengths[2]); // z
  corners->push_back(center + axes[0] * halfLengths[0]  // x
                               - axes[1] * halfLengths[1]  // y
                               + axes[2] * halfLengths[2]); // z
  corners->push_back(center + axes[0] * halfLengths[0]  // x
                               - axes[1] * halfLengths[1]  // y
                               - axes[2] * halfLengths[2]); // z
  corners->push_back(center - axes[0] * halfLengths[0]  // x
                               + axes[1] * halfLengths[1]  // y
                               + axes[2] * halfLengths[2]); // z
  corners->push_back(center - axes[0] * halfLengths[0]  // x
                               + axes[1] * halfLengths[1]  // y
                               - axes[2] * halfLengths[2]); // z
  corners->push_back(center - axes[0] * halfLengths[0]  // x
                               - axes[1] * halfLengths[1]  // y
                               + axes[2] * halfLengths[2]); // z
  corners->push_back(center - axes[0] * halfLengths[0]  // x
                               - axes[1] * halfLengths[1]  // y
                               - axes[2] * halfLengths[2]); // z

  return corners;
}

glm::vec3 SplineCurve::getLocation(double dist)
{
  // assume dist >= 0, dist < maxDist
  // assume at least 4 points in spline curve
  int start = (int)dist;
  glm::vec3 p0;
  glm::vec3 p1;
  glm::vec3 p2;
  glm::vec3 p3;
  if (start > 0 && start < points.size() - 2) {
    p0 = points.at(start - 1);
    p1 = points.at(start);
    p2 = points.at(start + 1);
    p3 = points.at(start + 2);
  }
  else if (start == 0) {
    p0 = points.at(start);
    p1 = points.at(start);
    p2 = points.at(start + 1);
    p3 = points.at(start + 2);
  }
  else if (start == points.size() - 2) {
    p0 = points.at(start - 1);
    p1 = points.at(start);
    p2 = points.at(start + 1);
    p3 = points.at(start + 1);
  }
  else {
    // don't ever be here
    exit(-1);
  }
  dist = dist - (double)start;
  glm::vec4 poly(1.0f, dist, dist * dist, dist * dist * dist);
  glm::vec3 result(0.0f, 0.0f, 0.0f);
  result += glm::dot(f0, poly) * p0;
  result += glm::dot(f1, poly) * p1;
  result += glm::dot(f2, poly) * p2;
  result += glm::dot(f3, poly) * p3;
  return result;
}

double SplineCurve::getMaxDist()
{
  return maxDist;
}

void SplineCurve::addPoint(glm::vec3 point)
{
  points.push_back(point);
  maxDist += 1;
}

void SplineCurve::addPoint(float x, float y, float z)
{
  addPoint(glm::vec3(x, y, z));
}

bool pointOutsidePlane(glm::vec3 point, glm::vec4 plane)
{
  return point.x * plane.x + point.y * plane.y + point.z * plane.z + plane.w >= 0;
}

bool obbOutsidePlane(OBB obb, glm::vec4 plane)
{
  std::vector<glm::vec3> *corners = obb.getCorners();
  
  for (auto cornerItr = corners->begin(); cornerItr != corners->end(); ++cornerItr) {
    glm::vec3 corner = *cornerItr;
    if (pointOutsidePlane(*cornerItr, plane)) {
      return true;
    }
  }

  return false;
}

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
  //assert(det > EPSILON || det < -1.0f * EPSILON);

  // MATH
  glm::vec3 result = (glm::dot(p1, n1) * glm::cross(n2, n3) + glm::dot(p2, n2) * glm::cross(n3, n1) + glm::dot(p3, n3) * glm::cross(n1, n2)) / det;
  return result;
}

double clamp(double x, double min, double max)
{
  return x < min ? min : (x > max ? max : x);
}

std::vector<glm::vec4> *getViewFrustum(glm::mat4 VP)
{
  std::vector<glm::vec4> *planes = new std::vector<glm::vec4>();
  glm::vec4 left = glm::vec4(VP[0][3] + VP[0][0], VP[1][3] + VP[1][0], VP[2][3] + VP [2][0], VP[3][3] + VP[3][0]);
  glm::vec4 right = glm::vec4(VP[0][3] - VP[0][0], VP[1][3] - VP[1][0], VP[2][3] - VP [2][0], VP[3][3] - VP[3][0]);
  glm::vec4 bottom = glm::vec4(VP[0][3] + VP[0][1], VP[1][3] + VP[1][1], VP[2][3] + VP [2][1], VP[3][3] + VP[3][1]);
  glm::vec4 top = glm::vec4(VP[0][3] - VP[0][1], VP[1][3] - VP[1][1], VP[2][3] - VP [2][1], VP[3][3] - VP[3][1]);
  glm::vec4 nearPlane = glm::vec4(VP[0][3] + VP[0][2], VP[1][3] + VP[1][2], VP[2][3] + VP [2][2], VP[3][3] + VP[3][2]);
  glm::vec4 farPlane = glm::vec4(VP[0][3] - VP[0][2], VP[1][3] - VP[1][2], VP[2][3] - VP [2][2], VP[3][3] - VP[3][2]);

  planes->push_back(left); // left
  planes->push_back(right); // right
  planes->push_back(bottom); // bottom
  planes->push_back(top); // top
  planes->push_back(nearPlane); // near
  planes->push_back(farPlane); // far

  return planes;
}
