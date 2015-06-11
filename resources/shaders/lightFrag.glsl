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

uniform float uDetectionLevel;

// Cook Stuff
uniform float uMatRoughness;
uniform float uFresReflectance;

float cookTorrance(vec3 _normal, vec3 _light, vec3 _view, float _fresnel, float _roughness) {
  vec3  half_vec = normalize( _view + _light ); // vector H 
  // Now compute the various scalar products 
  float NdotL = max( dot(_normal, _light), 0.0 );
  float NdotV = max( dot(_normal, _view), 0.0 );
  float NdotH = max( dot(_normal, half_vec), 1.0e-7 );
  float VdotH = max( dot(_view, half_vec), 1.0e-7 );

  // geometric component
  float geometric = 2.0 * NdotH / VdotH;
  geometric = min( 1.0, geometric * min(NdotV, NdotL) );

  // roughness
  float r_sq = _roughness * _roughness;
  float NdotH_sq = NdotH * NdotH;
  float NdotH_sq_r = 1.0 / (NdotH_sq * r_sq);
  float roughness_exp = (NdotH_sq - 1.0) * ( NdotH_sq_r );
  float roughness = 0.25 * exp(roughness_exp) * NdotH_sq_r / NdotH_sq;

  // final result
  return min(1.0, _fresnel * geometric * roughness / (NdotV + 1.0e-7));
}

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
  vec3 ambient = uAmbient * baseColor;
  vec3 diffuse = vec3(0.0, 0.0, 0.0);
  if (spotAngleCos > uLightAngleCos) {
    diffuse += max(dot(normal, toLight), 0.0) * attDist * baseColor * uLightCol;
  }
  float avgDiffuse = (diffuse.x + diffuse.y + diffuse.z) * 0.3333333;
  diffuse = vec3((diffuse.r + ((avgDiffuse - diffuse.r) * uDetectionLevel)), (diffuse.g + ((avgDiffuse - diffuse.g) * uDetectionLevel)), (diffuse.b + ((avgDiffuse - diffuse.b) * uDetectionLevel)));
  
  gl_FragColor = vec4(ambient + diffuse, 1.0);
}
