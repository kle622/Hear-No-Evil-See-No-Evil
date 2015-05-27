#define MAX_LIGHTS 10
uniform mat4 uModelMatrix;
uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uCamPos;
uniform sampler2D shadowMap;
uniform sampler2D texture;
uniform int hasTex;
uniform int numLights;
uniform float coneAngle;
uniform vec3 coneDirection;
uniform vec3 allLights[MAX_LIGHTS];
uniform vec3 uLightPos;

// Cook Stuff
uniform float uMatRoughness;
uniform float uFresReflectance;

// Amount to color drain
uniform float detectionLevel;

varying vec3 vNormal;
varying vec3 vCol;
varying vec3 vPos;
varying vec4 ShadowCoord;
varying vec2 texCoordOut;

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

/*vec3 drainColor(vec3 color) {
	  vec3 newColor = (0.0, 0.0, 0.0);
      float avgColor = (color.r + color.b + color.g)/3.0;
	  newColor.r = color.r + ((avgColor - color.r) * detectionLevel)/256.0;
	  newColor.g = color.g + ((avgColor - color.g) * detectionLevel)/256.0;
	  newColor.b = color.b + ((avgColor - color.b) * detectionLevel)/256.0;
	  //red = red1 + ((red2 - red1) * stage / 256)

	  return newColor;
} */

// CHECKPOINT!!!!!!!!!
void main() {
     vec3 color = vec3(0.0, 0.0, 0.0);
     float visibility = 1.0;
     float att = 1.0;
	 float clrBleedVal = 0.3;
     vec3 ambient = UaColor * 0.05;
     
   for (int i = 0; i < numLights; i++) {
     float visibility = 1.0;
     float att = 1.0;
     vec3 lightPos = allLights[i];
     vec3 surfacePos = vPos;
     vec3 surfaceToCamera = normalize(uCamPos - surfacePos);
     vec3 surfaceToLight = normalize(lightPos - surfacePos);     
     float bias = 0.005 * tan(acos(dot(vNormal, surfaceToLight)));
     bias = clamp(bias, 0.0, 0.01);
     vec3 spotDir = normalize(coneDirection);
     float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, spotDir)));
     
     if (lightToSurfaceAngle > coneAngle) {
     	att = 0.2;
     }
	   vec3 diffuse = UdColor * dot(vNormal, surfaceToLight);
	   diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
	   diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
	   diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
	   float temp = dot(surfaceToCamera, reflect(-surfaceToLight, vNormal));
	   temp = temp < 0.0 ? 0.0: temp;
	   vec3 specular = UsColor * pow(temp, Ushine); // n=1

	
	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( -0.94201624, -0.39906216, 0.0) / 700.0).z / ShadowCoord.w <  (ShadowCoord.z-bias) / ShadowCoord.w) {
	   visibility -= 0.1;
	}
	
	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( 0.94558609, -0.76890725, 0.0)  / 700.0).z / ShadowCoord.w <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.1;
        }

	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( -0.094184101, -0.9293887, 0.0)  / 700.0).z / ShadowCoord.w <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.1;
	}

	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( 0.34495938, 0.29387760, 0.0) / 700.0).z / ShadowCoord.w  <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.1;
    	}
	
	   ///////////// Cook stuff
	   //vec3 viewDir = normalize(uCamPos - vec3(vPos));
	   //float rs = cookTorrance(vNormal, vLight, viewDir, uFresReflectance, uMatRoughness); 
	   //gl_FragColor = vec4((att * visibility * ((max(dot(vNormal, vLight), 0) * (specular * rs)) + diffuse)) + ambient, 1.0);

	   /////////////////////////

	
    float avgAmbient = (ambient.r + ambient.b + ambient.g)/3.0;
    float avgSpecular = (specular.r + specular.b + specular.g)/3.0;

	if (hasTex == 1) {
	   diffuse = vec3(texture2D(texture, texCoordOut));
	   //diffuse = vec3(0.0, 1.0, 0.0);
	   //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	  // gl_FragColor = texture2D(texture, texCoordOut);
	   //gl_FragColor = vec4((att * diffuse) + ambient, 1.0);
	  color += (ambient + visibility * att * diffuse);
	  //color = vec3(0.0, 1.0, 0.0);
	}
	else {	
	     	//color = vec3(1.0, 0.0, 0.0);
	        //gl_FragColor = vec4(att *((diffuse + specular)) + ambient, 1.0);
		color += (ambient + att * visibility * (diffuse + specular));
	}
     	 
    }

      gl_FragColor = vec4(color, 1.0);
}
