#ifndef __DBUFFER_H__
#define __DBUFFER_H__

#define MAX_LIGHTS 15
#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include <iostream>

class DBuffer {
 public:
  DBuffer();
  ~DBuffer();

  bool init(unsigned int w_width, unsigned int w_height, int numLights);
  void start(int l);
  void stop();
  GLuint getDepthTex(int l);

 private:
  GLuint m_fb[MAX_LIGHTS];
  GLuint m_depthBuffRT[MAX_LIGHTS];
  GLuint m_depthTex[MAX_LIGHTS];
  unsigned int width;
  unsigned int height;
  int lights;
};

#endif
