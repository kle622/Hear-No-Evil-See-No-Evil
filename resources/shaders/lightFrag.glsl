uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;

uniform vec2 uScreenSize;
uniform vec3 uCamPos;

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 color = texture2D(uColMap, texCoord).xyz;
//  color = vec3(1.0, 1.0, 1.0);
  vec3 normal = texture2D(uNormMap, texCoord).xyz;
  normal = normalize(normal);
  vec3 light_color = vec3(0.5, 0.5, 0.243);

  gl_FragColor = vec4(light_color * color, 1.0);
}
