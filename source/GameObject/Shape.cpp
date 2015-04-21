#include "Shape.h"

Shape::Shape(GLint modelHandle, vec3 position, float rotation, 
    vec3 scale, vec3 direction, float velocity, int indices, GLuint posBuffer, GLuint norBuffer,
    GLint posHandle, GLint norHandle, int material = 0) : GameObject(modelHandle,
    position, rotation, scale, direction, velocity, vec3(0, 0, 0), 0, indices, posBuffer, norBuffer,
    -1, posHandle, norHandle, material) {
}

void Shape::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glVertexAttribPointer(posHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, norBuffer);
    glVertexAttribPointer(norHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);

    SetModel(modelHandle, position, rotation, scale);
    glDrawArrays(GL_TRIANGLES, 0, indices);
}
