#include "gbuffer.h"

GBuffer::GBuffer() {}

GBuffer::~GBuffer() {};

bool GBuffer::Init(unsigned int w_width, unsigned int w_height) {
  glGenFramebuffersEXT(1, &m_fbo);
  assert(m_fbo > 0);
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, m_fbo);

  glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
  glGenTextures(1, &m_depthTexture);
  assert(glGetError() == GL_NO_ERROR);

  for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
    glBindTexture(GL_TEXTURE_2D, m_textures[i]);
    assert(glGetError() == GL_NO_ERROR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w_width, w_height, 0, GL_RGB, GL_FLOAT, NULL);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    assert(glGetError() == GL_NO_ERROR);
    glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    assert(glGetError() == GL_NO_ERROR);
  }

  glBindTexture(GL_TEXTURE_2D, m_depthTexture);
  assert(glGetError() == GL_NO_ERROR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w_width, w_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  assert(glGetError() == GL_NO_ERROR);
  glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
  assert(glGetError() == GL_NO_ERROR);
  
  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(drawBuffers), drawBuffers);
  assert(glGetError() == GL_NO_ERROR);

  GLenum status = glCheckFramebufferStatusEXT(GL_DRAW_FRAMEBUFFER);
  assert(glGetError() == GL_NO_ERROR);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "ERROR!!! frame buffer not ok" << std::endl;
    return false;
  }
  
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
  assert(glGetError() == GL_NO_ERROR);

  return true;
}

void GBuffer::bindForWriting() {
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, m_fbo);
  assert(glGetError() == GL_NO_ERROR);
}

void GBuffer::bindForReading() {
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
  assert(glGetError() == GL_NO_ERROR);

  for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POS + i]);
    assert(glGetError() == GL_NO_ERROR);
  }
}

void GBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE texType) {
  glReadBuffer(GL_COLOR_ATTACHMENT0 + texType);
  assert(glGetError() == GL_NO_ERROR);
}
