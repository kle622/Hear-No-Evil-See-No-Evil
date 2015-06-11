#define MAX_LIGHTS 15

uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;

uniform vec2 uScreenSize;
uniform vec3 uLightPos[MAX_LIGHTS];
uniform vec3 uLightCol;
uniform mat4 uView;
uniform vec3 uLightAtten;
uniform vec3 uLightDirection;
uniform float uLightAngleCos;
uniform float uAmbient;
uniform int uNumLights;

uniform float uDetectionLevel;

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 baseColor = texture2D(uColMap, texCoord).xyz;
  vec3 normal = texture2D(uNormMap, texCoord).xyz;
  vec3 diffuse = vec3(0.0, 0.0, 0.0);
  vec3 ambient = uAmbient * baseColor;

  vec3 viewPos = vec3(uView * vec4(worldPos, 1.0));

  for (int i = 0; i < uNumLights; ++i) {
    // get vectors to light and camera
    vec3 lightPos = uLightPos[i];
    vec3 toLight = lightPos - worldPos;

    // get distance for attenuation
    float lightDist = length(toLight);

    // normalize light/cam/normal vectors
    toLight = normalize(toLight);

    // distance attenuation
    float attDist = min(1.0 / (uLightAtten.x + uLightAtten.y * lightDist + uLightAtten.z * lightDist * lightDist), 1.0);

    // spot light angle calculation
    float spotAngleCos = dot(-1.0 * toLight, normalize(uLightDirection));

    // final color assignment
    if (spotAngleCos > uLightAngleCos) {
      diffuse += max(dot(normal, toLight), 0.0) * attDist * baseColor * uLightCol;
    }
  }

  float avgDiffuse = (diffuse.x + diffuse.y + diffuse.z) * 0.3333333;
  diffuse = vec3((diffuse.r + ((avgDiffuse - diffuse.r) * uDetectionLevel)), (diffuse.g + ((avgDiffuse - diffuse.g) * uDetectionLevel)), (diffuse.b + ((avgDiffuse - diffuse.b) * uDetectionLevel)));
  
  gl_FragColor = vec4(ambient + diffuse, 1.0);
}
