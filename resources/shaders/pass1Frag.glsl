varying vec4 ShadowCoord;

void main() {

     gl_FragDepth = gl_FragCoord.z;
     gl_FragColor = vec4(ShadowCoord.z, ShadowCoord.z, ShadowCoord.z, 1.0);
}