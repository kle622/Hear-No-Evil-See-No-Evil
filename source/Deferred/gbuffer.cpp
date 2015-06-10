#include "gbuffer.h"

GBuffer::GBuffer() {}

GBuffer::~GBuffer() {
  glDeleteTextures(1, &m_diffTex);
  glDeleteTextures(1, &m_posTex);
  glDeleteTextures(1, &m_normTex);
  glDeleteFramebuffersEXT(1, &m_diffRT);
  glDeleteRenderbuffersEXT(1, &m_posRT);
  glDeleteRenderbuffersEXT(1, &m_normRT);
  glDeleteRenderbuffersEXT(1, &m_depthBuff);
};

bool GBuffer::Init(unsigned int w_width, unsigned int w_height) {
  width = w_width;
  height = w_height;

  glGenFramebuffersEXT(1, &m_fbo);
  assert(m_fbo > 0);
  glGenRenderbuffersEXT(1, &m_diffRT);
  glGenRenderbuffersEXT(1, &m_posRT);
  glGenRenderbuffersEXT(1, &m_normRT);
  glGenRenderbuffersEXT(1, &m_depthBuff);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

  //bind diffuse render target
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_diffRT);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_diffRT);
  
  //bind position render target
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_posRT);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA32F_ARB, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_RENDERBUFFER_EXT, m_posRT);  

  //bind normal render target
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_normRT);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA16F_ARB, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_RENDERBUFFER_EXT, m_normRT);

  //bind depth buffer
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuff);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBuff);    

  //bind diffuse texture
  glGenTextures(1, &m_diffTex);
  glBindTexture(GL_TEXTURE_2D, m_diffTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_diffTex, 0);

  
  //bind pos texture
  glGenTextures(1, &m_posTex);
  glBindTexture(GL_TEXTURE_2D, m_posTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, m_posTex, 0);


  //bind diffuse texture
  glGenTextures(1, &m_normTex);
  glBindTexture(GL_TEXTURE_2D, m_normTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, m_normTex, 0);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
    //throw std::runtime_error("FRAME BUFFER NOT OKAY!");
  }

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  return true;
}

void GBuffer::start() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
  
  glActiveTexture(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE1_ARB);
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE2_ARB);
  glEnable(GL_TEXTURE_2D);
  
  GLenum drawBuffs[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
  glDrawBuffers(3, drawBuffs);
}

void GBuffer::stop() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLuint GBuffer::getDiffTexture() {
  return m_diffTex;
}

GLuint GBuffer::getPosTexture() {
  return m_posTex;
}

GLuint GBuffer::getNormTexture() {
  return m_normTex;
}
