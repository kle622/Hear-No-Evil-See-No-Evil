#ifndef __PASS2__HANDLES__
#define __PASS2__HANDLES__

#include "GameObject.h"
#include "Shape.h"
#include "Player.h"

class Pass2Handles  : public Handles {
 public:
  GLuint shadowMap, uDepthMVP, hasTex, texture, aTexCoord;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(GameObject* obj);
  void draw(Shape* obj);
};


#endif
