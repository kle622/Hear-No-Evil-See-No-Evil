varying vec4 ShadowCoord;

void main() {

     gl_FragDepth = ShadowCoord.z;
}