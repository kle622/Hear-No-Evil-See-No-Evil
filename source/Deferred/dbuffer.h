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

  void Init(int w_width, int w_height);
  void start();
  void stop();

 private:
  int width, height;
  GLuint m_fbo;
  GLuint m_depthBuffRT;
  unsigned int m_depthTex;
};

#endif
