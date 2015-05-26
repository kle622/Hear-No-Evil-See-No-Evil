#define MAX_LIGHTS 2

uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uCamPos;
uniform sampler2D shadowMap;
uniform sampler2D texture;
uniform int hasTex;
uniform vec3 uLightPos;
uniform int numLights;

uniform struct Light {
   vec3 position;
   vec3 intensities;
   float attenuation;
   float coneAngle;
   vec3 coneDirection;
} allLights[MAX_LIGHTS];

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCol;
varying vec3 vLight;
varying vec4 ShadowCoord;
varying vec2 texCoordOut;

// CHECKPOINT!!!!!!!!!
void main() {
     
     vec3 color = vec3(0.0, 0.0, 0.0);
     //vec3 light = vec3(100.0, 150.0, -10.0);
     float visibility = 1.0;
     float bias = 0.005 * tan(acos(dot(vNormal, vLight)));
     bias = clamp(bias, 0.0, 0.01);
     float att = 1.0;
     vec3 ambient = UaColor * 0.2;
     
   for (int i = 0; i < numLights; i++) {

     Light light = allLights[i];
     vec3 surfaceToLight = normalize(light.position - vPos);     
     vec3 spotDir = normalize(-light.coneDirection);
     float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, spotDir)));
     
     if (lightToSurfaceAngle > light.coneAngle) {
     	att = 0.2;
     }
	   vec3 diffuse = UdColor * dot(vNormal, vLight);
	   diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
	   diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
	   diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
	   float temp = dot(vNormal, normalize(normalize(uCamPos - vec3(vPos)) + vLight));
	   temp = temp < 0.0 ? 0.0: temp;
	   vec3 specular = UsColor * pow(temp, Ushine); // n=1

	
	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( -0.94201624, -0.39906216, 0.0) / 700.0).z  <  ShadowCoord.z-bias) {
	   visibility -= 0.1;
	}
	
	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( 0.94558609, -0.76890725, 0.0)  / 700.0).z  <  ShadowCoord.z-bias) {
           visibility -= 0.1;
        }

	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( -0.094184101, -0.9293887, 0.0)  / 700.0).z  <  ShadowCoord.z-bias) {
           visibility -= 0.1;
	}

	if (texture2DProj(shadowMap, ShadowCoord.xyz + vec3( 0.34495938, 0.29387760, 0.0) / 700.0).z  <  ShadowCoord.z-bias) {
           visibility -= 0.1;
    	}
	
	if (hasTex == 1) {
	   diffuse = vec3(texture2D(texture, texCoordOut));
	   //diffuse = vec3(0.0, 1.0, 0.0);
	   //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	  // gl_FragColor = texture2D(texture, texCoordOut);
	   //gl_FragColor = vec4((att * diffuse) + ambient, 1.0);
	  color += (ambient + att * diffuse);
	  //color = vec3(0.0, 1.0, 0.0);
	}
	else {	
	     	//color = vec3(1.0, 0.0, 0.0);
	        //gl_FragColor = vec4(att * ((diffuse + specular)) + ambient, 1.0);
		color += (ambient + att * (diffuse + specular));
	}
     	 
    }

      //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
      gl_FragColor = vec4(color, 1.0);
}
