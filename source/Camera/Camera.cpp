#include "Camera.h"

Camera::Camera(GLFWwindow* window, GLint viewHandle, GLint projectionHandle, 
    vec3 position, float fov, float aspect,
    float near, float far) {
    this->window = window;
    this->viewHandle = viewHandle;
    this->projectionHandle = projectionHandle;
    this->position = position;
    this->direction = vec3(0);
    this->rightV = vec3(0);
    this->fov = fov;
    this->aspect = aspect;
    this->near = near;
    this->far = far;
}

void Camera::setView(int g_width, int g_height) {
    double theta, phi;

    glfwGetCursorPos(window, &theta, &phi);
    float horizontal = .005 * float(g_width / 2 - theta);
    float vertical = .005 * float(g_height / 2 - phi);

    if (vertical > 1.4) {
        vertical = 1.4;
    }
    else if (vertical < -1.4) {
        vertical = -1.4;
    }

    float x = cos(vertical) * sin(horizontal);
    float y = sin(vertical);
    float z = cos(vertical) * cos(horizontal);
    direction = glm::vec3(x, y, z);
    rightV = glm::vec3(sin(horizontal - 3.14 / 2.0), 0, cos(horizontal - 3.14 / 2.0));

    glm::mat4 Cam = glm::lookAt(
        position, 
        position + direction,
        vec3(0, 1, 0));

    if (viewHandle >= 0)
        glUniformMatrix4fv(viewHandle, 1, GL_FALSE, glm::value_ptr(Cam));
}

void Camera::setProjection(int g_width, int g_height) {
    glm::mat4 Projection = glm::perspective(fov, (float)g_width / 
        g_height, near, far);
    if (projectionHandle >= 0)
        glUniformMatrix4fv(projectionHandle, 1, GL_FALSE, 
            glm::value_ptr(Projection));
}

double clamp(double x, double min, double max) {
    return x < min ? min : (x > max ? max : x);
}

