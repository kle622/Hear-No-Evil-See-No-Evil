#include "KawaseHandles.h"

bool KawaseHandles::installShaders(const std::string &vShaderName, const std::string &fShaderName) {
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
  uMVP = GLSL::getUniformLocation(this->prog, "uMVP");
  uBloomMap = GLSL::getUniformLocation(this->prog, "uBloomMap");
  uKernelSize = GLSL::getUniformLocation(this->prog, "uKernelSize");
  uWindowSize = GLSL::getUniformLocation(this->prog, "uWindowSize");

  assert(glGetError() == GL_NO_ERROR);
  return true;
}


void KawaseHandles::draw(GameObject* obj) {
  GLSL::enableVertexAttribArray(this->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, obj->mesh->posBufObj);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  for (int s = 0; s < obj->mesh->shapes.size(); ++s) {
    int nIndices = (int)obj->mesh->shapes[s].mesh.indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->mesh->indBufObj);
    
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
  }
}

void KawaseHandles::draw(Shape* obj) {
  GLSL::enableVertexAttribArray(this->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, obj->posBuffer);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, obj->indices);
}
