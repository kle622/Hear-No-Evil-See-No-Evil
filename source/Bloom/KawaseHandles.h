#ifndef __KAWASE_HANDLES
#define __KAWASE_HANDLES

#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../Library/GLSL.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Shape.h"
#include <string>

class KawaseHandles {
  public:
    GLuint prog;
    GLuint aPosition;
    GLuint uMVP;
    GLuint uBloomMap;
    GLuint uKernelSize;
    GLuint uWindowSize;
    bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
    void draw(GameObject* obj);
    void draw(Shape* obj);
};

#endif
