#ifndef TEXTURES_H_
#define TEXTURES_H_

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include "../GameObject/Mesh.h"
#include "../GameObject/Handles.h"
#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr

using namespace glm;
using namespace std;

/*data structure for the image used for  texture mapping */
typedef struct Image {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
} Image;

typedef struct RGB {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} RGB;

class Textures {
protected:
  Mesh *mesh;
  Handles *handles;

public:
  //Constructor
  Textures(Mesh* mesh, Handles* handles);

  GLvoid LoadTexture(char* image_file, int texID);
  void drawTexture(int texId);
  //unsigned int getint(FILE *fp);
  //unsigned int getshort(FILE *fp);
  int ImageLoad(char *filename, Image *image);
};
#endif
