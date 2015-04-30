#include <memory>
#include <limits>
#include <cassert>
#include "Camera.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Handles.h"
#include "../GameObject/Player.h"
#include "../GameObject/Wall.h"
#include "../WorldGrid/WorldGrid.h"
#include "../Library/OrientedBoundingBox.h"

#define UPPER_BOUND_DEFAULT 85.0f
#define LOWER_BOUND_DEFAULT -35.0f
#define EPSILON .00001f
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)

class Camera3DPerson: public Camera
{
  private:
    float theta;
    float phi;
    glm::vec3 getEye();
    glm::vec3 setZoom(glm::vec3 outDir);
    float castRayOnObjects(glm::vec3 rayStart, glm::vec3 rayEnd, std::vector<shared_ptr<GameObject>> objects);
    bool rayOBBIntersect(float *dist, glm::vec3 rayOrigin, glm::vec3 rayDirection, OrientedBoundingBox obb);
  public:
    float zoom;
    float lowerBound; // bounds stored as degrees
    float upperBound;
    GameObject *focus;
    WorldGrid *world;
    Camera3DPerson(Handles *handles, WorldGrid *world, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far);
    void moveVert(float step);
    void moveHoriz(float step);
    //Object Methods
    virtual void setView();
    virtual void setProjection();
};
