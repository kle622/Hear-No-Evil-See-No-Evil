#ifndef __ALPHA_HANDLES
#define __ALPHA_HANDLES

#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../Library/GLSL.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Shape.h"
#include <string>

class AlphaHandles {
  public:
    GLuint prog;
    GLuint aPosition;
    GLuint aUV;
    GLuint uMVP;
    GLuint uTexture;
    bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
    void draw(Mesh* obj);
    void draw(Shape* obj);
};

#endif
