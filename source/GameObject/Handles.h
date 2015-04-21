//
//  Handles.h
//

#ifndef __MarchingCubes__Handles__
#define __MarchingCubes__Handles__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
//#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLSL.h"

class Handles {
public:
   GLuint prog;
   GLuint aPosition, aNormal, uLightVec, uMatAmb, uMatDif, uMatSpec, uMatShine, uProjMatrix, uViewMatrix, uModelMatrix;
   bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
};

#endif /* defined(__MarchingCubes__Handles__) */
