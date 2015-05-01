
class DebugDraw
{
  public:
    //GeometryHandles *handles;
    //DebugDraw(GeometryHandles *handles);
    void addLine(glm::vec3 start, glm::vec3 end);
    void addCircle(glm::vec3 center, glm::vec3 radius);
    void addPolygon(int numVertices, ...);
    void drawAll();
  private:
};
