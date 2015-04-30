#include <memory>
#include <limits>
#include "Camera.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Handles.h"
#include "../GameObject/Player.h"
#include "../GameObject/Wall.h"
#include "../WorldGrid/WorldGrid.h"

#define UPPER_BOUND_DEFAULT 85.0f
#define LOWER_BOUND_DEFAULT -10.0f
#define EPSILON .000000000000001
#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)

typedef struct
{
  glm::vec3 center;
  glm::vec3 axes[3];
  float halfLengths[3];
} OrientedBoundingBox;

class Camera3DPerson: public Camera
{
  private:
    float theta;
    float phi;
    glm::vec3 getEye();
    glm::vec3 adjustZoom(glm::vec3 outDir);
    float castRay(glm::vec3 rayStart, glm::vec3 rayEnd);
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
