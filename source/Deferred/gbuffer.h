#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include <iostream>
 
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

class GBuffer {
 public:
  enum GBUFFER_TEXTURE_TYPE {
    GBUFFER_TEXTURE_TYPE_POS,
    GBUFFER_TEXTURE_TYPE_DIFF,
    GBUFFER_TEXTURE_TYPE_NORM,
    GBUFFER_TEXTURE_TYPE_TEXCOORD,
    GBUFFER_NUM_TEXTURES
  };

  GBuffer();
  
  ~GBuffer();

  bool Init(unsigned int w_width, unsigned int w_height);
  void bindForWriting();
  void bindForReading();
  void setReadBuffer(GBUFFER_TEXTURE_TYPE texType);

 private:
  GLuint m_fbo;
  GLuint m_textures[GBUFFER_NUM_TEXTURES];
  GLuint m_depthTexture;
};

#endif
