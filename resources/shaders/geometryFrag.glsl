// normals are in model space
varying vec3 vNormal;
varying vec3 vWorldPos;
varying vec2 vTexCoord;

uniform sampler2D texture;

void main() {
  gl_FragData[0] = vec3(texture2D(texture, vTexCoord)); // diffuse
  gl_FragData[1] = vNormal; // normal
  gl_FragData[2] = vWorldPos; // world pos
}
