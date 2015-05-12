#ifndef __PASS1__HANDLES__
#define __PASS1__HANDLES__

#include "GameObject.h"
#include "Shape.h"

class Pass1Handles : public Handles {
 public:
  GLuint uDepthMVP;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(GameObject* obj);
  void draw(Shape* obj);
};
#endif
