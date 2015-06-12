#include "dbuffer.h"

DBuffer::DBuffer() {}

DBuffer::~DBuffer() {
  for (int i = 0; i < lights; i++) {
    glDeleteFramebuffersEXT(1, &m_fb[i]);
    glDeleteTextures(1, &m_depthTex[i]);
    glDeleteRenderbuffersEXT(1, &m_depthBuffRT[i]);
  }
}

bool DBuffer::init(unsigned int w_width, unsigned int w_height, int numLights) {
  width = w_width;
  height = w_height;
  lights = numLights;

  for (int i = 0;  i < numLights; i++) {
    glGenFramebuffersEXT(1, &m_fb[i]);
    glGenRenderbuffersEXT(1, &m_depthBuffRT[i]);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fb[i]);
    
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffRT[i]);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_depthBuffRT[i]);
    
    glGenTextures(1, &m_depthTex[i]);
    glBindTexture(GL_TEXTURE_2D, m_depthTex[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_depthTex[i], 0);
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
      std::cerr << "DBUFFER FRAME BUFFER NOT OKAY! " <<  glCheckFramebufferStatus(GL_FRAMEBUFFER)  << std::endl;
      return false;
    }
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  }
  return true;
}


void DBuffer::start(int l) {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fb[l]);

  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);  

  glDrawBuffer(GL_NONE);  
  assert(glGetError() == GL_NO_ERROR);

}

void DBuffer::stop() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

GLuint DBuffer::getDepthTex(int l) {
  return m_depthTex[l];
}
