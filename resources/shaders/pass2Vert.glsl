attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 texCoordIn;

uniform mat4 uProjMatrix; // perspective
uniform mat4 uViewMatrix; // camera
uniform mat4 uModelMatrix; // global (except camera)
uniform vec3 uLightPos;
uniform vec3 UaColor;	// ambient
uniform vec3 UdColor;	// diffuse
uniform vec3 UsColor;	// specular
uniform float Ushine;
uniform mat4 uDepthMVP;
uniform int hasTex;

varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vLight;
varying vec3 vCol;
varying vec4 ShadowCoord;
varying vec2 texCoordOut;

// CHECKPOINT!!!!!!!!!
void main() {
     	vec3 light = vec3(5.0, 20.0, -3.0);
	vec3 pre_pos = vec3(uModelMatrix * vec4(aPosition, 1.0));
	gl_Position = uProjMatrix * uViewMatrix * vec4(pre_pos, 1.0); // vec4
	//vec3 light_dir = normalize(uLightPos - pre_pos);
	vec3 normal_fin = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));

	vNormal = normal_fin; // normalized
	vLight = normalize(light - pre_pos);	// normalized
	//vLight = normalize(uLightPos - pre_pos);
	vPos = pre_pos;
	
	ShadowCoord = uDepthMVP * vec4(aPosition, 1.0);
	
	if (hasTex == 1) 
	   texCoordOut = texCoordIn;
}