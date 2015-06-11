#ifndef __LIGHT_PASS__HANDLES__
#define __LIGHT_PASS__HANDLES__
#include "../GameObject/Mesh.h"
#include "../GameObject/Shape.h"

class LightPassHandles {
 public:
  GLuint prog;
  GLuint aPosition;
  GLuint uModel;
  GLuint uView;
  GLuint uProj;
  GLuint uDepthMVP;
  GLuint uScreenSize;
  GLuint uPosMap;
  GLuint uColMap;
  GLuint uNormMap;
  GLuint uLightPos;
  GLuint uLightCol;
  GLuint uLightAtten;
  GLuint uLightDirection;
  GLuint uLightAngleCos;
  GLuint uAmbient;
  bool installShaders(const std::string &vShaderName, const std::string &fShaderName);
  void draw(Mesh *mesh);
  void draw(Shape* obj);
  void drawQuad();
  GLuint quad_VertexArrayID;
  GLuint quad_vertexbuffer;
};

static const GLfloat g_quad_vertex_buffer_data[] = {
  -2.0f, -2.0f, 0.0f,
  2.0f, -2.0f, 0.0f,
  -2.0f,  2.0f, 0.0f,
  -2.0f,  2.0f, 0.0f,
  2.0f, -2.0f, 0.0f,
  2.0f,  2.0f, 0.0f,
};

#endif
