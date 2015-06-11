#ifndef __GEOMETRY_PASS__HANDLES__
#define __GEOMETRY_PASS__HANDLES__
#include "../GameObject/GameObject.h"
#include "../GameObject/Shape.h"
#include "../GameObject/Wall.h"

class GeometryPassHandles {
 public:
  GLuint prog;
  GLuint aPosition;
  GLuint aNormal;
  GLuint aTexCoord;
  GLuint uModel;
  GLuint uView;
  GLuint uProj;
  GLuint uDepthMVP;
  GLuint texture;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(GameObject* obj);
  void draw(Shape* obj);
};


#endif
