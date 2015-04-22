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
    glm::vec3 *focus;
    Camera3DPerson(Handles *handles, glm::vec3 *focus, float zoom, float fov, float aspect, float _near, float _far);
    void moveHoriz(float step);
    void moveVert(float step);
    void checkCollide(vector<shared_ptr<GameObject>> objects);
}
