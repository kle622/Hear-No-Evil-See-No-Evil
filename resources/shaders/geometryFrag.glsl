// normals are in model space
varying vec3 vNormal;
varying vec3 vWorldPos;
varying vec2 vTexCoord;
varying vec4 vShadowCoord;

uniform sampler2D texture;

void main() {
  gl_FragData[0] = texture2D(texture, vTexCoord); // diffuse
  gl_FragData[1] = vec4(vWorldPos, 1.0); // world pos
  gl_FragData[2] = vec4(normalize(vNormal), 1.0); // normal
  gl_FragData[3] = vec4(vShadowCoord.z / vShadowCoord.w, vShadowCoord.z / vShadowCoord.w, vShadowCoord.z / vShadowCoord.w, 1.0);
}
