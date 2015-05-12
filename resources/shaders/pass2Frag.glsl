uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform int uShadeModel;
uniform vec3 uLightPos;
uniform vec3 uCamPos;
uniform sampler2D shadowMap;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vCol;
varying vec3 vLight;
varying vec4 ShadowCoord;

// CHECKPOINT!!!!!!!!!
void main() {
     float visibility = 1.0;
     float bias = 0.005;
	vec3 normal_fin = normalize(vNormal);
	vec3 light_dir = normalize(vLight);
	//vec3 light_dir = normalize(uLightPos - vec3(gl_FragCoord));

	vec3 diffuse = UdColor * dot(normal_fin, light_dir);
	diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
	diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
	diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
	float temp = dot(normal_fin, normalize(normalize(uCamPos - vec3(vPos)) + vLight));
	temp = temp < 0.0 ? 0.0: temp;
	vec3 specular = UsColor * pow(temp, Ushine); // n=1
	vec3 ambient = UaColor;

	if ( texture2D(shadowMap, ShadowCoord.xy).z < ShadowCoord.z - bias) {
           visibility = 0.25;
    	}
	
	gl_FragColor = vec4(visibility * (diffuse + specular) + ambient, 1.0);
}
