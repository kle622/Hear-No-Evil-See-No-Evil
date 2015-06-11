#ifndef __DBUFFER_H__
#define __DBUFFER_H__

#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include <iostream>
#include <exception>

class DBuffer {

 public:
  DBuffer();
  ~DBuffer();

  bool Init(unsigned int w_width, unsigned int w_height);
  void start();
  void stop();
  GLuint getDepthTex();

 private:
  unsigned int width, height;
  GLuint m_fbo;
  GLuint m_depthBuffRT;
  GLuint m_depthTex;
};

#endif
