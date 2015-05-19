uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uLightPos;
uniform vec3 uCamPos;
uniform sampler2D shadowMap;
uniform sampler2D texture;
uniform int hasTex;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCol;
varying vec3 vLight;
varying vec4 ShadowCoord;
varying vec2 texCoordOut;

// CHECKPOINT!!!!!!!!!
void main() {
     float visibility = 1.0;
     float bias = 0.005 * tan(acos(dot(vNormal, vLight)));
     bias = clamp(bias, 0.0, 0.01);

	vec3 diffuse = UdColor * dot(vNormal, vLight);
	diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
	diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
	diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
	float temp = dot(vNormal, normalize(normalize(uCamPos - vec3(vPos)) + vLight));
	temp = temp < 0.0 ? 0.0: temp;
	vec3 specular = UsColor * pow(temp, Ushine); // n=1
	vec3 ambient = UaColor * 0.3;

	if (texture2D(shadowMap, ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0).z  <  ShadowCoord.z-bias) {
	   visibility -= 0.05;
	}
	
	if (texture2D(shadowMap, ShadowCoord.xy + vec2( 0.94558609, -0.76890725)  / 700.0).z  <  ShadowCoord.z-bias) {
           visibility -= 0.05;
        }

	if (texture2D(shadowMap, ShadowCoord.xy + vec2( -0.094184101, -0.9293887)  / 700.0).z  <  ShadowCoord.z-bias) {
           visibility -= 0.05;
	}

	if (texture2D(shadowMap, ShadowCoord.xy + vec2( 0.34495938, 0.29387760) / 700.0).z  <  ShadowCoord.z-bias) {
           visibility -= 0.05;
    	}
	

/*	visibility -= 0.2 *  (1.0 - texture2D(shadowMap, vec2(vec3(ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0, 
           (ShadowCoord.z-bias)/ShadowCoord.w))).z);
	visibility -= 0.2 * (1.0 - texture2D(shadowMap, vec2(vec3(ShadowCoord.xy + vec2( 0.94558609, -0.76890725)  / 700.0, 
	   (ShadowCoord.z-bias)/ShadowCoord.w))).z);
	visibility -= 0.2 * (1.0 - texture2D(shadowMap, vec2(vec3(ShadowCoord.xy +  vec2( -0.094184101, -0.9293887)  / 700.0, 
	   (ShadowCoord.z-bias)/ShadowCoord.w))).z);
	visibility -= 0.2 * (1.0 - texture2D(shadowMap, vec2(vec3(ShadowCoord.xy + vec2( 0.34495938, 0.29387760) / 700.0, 
	   (ShadowCoord.z-bias)/ShadowCoord.w))).z);*/

	/*visibility -= 0.2 *  (1.0 - texture2D(shadowMap, ShadowCoord.xy + vec2( -0.94201624, -0.39906216) / 700.0).z);
        visibility -= 0.2 * (1.0 - texture2D(shadowMap, ShadowCoord.xy + vec2( 0.94558609, -0.76890725)  / 700.0).z);
        visibility -= 0.2 * (1.0 - texture2D(shadowMap, ShadowCoord.xy +  vec2( -0.094184101, -0.9293887)  / 700.0).z);
        visibility -= 0.2 * (1.0 - texture2D(shadowMap, ShadowCoord.xy + vec2( 0.34495938, 0.29387760) / 700.0).z);*/
	
	if (hasTex == 1) {
	   //diffuse = vec3(texture2D(texture, texCoordOut));
	   //diffuse = vec3(0.0, 1.0, 0.0);
	   gl_FragColor = texture2D(texture, texCoordOut);
	}
	else {	
	     gl_FragColor = vec4(visibility * (diffuse + specular + ambient), 1.0);
	}
}
