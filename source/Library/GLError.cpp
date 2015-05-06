#include "GLError.h"

void checkGLError()
{
  // error checking
  GLenum res = glGetError();
  switch (res) {
  case GL_NO_ERROR:
    break;
  case GL_INVALID_ENUM:
    fprintf(stderr, "invalid enum\n");
    assert(res == GL_NO_ERROR);
    break;
  case GL_INVALID_VALUE:
    fprintf(stderr, "invalid value\n");
    assert(res == GL_NO_ERROR);
    break;
  case GL_INVALID_OPERATION:
    fprintf(stderr, "invalid operation\n");
    assert(res == GL_NO_ERROR);
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    fprintf(stderr, "invalid framebuffer operation\n");
    assert(res == GL_NO_ERROR);
    break;
  case GL_OUT_OF_MEMORY:
    fprintf(stderr, "out of memory\n");
    assert(res == GL_NO_ERROR);
    break;
  case GL_STACK_UNDERFLOW:
    fprintf(stderr, "stack underflow\n");
    assert(res == GL_NO_ERROR);
    break;
  case GL_STACK_OVERFLOW:
    fprintf(stderr, "stack overflow\n");
    assert(res == GL_NO_ERROR);
    break;
  default:
    fprintf(stderr, "unknown error\n");
    assert(res == GL_NO_ERROR);
    break;
  }
}

void assertNormalized(glm::vec2 v)
{
  assert((glm::length(v) > 1.0f - EPSILON) && (glm::length(v) < 1.0f + EPSILON));
}

void assertNormalized(glm::vec3 v)
{
  assert((glm::length(v) > 1.0f - EPSILON) && (glm::length(v) < 1.0f + EPSILON));
}

void assertNormalized(glm::vec4 v)
{
  assert((glm::length(v) > 1.0f - EPSILON) && (glm::length(v) < 1.0f + EPSILON));
}
