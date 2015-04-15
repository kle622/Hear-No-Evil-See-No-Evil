#ifndef CAMERA_H_
#define CAMERA_H_

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

using namespace glm;

class Camera {
    protected:
        //Properties
        GLFWwindow* window;
        GLint viewHandle;
        GLint projectionHandle;
        
    public:
        //Properties
        vec3 position;
        vec3 direction;
        vec3 rightV;
        float fov;
        float aspect;
        float near;
        float far;

        //Constructor
        Camera(GLFWwindow* window, GLint viewHandle, GLint projectionHandle, 
            vec3 position, float fov, float aspect,
            float near, float far);

        //Object Methods
        void setView(int g_width, int g_height);
        void setProjection(int g_width, int g_height);
};

double clamp(double x, double min, double max);

#endif