attribute vec3 aPosition;
attribute vec3 aColor;
varying vec4 vColor;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

void main()
{
  gl_Position = uProjMatrix * uViewMatrix * vec4(aPosition, 1.0);
  vColor = vec4(aColor, 1.0);
}
