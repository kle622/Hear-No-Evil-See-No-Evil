#include <memory>
#include "Camera.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Handles.h"
#include "../WorldGrid/WorldGrid.h"

#define UPPER_BOUND_DEFAULT 85.0f
#define LOWER_BOUND_DEFAULT -10.0f

class Camera3DPerson: public Camera
{
  private:
    float theta;
    float phi;
    glm::vec3 getEye();
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
