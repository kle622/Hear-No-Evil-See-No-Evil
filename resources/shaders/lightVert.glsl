attribute vec3 aPosition;
attribute vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

varying vec2 vUV;

void main() {
  gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
  //gl_Position = vec4(aPosition, 1.0);
  gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);
  vUV = aUV;
}
