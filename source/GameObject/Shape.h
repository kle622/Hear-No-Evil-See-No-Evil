#ifndef SHAPE_H_
#define SHAPE_H_

#include "GameObject.h"

using namespace glm;

class Shape: public GameObject {
    protected:
        
    public:
        //Constructor
        Shape(GLint modelHandle, vec3 position, float rotation, 
		    vec3 scale, vec3 direction, float velocity, int indices, 
		    GLuint posBuffer, GLuint norBuffer,
		    GLint posHandle, GLint norHandle, int material);

        //Object methods
        void draw();
};

#endif