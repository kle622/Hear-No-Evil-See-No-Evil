// normals are in model space
varying vec3 vNormal;
varying vec3 vWorldPos;
varying vec2 vTexCoord;

uniform sampler2D texture;

void main() {
  gl_FragData[0] = texture2D(texture, vTexCoord); // diffuse
  gl_FragData[0] = vec4(texture2D(texture, gl_TexCoord[0].st).rgb, 0.0);
  gl_FragData[1] = vec4(vWorldPos, 0.0); // world pos
  gl_FragData[2] = vec4(vNormal, 0.0); // normal
}
