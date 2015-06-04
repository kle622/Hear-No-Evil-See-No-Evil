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
#include <memory>
#include <cmath>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "../Library/GLSL.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Handles.h"
#include "../WorldGrid/WorldGrid.h"
#include "../DebugDraw/DebugDraw.h"
#include "../Geometry/Geometry.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "../glm/gtc/type_ptr.hpp" //value_ptr

#define PI 3.1415926535897932384626f

class Camera {
  public:
    //Properties
    glm::vec3 lookat;
    glm::vec3 eye;
    glm::vec3 up;
    float fov;
    float aspect;
    float _near;
    float _far;
    float max_vert_angle = 85;
    DebugDraw *debug;

    //Constructor
    Camera(glm::vec3 lookat, glm::vec3 eye, glm::vec3 up, float fov, float aspect, float _near, float _far, DebugDraw *debug);

    // these vectors are all normalized
    virtual glm::vec3 getForward();
    virtual glm::vec3 getStrafe();
    virtual glm::vec3 getUp();

    // gets view/proj matrices
    virtual glm::mat4 getView();
    virtual glm::mat4 getProjection();

    virtual void moveVert(float step);
    virtual void moveHoriz(float step);

    // used for culling entire scene
    // returns list of objects that should be drawn
    std::vector<std::shared_ptr<GameObject>> getUnculled(WorldGrid *worldgrid);

  private:
    float theta = 0;
    float phi = 0;
};

#endif
