#ifndef __GEOMETRY_PASS__HANDLES__
#define __GEOMETRY_PASS__HANDLES__
#include "GameObject.h"
#include "Shape.h"
#include "Player.h"

class GeometryPassHandles : public Handles {
 public:
  GLuint aPosition;
  GLuint aNormal;
  GLuint aTexCoord;
  GLuint uModel;
  GLuint uView;
  GLuint uProj;
  GLuint texture;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(GameObject* obj);
  void draw(Shape* obj);
};


#endif
