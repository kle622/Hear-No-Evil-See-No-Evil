uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform vec3 uLightPos;
uniform vec3 uCamPos;
uniform sampler2D shadowMap;

// Cook Stuff
uniform float uMatRoughness;
uniform float uFresReflectance;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCol;
varying vec3 vLight;
varying vec4 ShadowCoord;

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

	vec3 diffuse = UdColor * dot(vNormal, vLight); // lambertian
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

    ///////////// Cook stuff
	vec3 viewDir = normalize(uCamPos - vec3(vPos));

	/* float geometricAtten = min(1, (2 * dot(vNormal, halfDir) * (vNormal, viewDir))/dot(viewDir, halfDir), (2 * dot(vNormal, halfDir) * (vNormal, viewDir))/dot(viewDir, halfDir));
	float roughness = (1/(uMatRoughness * pow(dot(vNormal, halfDir), 4)) * exp( (pow(dot(vNormal, halfDir), 2) - 1) / (pow(uMatRoughness,2) * pow(dot(vNormal, halfDir), 2)) );
	float fresnel = uFresReflectance + pow(1-dot(halfDir, viewDir) , 5) * (1-uFresReflectance);
	float Rs = (fresnel * roughness * geometricAtten)/(dot(vNormal, viewDir) * dot(vNormal, vLight)); */

	float rs = cookTorrance(vNormal, vLight, viewDir, uFresReflectance, uMatRoughness); 
	
	gl_FragColor = vec4(max(dot(vNormal, vLight), 0) * ((specular * rs) + diffuse), 1.0);

	
	//////////////
	 	
	//gl_FragColor = vec4(visibility * (diffuse + specular + ambient), 1.0);
	//gl_FragColor = texture2D(shadowMap, ShadowCoord.xy);
	//gl_FragColor = vec4(ShadowCoord.x, 0.0, ShadowCoord.z, 1.0);
}
