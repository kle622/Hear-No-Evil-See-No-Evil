uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;

uniform vec2 uScreenSize;
uniform vec3 uCamPos;

varying vec2 vUV;

void main() {
  vec2 texCoord = vUV;
  //vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  worldPos = texture2D(uPosMap, gl_TexCoord[0].xy).xyz;
  //vec3 color = texture2D(uColMap, texCoord).xyz;
  vec3 color = vec3(1.0, 0.0, 0.0);
  vec3 normal = texture2D(uNormMap, texCoord).xyz;
  normal = texture2D(uNormMap, gl_TexCoord[0].xy).xyz;
  normal = normalize(normal);

  gl_FragColor = vec4(color, 1.0);
}
