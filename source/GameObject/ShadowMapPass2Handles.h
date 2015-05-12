#ifndef __PASS2__HANDLES__
#define __PASS2__HANDLES__

#include "GameObject.h"
#include "Shape.h"

class Pass2Handles  : public Handles {
 public:
  GLuint shadowMap, uDepthMVP;
  bool installShaders();
  void draw(GameObject* obj);
  void draw(Shape* obj);
};


#endif
