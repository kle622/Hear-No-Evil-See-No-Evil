attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoordIn;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

// normals are in model space
varying vec3 vNormal;
varying vec3 vWorldPos;
varying vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoordIn;
  vNormal = vec3(uModel * vec4(aNormal, 0.0));
  vWorldPos = vec3(uModel * vec4(aPosition, 1.0));
  gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
}
