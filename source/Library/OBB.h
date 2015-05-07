#ifndef __ORIENTED_BOUNDING_BOX_H
#define __ORIENTED_BOUNDING_BOX_H

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

/*typedef struct
{
  glm::vec3 center;
  glm::vec3 axes[3];
  float halfLengths[3];
} OBB;*/
#endif
