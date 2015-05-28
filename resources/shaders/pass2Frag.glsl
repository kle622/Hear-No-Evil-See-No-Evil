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

varying vec3 vNormal;
varying vec3 vCol;
varying vec3 vPos;
varying vec4 ShadowCoord;
varying vec2 texCoordOut;

// CHECKPOINT!!!!!!!!!
void main() {
     
     vec3 color = vec3(0.0, 0.0, 0.0);
     vec3 ambient = UaColor * 0.5;
     //float bias = 0.005;
     float visibility = 0.4;

   for (int i = 0; i < numLights; i++) {
     float visibility = 1.0;
     float att = 1.5;
     vec3 lightPos = allLights[i];
     vec3 surfacePos = vPos;
     vec3 surfaceToCamera = normalize(uCamPos - surfacePos);
     vec3 surfaceToLight = normalize(lightPos - surfacePos);     
     //float bias = 0.005 * tan(acos(dot(vNormal, surfaceToLight)));
     //bias = clamp(bias, 0.0, 0.01);
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
	
	if (hasTex == 1) {
	   diffuse = vec3(texture2D(texture, texCoordOut));
	   color += att * diffuse;
	}
	else {	
	   color += (att * (diffuse + specular));
	}
   }	
     	 
//vec2( -0.94201624, -0.39906216) / 700.0
//vec2( 0.94558609, -0.76890725)  / 700.0
//vec2( -0.094184101, -0.9293887)  / 700.0
//vec2( 0.34495938, 0.29387760) / 700.0
/*	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z < (ShadowCoord.z-bias) / ShadowCoord.w) {
	   visibility -= 0.2;
	}
	
	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.2;
        }

	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility -= 0.2;
	}

	if (texture2D(shadowMap, ShadowCoord.xy/ShadowCoord.w).z <  (ShadowCoord.z-bias) / ShadowCoord.w) {
           visibility = 0.4;
    	}*/

    gl_FragColor = vec4(visibility * color, 1.0);
}
