uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;

uniform vec2 uScreenSize;
uniform vec3 uLightPos;
uniform mat4 uView;
uniform vec3 uLightAtten;

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 color = texture2D(uColMap, texCoord).xyz;
  vec3 normal = texture2D(uNormMap, texCoord).xyz;

  vec3 light_color = vec3(0.5, 0.5, 0.243);
  vec3 viewPos = vec3(uView * vec4(worldPos, 1.0));

  // get vectors to light and camera
  vec3 toLight = uLightPos - worldPos;

  // get distance for attenuation
  float lightDist = length(toLight);

  // normalize light/cam/normal vectors
  toLight = normalize(toLight);

  // distance attenuation
  //float attDist = 1.0;
  float attDist = min(1.0 / (uLightAtten.x + uLightAtten.y * lightDist + uLightAtten.z * lightDist * lightDist), 1.0);

  // final color assignment
  color = max(dot(normal, toLight), 0.0) * attDist * color;
  gl_FragColor = vec4(color, 1.0);
}
