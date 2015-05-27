// blurs data in uBloomMap and renders to frame buffer
uniform sampler2D uTexture;
uniform float uAlpha;
varying vec2 vUV;

void main()
{
  vec4 color = texture2D(uTexture, vUV);
  color.w = uAlpha;
  if (!(color.r > 0.0f && color.g > 0.0f && color.b > 0.0f)) {
    discard;
  }
  gl_FragColor = color;
}
