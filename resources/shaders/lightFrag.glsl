#define MAX_LIGHTS 15

uniform sampler2D uPosMap;
uniform sampler2D uColMap;
uniform sampler2D uNormMap;
uniform sampler2D uDepthMap;

uniform vec2 uScreenSize;
uniform vec3 uLightPos[MAX_LIGHTS];
uniform vec3 uLightCol;
uniform mat4 uView;
uniform mat4 uDepthMVP;
uniform vec3 uLightAtten;
uniform vec3 uLightDirection[MAX_LIGHTS];
uniform float uLightAngleCos;
uniform float uAmbient;
uniform int uNumLights;

uniform float uDetectionLevel;
uniform int detecDir;
uniform int uShadeType;


uniform vec2 pt1;
uniform vec2 pt2;
uniform vec2 pt3;

int isInTri() {
	float i = gl_FragCoord.x;
	float j = gl_FragCoord.y;
	float totalArea = (((pt2.x - pt1.x) * (pt3.y - pt1.y)) - ((pt3.x - pt1.x) * (pt2.y - pt1.y)));

	int is_inside = 0;

	float alpha, beta, gamma, zero, one;
	zero = 0.0; 
	one = 1.0;

	beta = (((pt1.x-pt3.x) * (j-pt3.y)) - ((i-pt3.x) * (pt1.y-pt3.y)));
	beta = beta/totalArea;

	gamma = (((pt2.x-pt1.x) * (j-pt1.y)) - ((i-pt1.x) * (pt2.y-pt1.y)));
	gamma = gamma/totalArea;

	alpha = 1.0 - beta - gamma;

	if(zero <= alpha && alpha <= one) {
		if(zero <= beta && beta <= one) {
			if(zero <= gamma && gamma <= one){
				is_inside = 1;
			}
		}
	}

	return is_inside;
}

void main() {
  vec2 texCoord = gl_FragCoord.xy / uScreenSize;
  vec3 worldPos = texture2D(uPosMap, texCoord).xyz;
  vec3 baseColor = texture2D(uColMap, texCoord).xyz;
  vec3 normal = texture2D(uNormMap, texCoord).xyz;
  vec3 diffuse = vec3(0.0, 0.0, 0.0);
  vec3 ambient = uAmbient * baseColor * 3.5;
  float visibility = 1.0;
  float bias = 0.005;
  vec3 viewPos = vec3(uView * vec4(worldPos, 1.0));
  vec4 shadowCoord = uDepthMVP * vec4(worldPos, 1.0);  

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
    float spotAngleCos = dot(-1.0 * toLight, normalize(uLightDirection[i]));

    // angle attenuation
    float attAngle = max((1.0 - (1.0 - spotAngleCos) * 1.0/(1.0 - uLightAngleCos)), 0.0);
    //float attAngle = max(spotAngleCos - uLightAngleCos, 0.0);

    // final color assignment
    //if (spotAngleCos > uLightAngleCos) {
      //diffuse += max(dot(normal, toLight), 0.0) * attDist * baseColor * uLightCol;
    //}
    diffuse += max(dot(normal, toLight), 0.0) * attDist * attAngle * baseColor * uLightCol;
  }

  if (texture2D(uDepthMap, (shadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/shadowCoord.w).z < (shadowCoord.z-bias) / shadowCoord.w) {
            visibility -= 0.1;
        }
        
        if (texture2D(uDepthMap, (shadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/shadowCoord.w).z <  (shadowCoord.z-bias) / shadowCoord.w) {
            visibility -= 0.1;
        }
        
        if (texture2D(uDepthMap, (shadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/shadowCoord.w).z <  (shadowCoord.z-bias) / shadowCoord.w) {
            visibility -= 0.1;
        }
        
        if (texture2D(uDepthMap, (shadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0)/shadowCoord.w).z <  (shadowCoord.z-bias) / shadowCoord.w) {
            visibility -= 0.1;
        }

  float avgDiffuse = (diffuse.x + diffuse.y + diffuse.z) * 0.3333333;
  diffuse = vec3((diffuse.r + ((avgDiffuse - diffuse.r) * uDetectionLevel)), (diffuse.g + ((avgDiffuse - diffuse.g) * uDetectionLevel)), (diffuse.b + ((avgDiffuse - diffuse.b) * uDetectionLevel)));
  
  gl_FragColor = vec4(ambient + (visibility * diffuse), 1.0);

  if (detecDir > 0) {
	    int isIn = isInTri();
		if(isIn == 1) {
			gl_FragColor = vec4(140.0/255.0, 18.0/255.0, 28.0/255.0, 1.0);
		}
		//gl_FragColor = vec4(140.0/255.0, 18.0/255.0, 28.0/255.0, 0.5);
  }
  if (uShadeType == 1) {
    gl_FragColor = vec4(normal, 1.0); // draw normal map
  } else if (uShadeType == 2) {
    gl_FragColor = vec4(worldPos, 1.0); // draw position map
  } else if (uShadeType == 3) {
    gl_FragColor = vec4(baseColor, 1.0); // draw color map
  }
}
