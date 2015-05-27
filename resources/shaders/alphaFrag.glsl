// blurs data in uBloomMap and renders to frame buffer
uniform sampler2D uTexture;
// uKernelSize should be between 0-3
// window size (for edge cases)
uniform vec2 uWindowSize;
varying vec2 vUV;

void main()
{
  vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  gl_FragColor = color;
}
