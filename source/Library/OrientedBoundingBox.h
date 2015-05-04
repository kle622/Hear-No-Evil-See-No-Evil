#ifndef __ORIENTED_BOUNDING_BOX_H
#define __ORIENTED_BOUNDING_BOX_H
typedef struct
{
  glm::vec3 center;
  glm::vec3 axes[3];
  float halfLengths[3];
} OrientedBoundingBox;
#endif
