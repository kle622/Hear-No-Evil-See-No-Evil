attribute vec3 aPosition;

uniform mat4 uMVP;
uniform vec4 uColor;

void main() {
  gl_Position = uMVP * vec4(aPosition, 1.0);
}
