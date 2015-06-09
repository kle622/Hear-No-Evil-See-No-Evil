#ifndef __LIGHT_PASS__HANDLES__
#define __LIGHT_PASS__HANDLES__
#include "GameObject.h"
#include "Shape.h"
#include "Player.h"

class LightPassHandles : public Handles {
 public:
  GLuint aPosition;
  GLuint uModel;
  GLuint uView;
  GLuint uProj;
  GLuint uScreenSize;
  GLuint uPosMap;
  GLuint uColMap;
  GLuint uNormMap;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(GameObject* obj);
  void draw(Shape* obj);
};


#endif
