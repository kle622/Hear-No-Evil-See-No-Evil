#include "LightPassHandles.h"

bool LightPassHandles::installShaders(const std::string &vShaderName, const std::string &fShaderName) {
  GLint rc;

  // Create shader handles
  GLuint VS = glCreateShader(GL_VERTEX_SHADER);
  GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

  // Read shader sources
  const char *vshader = GLSL::textFileRead(vShaderName.c_str());
  const char *fshader = GLSL::textFileRead(fShaderName.c_str());
  glShaderSource(VS, 1, &vshader, NULL);
  glShaderSource(FS, 1, &fshader, NULL);

  // Compile vertex shader
  glCompileShader(VS);
  GLSL::printError();
  glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
  GLSL::printShaderInfoLog(VS);
  if(!rc) {
    printf("Error compiling vertex shader %s\n", vShaderName.c_str());
    return false;
  }

  // Compile fragment shader
  glCompileShader(FS);
  GLSL::printError();
  glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
  GLSL::printShaderInfoLog(FS);
  if(!rc) {
    printf("Error compiling fragment shader %s\n", fShaderName.c_str());
    return false;
  }

  // Create the program and link
  this->prog = glCreateProgram();
  assert(this->prog > 0);
  glAttachShader(this->prog, VS);
  glAttachShader(this->prog, FS);
  glLinkProgram(this->prog);

  GLSL::printError();
  glGetProgramiv(this->prog, GL_LINK_STATUS, &rc);
  GLSL::printProgramInfoLog(this->prog);
  if(!rc) {
    printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
    return false;
  }

  aPosition = GLSL::getAttribLocation(this->prog, "aPosition");
  uProj = GLSL::getUniformLocation(this->prog, "uProj");
  uView = GLSL::getUniformLocation(this->prog, "uView");
  uModel = GLSL::getUniformLocation(this->prog, "uModel");
  //uDepthMVP = GLSL::getUniformLocation(this->prog, "uDepthMVP");
  uScreenSize = GLSL::getUniformLocation(this->prog, "uScreenSize");
  uNormMap = GLSL::getUniformLocation(this->prog, "uNormMap");
  uPosMap = GLSL::getUniformLocation(this->prog, "uPosMap");
  uColMap = GLSL::getUniformLocation(this->prog, "uColMap");
  uLightPos = GLSL::getUniformLocation(this->prog, "uLightPos");
  uLightCol = GLSL::getUniformLocation(this->prog, "uLightCol");
  uLightAtten = GLSL::getUniformLocation(this->prog, "uLightAtten");
  uLightDirection = GLSL::getUniformLocation(this->prog, "uLightDirection");
  uLightAngleCos = GLSL::getUniformLocation(this->prog, "uLightAngleCos");
  uDetectionLevel = GLSL::getUniformLocation(this->prog, "uDetectionLevel");
  uMatRoughness = GLSL::getUniformLocation(this->prog, "uMatRoughness");
  uFresReflectance = GLSL::getUniformLocation(this->prog, "uFresReflectance");
  uAmbient = GLSL::getUniformLocation(this->prog, "uAmbient");

  // stuff to init quad buffers (i didn't know where to put this)
  /*glGenVertexArrays(1, &quad_VertexArrayID);
  assert(quad_VertexArrayID > 0);
  glBindVertexArray(quad_VertexArrayID);*/

  glGenBuffers(1, &quad_vertexbuffer);
  assert(quad_vertexbuffer > 0);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data) * sizeof(GLfloat), &g_quad_vertex_buffer_data[0], GL_STATIC_DRAW);

  return true;
}

void LightPassHandles::draw(Mesh* mesh) {
  GLSL::enableVertexAttribArray(this->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->posBufObj);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  for (int s = 0; s < mesh->shapes.size(); ++s) {
    int nIndices = (int)mesh->shapes[s].mesh.indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indBufObj);
    
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
  }
  /*
  GLSL::disableVertexAttribArray(this->aPosition);
  GLSL::disableVertexAttribArray(this->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}

void LightPassHandles::draw(Shape* obj) {
  GLSL::enableVertexAttribArray(this->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, obj->posBuffer);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, obj->indices);

  /*  GLSL::disableVertexAttribArray(this->aPosition);
  GLSL::disableVertexAttribArray(this->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}

void LightPassHandles::drawQuad()
{
  GLSL::enableVertexAttribArray(aPosition);
  //glBindVertexArray(quad_VertexArrayID);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, sizeof(g_quad_vertex_buffer_data) * 3 * 6);
}
