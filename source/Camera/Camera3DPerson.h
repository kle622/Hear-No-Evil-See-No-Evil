#include "Camera.h"
#include "../GameObject/GameObject.h"

class Camera3DPerson: public Camera
{
  private:
    float theta;
    float phi;
    glm::vec3 getEye();
  public:
    float zoom;
    GameObject *focus;
    Camera3DPerson(Handles *handles, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far);
    void moveVert(float step);
    void checkCollide(vector<shared_ptr<GameObject>> objects);
}
