#ifndef __SIMPLE_HANDLES
#define __SIMPLE_HANDLES

#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../Library/GLSL.h"
#include "../GameObject/Mesh.h"
#include "../GameObject/Shape.h"
#include <string>

class SimpleHandles {
  public:
    GLuint prog;
    GLuint aPosition;
    GLuint uMVP;
    GLuint uColor;
    bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
    void draw(Mesh* obj);
    void draw(Shape* obj);
};

#endif
