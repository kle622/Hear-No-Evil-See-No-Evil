#define MAX_LIGHTS 2
uniform mat4 uModelMatrix;
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
   float coneAngle;
   vec3 coneDirection;
} allLights[MAX_LIGHTS];

// Cook Stuff
uniform float uMatRoughness;
uniform float uFresReflectance;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCol;
varying vec3 vLight;
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
	
	   ///////////// Cook stuff
	   vec3 viewDir = normalize(uCamPos - vec3(vPos));

	   /*float geometricAtten = min(1, (2 * dot(vNormal, halfDir) * (vNormal, viewDir))/dot(viewDir, halfDir), (2 * dot(vNormal, halfDir) * (vNormal, viewDir))/dot(viewDir, halfDir));
	   float roughness = (1/(uMatRoughness * pow(dot(vNormal, halfDir), 4)) * exp( (pow(dot(vNormal, halfDir), 2) - 1) / (pow(uMatRoughness,2) * pow(dot(vNormal, halfDir), 2)) );
	   float fresnel = uFresReflectance + pow(1-dot(halfDir, viewDir) , 5) * (1-uFresReflectance);
	   float Rs = (fresnel * roughness * geometricAtten)/(dot(vNormal, viewDir) * dot(vNormal, vLight));*/

	   float rs = cookTorrance(vNormal, vLight, viewDir, uFresReflectance, uMatRoughness); 
	   /////////////////////////


	if (hasTex == 1) {
	   diffuse = vec3(texture2D(texture, texCoordOut));
	   //diffuse = vec3(0.0, 1.0, 0.0);
	   //gl_FragColor = texture2D(texture, texCoordOut);
	   color += ((att * visibility * diffuse) + ambient, 1.0);
	   //gl_FragColor = vec4((att * visibility * ((max(dot(vNormal, vLight), 0) * (specular * rs)) + diffuse)) + ambient, 1.0);
	}
	else {	
	    /color = vec3(1.0, 0.0, 0.0);
	    //gl_FragColor = vec4(att * ((diffuse + specular)) + ambient, 1.0);
		color += (ambient + att * (diffuse + specular));
		//gl_FragColor = vec4((att * visibility * ((max(dot(vNormal, vLight), 0) * (specular * rs)) + diffuse)) + ambient, 1.0);
	} 

      gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
      gl_FragColor = vec4(color, 1.0);
}
