#ifndef __MESH_H
#define __MESH_H

#include <iostream>
#include "Handles.h"
#include "../Library/tiny_obj_loader.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr



class Mesh {
  public:
    // should be called once to initialize object
    // pass a string that names the .obj file to be loaded,
    // e.g. "cube.obj"
  void loadShapes(const std::string &objFile);
    void drawObject(Handles *handles);
    void loadTexture(const std::string &filename);
    float radius;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<float> norBuf;
    std::vector<float> posBuf;
    std::vector<float> texBuf;
    GLuint posBufObj;
    GLuint norBufObj;
    GLuint indBufObj;
    GLuint texBufObj;
    int material;
    char* bmp;
    GLfloat* uvs;
  private:
    void computeBound(void);
    void resize_obj(void);
    void sendNormals(void);
    glm::vec3 center;
};

#endif
