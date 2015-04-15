#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"

using namespace glm;

class Player: public GameObject {
    protected:

    public:
        //Constructor
        Player(GLint modelHandle, 
            vec3 position, float rotation, float scale, 
            vec3 direction, float velocity, vec3 dimensions, int indices,
            GLuint posBuffer, GLuint norBuffer, GLuint indBuffer,
            GLint posHandle, GLint norHandle, int material);

        //Object methods
        void move(float time);
        bool collide(GameObject* object);
};

#endif