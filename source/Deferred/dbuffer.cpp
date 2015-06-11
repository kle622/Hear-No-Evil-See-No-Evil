#include "dbuffer.h"

DBuffer::DBuffer() {}

DBuffer::~DBuffer() {
  glDeleteFramebuffersEXT(1, &m_fbo);
  glDeleteRenderbuffersEXT(1, &m_depthBuffRT);
  glDeleteTextures(1, &m_depthTex);
}

bool DBuffer::Init(unsigned int w_width, unsigned int w_height) {
  width = w_width;
  height = w_height;

  glGenFramebuffersEXT(1, &m_fbo);
  assert(m_fbo > 0);
  glGenRenderbuffersEXT(1, &m_depthBuffRT);
  assert(m_depthBuffRT > 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
  assert(m_fbo > 0);

  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffRT);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBuffRT);

  glGenTextures(1, &m_depthTex);
  assert(m_depthTex > 0);
  glBindTexture(GL_TEXTURE_2D, m_depthTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTex, 0);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
    std::cerr << "DBUFFER FRAME BUFFER NOT OKAY!" << std::endl;
    return false;
  }

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  return true;
}

void DBuffer::start() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
  
  glClear(GL_DEPTH_BUFFER_BIT);
  
  glDrawBuffer(GL_NONE);
}

void DBuffer::stop() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLuint DBuffer::getDepthTex() {
  return m_depthTex;
}
