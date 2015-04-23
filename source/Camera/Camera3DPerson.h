#include <memory>
#include "Camera.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/Handles.h"

class Camera3DPerson: public Camera
{
  private:
    float theta;
    float phi;
    float boundHigh;
    float boundLow;
    glm::vec3 getEye();
  public:
    float zoom;
    GameObject *focus;
    Camera3DPerson(Handles *handles, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far);
    void moveVert(float step);
    void moveHoriz(float step);
    void checkCollide(vector<shared_ptr<GameObject>> objects);
    void setUpperBound(float degrees);
    void setLowerBound(float degrees);
    //Object Methods
    virtual void setView();
    virtual void setProjection();
};
