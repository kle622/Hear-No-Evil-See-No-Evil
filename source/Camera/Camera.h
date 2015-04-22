#ifndef CAMERA_H_
#define CAMERA_H_

/*
 * Basic camera class. Controlled by directly modifying eye, lookat, etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <time.h>
#include "../Library/GLSL.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr

class Camera {
    public:
        //Properties
        Handles *handles;
        glm::vec3 lookat;
        glm::vec3 eye;
        glm::vec3 up;
        float fov;
        float aspect;
        float _near;
        float _far;

        //Constructor
        Camera(Handles *handles, glm::vec3 lookat, glm::vec3 eye, glm::vec3 up, float fov, float aspect, float _near, float _far);

        //Object Methods
        void setView();
        void setProjection();
};

double clamp(double x, double min, double max);

#endif
