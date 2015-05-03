attribute vec3 aPosition;
attribute vec3 aColor;
varying vec4 vColor;

void main()
{
  gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
  vColor = vec4(aColor, 1.0);
}
