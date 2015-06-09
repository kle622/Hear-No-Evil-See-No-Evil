uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;
uniform vec2 uScreenSize;

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 color = texture2D(uColMap, texCoord).xyz;
  vec3 normal = texture2D(uNormMap, texCoord).xyz;
  normal = normalize(normal);

  gl_FragColor = vec4(color, 1.0f);
}
