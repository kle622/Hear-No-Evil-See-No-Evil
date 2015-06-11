#include "dbuffer.h"

DBuffer::DBuffer() {}

DBuffer::~DBuffer() {
  glDeleteFramebuffersEXT(1, &m_fb);
  glDeleteTextures(1, &m_depthTex);
  glDeleteRenderbuffersEXT(1, &m_depthBuffRT);
}

bool DBuffer::init(unsigned int w_width, unsigned int w_height) {
  width = w_width;
  height = w_height;

  glGenFramebuffersEXT(1, &m_fb);
  glGenRenderbuffersEXT(1, &m_depthBuffRT);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fb);

  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffRT);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 1024, 1024);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBuffRT);

  glGenTextures(1, &m_depthTex);
  glBindTexture(GL_TEXTURE_2D, m_depthTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_depthTex, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
    std::cerr << "DBUFFER FRAME BUFFER NOT OKAY! " <<  glCheckFramebufferStatus(GL_FRAMEBUFFER)  << std::endl;
    return false;
  }

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  return true;
}


void DBuffer::start() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fb);

  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);  

  glDrawBuffer(GL_NONE);  
  assert(glGetError() == GL_NO_ERROR);

}

void DBuffer::stop() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLuint DBuffer::getDepthTex() {
  return m_depthTex;
}
