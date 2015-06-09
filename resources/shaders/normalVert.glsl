attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

// normals are in model space
varying vec4 vNormal;

void main() {
  gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
  vNormal = uModel * vec4(aNormal, 0.0);
}
