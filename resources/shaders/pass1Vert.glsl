attribute vec3 aPosition;

uniform mat4 uDepthMVP;

varying vec4 ShadowCoord;

void main() {
     gl_Position = uDepthMVP * vec4(aPosition, 1.0);
     ShadowCoord = uDepthMVP * vec4(aPosition, 1.0);
}