typedef struct {
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<float> norBuf;
    std::vector<float> posBuf;
    std::vector<float> texBuf;
    GLuint posBufObj;
    GLuint norBufObj;
    GLuint indBufObj;
    GLuint texBufObj;
    glm::vec3 center;
    float radius;
} object_t;
