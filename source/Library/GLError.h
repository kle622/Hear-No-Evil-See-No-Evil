#ifndef __GL_ERROR_H
#define __GL_ERROR_H

#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include <stdio.h>
#include <cassert>
#define EPSILON 0.0001f

void checkGLError();
void assertNormalized(glm::vec2 v);
void assertNormalized(glm::vec3 v);
void assertNormalized(glm::vec4 v);

#endif
