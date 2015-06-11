uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;

uniform vec2 uScreenSize;
uniform vec3 uLightPos;
uniform vec3 uLightCol;
uniform mat4 uView;
uniform vec3 uLightAtten;
uniform vec3 uLightDirection;
uniform float uLightAngleCos;
uniform float uAmbient;

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 baseColor = texture2D(uColMap, texCoord).xyz;
  vec3 normal = texture2D(uNormMap, texCoord).xyz;

  vec3 viewPos = vec3(uView * vec4(worldPos, 1.0));

  // get vectors to light and camera
  vec3 toLight = uLightPos - worldPos;

  // get distance for attenuation
  float lightDist = length(toLight);

  // normalize light/cam/normal vectors
  toLight = normalize(toLight);

  // distance attenuation
  float attDist = min(1.0 / (uLightAtten.x + uLightAtten.y * lightDist + uLightAtten.z * lightDist * lightDist), 1.0);

  // spot light angle calculation
  float spotAngleCos = dot(-1.0 * toLight, normalize(uLightDirection));

  // final color assignment
  vec3 finalColor = uAmbient * baseColor;
  if (spotAngleCos > uLightAngleCos) {
    finalColor += max(dot(normal, toLight), 0.0) * attDist * baseColor * uLightCol;
  }
  gl_FragColor = vec4(finalColor, 1.0);
}
