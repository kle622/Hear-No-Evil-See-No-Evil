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
uniform mat4 uDepthMVP;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vLight;
varying vec3 vCol;
varying vec4 ShadowCoord;

// CHECKPOINT!!!!!!!!!
void main()
{
	vec3 pre_pos = vec3(uModelMatrix * vec4(aPosition, 1.0));
	gl_Position = uProjMatrix * uViewMatrix * vec4(pre_pos, 1.0); // vec4
	vec3 light_dir = normalize(vec3(vec4(uLightPos, 0.0)) - pre_pos);
	vec3 normal_fin = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));

	vNormal = normal_fin; // normalized
	vLight = light_dir;	// normalized
	vPos = pre_pos;
	
	ShadowCoord = uDepthMVP * vec4(aPosition, 1.0);
}