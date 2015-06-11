#ifndef __LIGHT_PASS__HANDLES__
#define __LIGHT_PASS__HANDLES__
#include "../GameObject/Mesh.h"
#include "../GameObject/Shape.h"

class LightPassHandles {
 public:
  GLuint prog;
  GLuint aPosition;
  GLuint uModel;
  GLuint uView;
  GLuint uProj;
  GLuint uScreenSize;
  GLuint uPosMap;
  GLuint uColMap;
  GLuint uNormMap;
  GLuint uCamPos;
  GLuint uLightPos;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(Mesh *mesh);
  void draw(Shape* obj);
};


#endif
