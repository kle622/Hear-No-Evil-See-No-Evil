//
//  DebugHandles.h
//

#ifndef __MarchingCubes__Debug__Handles__
#define __MarchingCubes__Debug__Handles__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../Library/GLSL.h"

class DebugHandles {
public:
   GLuint prog;
   GLuint aPosition, aColor, uProjMatrix, uViewMatrix, uModelMatrix;
   bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
};

#endif /* defined(__MarchingCubes__Debug__Handles__) */
