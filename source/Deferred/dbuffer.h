#ifndef __DBUFFER_H__
#define __DBUFFER_H__

#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include <iostream>

class DBuffer {
 public:
  DBuffer();
  ~DBuffer();

  bool init(unsigned int w_width, unsigned int w_height);
  void start();
  void stop();
  GLuint getDepthTex();

 private:
  GLuint m_fb;
  GLuint m_depthBuffRT;
  GLuint m_depthTex;
  unsigned int width;
  unsigned int height;
};

#endif
