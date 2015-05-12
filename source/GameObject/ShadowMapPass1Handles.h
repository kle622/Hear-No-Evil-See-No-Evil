#ifndef __PASS1__HANDLES__
#define __PASS1__HANDLES__

#include "GameObject.h"

class Pass1Handles : public Handles {
 public:
  GLuint uDepthMVP;
  bool installShaders();
  void draw(GameObject* obj);
};
#endif
