#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include <iostream>
#include <exception>
 
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

class GBuffer {
 public:
  GBuffer();  
  ~GBuffer();

  bool Init(unsigned int w_width, unsigned int w_height);
  void start();
  void stop();
  GLuint getDiffTexture();
  GLuint getPosTexture();
  GLuint getNormTexture();

 private:
  GLuint m_fbo;
  GLuint m_diffRT;
  unsigned int m_diffTex;
  GLuint m_posRT;
  unsigned int m_posTex;
  GLuint m_normRT;
  unsigned int m_normTex;
  GLuint m_depthBuff;
  unsigned int m_depthTex;
  GLuint m_depthRT;
  
  unsigned int width;
  unsigned int height;
};

#endif
