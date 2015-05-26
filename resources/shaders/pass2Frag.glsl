uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uLightPos;
uniform vec3 uCamPos;
uniform sampler2D shadowMap;
uniform sampler2D texture;
uniform int hasTex;
uniform float coneAngle;
uniform vec3 coneDirection;
uniform float attenuation;

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
     float visibility = 1.0;
     float bias = 0.005 * tan(acos(dot(vNormal, vLight)));
     bias = clamp(bias, 0.0, 0.01);
     float att = 1.0;
     vec3 ambient = UaColor * 0.1;

     vec3 surfaceToLight = normalize(vLight - vPos);     
     vec3 spotDir = normalize(-coneDirection);
     float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, spotDir)));
     
     if (lightToSurfaceAngle > coneAngle) {
     	att = 0.5;
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
	   //gl_FragColor = texture2D(texture, texCoordOut);
	   gl_FragColor = vec4((att * visibility * diffuse) + ambient, 1.0);
	}
	else {	
	        gl_FragColor = vec4(att * (visibility * (diffuse + specular)) + ambient, 1.0);
	}
}
