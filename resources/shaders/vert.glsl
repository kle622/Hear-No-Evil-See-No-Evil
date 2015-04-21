attribute vec3 aPosition;
attribute vec3 aNormal;
uniform mat4 uProjMatrix; // perspective
uniform mat4 uViewMatrix; // camera
uniform mat4 uModelMatrix; // global (except camera)
uniform vec3 uLightPos;
uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform int uShadeModel;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vLight;
varying vec3 vCol;
// CHECKPOINT!!!!!!!!!
void main()
{
	vec3 pre_pos = vec3(uModelMatrix * vec4(aPosition, 1.0));
	gl_Position = uProjMatrix * uViewMatrix * vec4(pre_pos, 1.0); // vec4
	vec3 light_dir = normalize(vec3(vec4(uLightPos, 0.0)) - pre_pos);
	vec3 normal_fin = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));

	if (uShadeModel == 0) {	// Normal Lighting
		vCol = 0.5 * (normalize(aNormal) + 1.0);
	}
	else if (uShadeModel == 1) { // Gourad Lighting
		vec3 diffuse = UdColor * dot(normal_fin, light_dir); // both must be noralized
		diffuse.x = diffuse.x < 0.0 ? 0.0: diffuse.x;
		diffuse.y = diffuse.y < 0.0 ? 0.0: diffuse.y;
		diffuse.z = diffuse.z < 0.0 ? 0.0: diffuse.z;
		float temp = dot(normal_fin, normalize(light_dir - normalize(pre_pos))); // both must be noralized
		temp = temp < 0.0 ? 0.0: temp;
		vec3 specular = UsColor * pow(temp, Ushine); // n=1
		vec3 ambient = UaColor;
		vCol = specular + diffuse + ambient;
	}
	else {	// Phong Lighting
		vNormal = normal_fin; // normalized
		vLight = light_dir;	// normalized
		vPos = pre_pos;
	}
}