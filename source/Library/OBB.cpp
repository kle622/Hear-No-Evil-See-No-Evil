#include "OBB.h"


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
