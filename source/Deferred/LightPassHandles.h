#ifndef __LIGHT_PASS__HANDLES__
#define __LIGHT_PASS__HANDLES__
#include "../GameObject/Mesh.h"
#include "../GameObject/Shape.h"

class LightPassHandles : public Handles {
 public:
  GLuint aPosition;
  GLuint aUV;
  GLuint uModel;
  GLuint uView;
  GLuint uProj;
  GLuint uScreenSize;
  GLuint uPosMap;
  GLuint uColMap;
  GLuint uNormMap;
  GLuint uCamPos;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(Mesh *mesh);
  void draw(Shape* obj);
};


#endif
