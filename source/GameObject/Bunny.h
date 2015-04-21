#ifndef BUNNY_H_
#define BUNNY_H_

#include "GameObject.h"

using namespace glm;

class Bunny: public GameObject {
    protected:
        
    public:
        //Properties

        //Constructor
        Bunny(GLint modelHandle, 
            vec3 position, float rotation, vec3 scale, 
            vec3 direction, float velocity, vec3 dimensions, int indices,
            GLuint posBuffer, GLuint norBuffer, GLuint indBuffer,
            GLint posHandle, GLint norHandle, int material);

        //Object methods
        void move(float time);
        bool collide(GameObject* object);
};

#endif