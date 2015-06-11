uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;

uniform vec2 uScreenSize;
uniform vec3 uCamPos;
uniform vec3 uLightPos;
uniform mat4 uView;
uniform vec3 uLightAtten;

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 color = texture2D(uColMap, texCoord).xyz;
  vec3 normal = texture2D(uNormMap, texCoord).xyz;

  vec3 light_color = vec3(0.5, 0.5, 0.243);

  // get vectors to light and camera
  vec3 toLight = vec3(uView * vec4(uLightPos, 1.0)) - worldPos;
  vec3 toCam = vec3(uView * vec4(uCamPos, 1.0)) - worldPos;

  // get distance for attenuation
  float lightDist = length(toLight);

  // normalize light/cam/normal vectors
  toLight = normalize(toLight);
  toCam = normalize(toCam);
  normal = normalize(normal);

  // distance attenuation
  //float attDist = 1.0;
  float attDist = min(1.0 / (uLightAtten.x + uLightAtten.y * lightDist + uLightAtten.z * lightDist * lightDist), 1.0);

  // final color assignment
  color = dot(normal, toLight) * attDist * color;
  gl_FragColor = vec4(color, 1.0);
}
