#include "ShadowMapPass2Handles.h"

bool Pass2Handles::installShaders(const std::string &vShaderName, const std::string &fShaderName) {
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

  uDepthMVP = GLSL::getUniformLocation(this->prog, "uDepthMVP");
  aPosition = GLSL::getAttribLocation(this->prog, "aPosition");
  aNormal = GLSL::getAttribLocation(this->prog, "aNormal");
  uProjMatrix = GLSL::getUniformLocation(this->prog, "uProjMatrix");
  uViewMatrix = GLSL::getUniformLocation(this->prog, "uViewMatrix");
  uModelMatrix = GLSL::getUniformLocation(this->prog, "uModelMatrix");
  //uLightPos = GLSL::getUniformLocation(this->prog, "uLightPos");
  uCamPos = GLSL::getUniformLocation(this->prog, "uCamPos");
  uMatAmb = GLSL::getUniformLocation(this->prog, "UaColor");
  uMatDif = GLSL::getUniformLocation(this->prog, "UdColor");
  uMatSpec = GLSL::getUniformLocation(this->prog, "UsColor");
  uMatShine = GLSL::getUniformLocation(this->prog, "Ushine");
  shadowMap = GLSL::getUniformLocation(this->prog, "shadowMap");
  texture = GLSL::getUniformLocation(this->prog, "texture");
  hasTex = GLSL::getUniformLocation(this->prog, "hasTex");
  aTexCoord = GLSL::getAttribLocation(this->prog, "texCoordIn");
  uNumLights = GLSL::getUniformLocation(this->prog, "numLights");
  uConeDirection = GLSL::getUniformLocation(this->prog, "coneDirection");
  uConeAngle = GLSL::getUniformLocation(this->prog, "coneAngle");
  detectionLevel = GLSL::getUniformLocation(this->prog, "detectionLevel");
  //uAttenuation = GLSL::getUniformLocation(this->prog, "attenuation");

  return true;
}


void Pass2Handles::draw(GameObject* obj) {
  GLSL::enableVertexAttribArray(this->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, obj->mesh->posBufObj);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  GLSL::enableVertexAttribArray(this->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, obj->mesh->norBufObj);
  glVertexAttribPointer(this->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

  if(obj->mesh->hasTexture) {
    GLSL::enableVertexAttribArray(this->aTexCoord);
    glBindBuffer(GL_ARRAY_BUFFER, obj->mesh->texBufObj);
    glVertexAttribPointer(this->aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
  }

  for (int s = 0; s < obj->mesh->shapes.size(); ++s) {
    int nIndices = (int)obj->mesh->shapes[s].mesh.indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->mesh->indBufObj);
    
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
  }
  /*
  GLSL::disableVertexAttribArray(this->aPosition);
  GLSL::disableVertexAttribArray(this->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}

void Pass2Handles::draw(Shape* obj) {
  GLSL::enableVertexAttribArray(this->aPosition);
  glBindBuffer(GL_ARRAY_BUFFER, obj->posBuffer);
  glVertexAttribPointer(this->aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

  GLSL::enableVertexAttribArray(this->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, obj->norBuffer);
  glVertexAttribPointer(this->aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
  
  if (obj->texBuffer > 0) {
  
    GLSL::enableVertexAttribArray(this->aTexCoord);
    glBindBuffer(GL_ARRAY_BUFFER, obj->texBuffer);
    glVertexAttribPointer(this->aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->idxBuffer);
  }

  //printf("about to draw in pass2handles\n");
  glDrawArrays(GL_TRIANGLES, 0, obj->indices);

  /*  GLSL::disableVertexAttribArray(this->aPosition);
  GLSL::disableVertexAttribArray(this->aNormal);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}
