#ifndef PLAYER_H_
#define PLAYER_H_

#include "GameObject.h"

using namespace glm;

class Player: public GameObject {
    protected:

    public:
        //members
        int score = 0;

        //Constructor
        Player(GLint modelHandle, 
            vec3 position, float rotation, float scale, 
            vec3 direction, float velocity, vec3 dimensions, 
            float scanRadius, int indices,
            GLuint posBuffer, GLuint norBuffer, GLuint indBuffer,
            GLint posHandle, GLint norHandle, int material);

        //Object methods
        bool collide(GameObject* object);
};

#endif