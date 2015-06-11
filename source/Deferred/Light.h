#ifndef __LIGHT_H_
#define __LIGHT_H_

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, txrans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr

typedef struct
{
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 atten;
  float angle;
  glm::vec3 direction;
  bool guardLight;
} Light;

#endif
