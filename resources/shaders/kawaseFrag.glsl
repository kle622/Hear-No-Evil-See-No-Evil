// blurs data in uTexture and renders to frame buffer
uniform sampler2D uTexture;
// uKernelSize should be between 0-3
uniform float uKernelSize;
// window size (for edge cases)
uniform vec2 uWindowSize;
varying vec2 vUV;

void main()
{
  float ksm = uKernelSize - 1.0f;
  vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  float xinv = 1.0f / uWindowSize.x;
  float yinv = 1.0f / uWindowSize.y;
  
  // upper right
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv, uKernelSize * yinv));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv, ksm * yinv));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv, uKernelSize * yinv));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv, ksm * yinv));
  
  // upper left
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv * -1.0f, uKernelSize * yinv));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv * -1.0f, ksm * yinv));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv * -1.0f, uKernelSize * yinv));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv * -1.0f, ksm * yinv));
  
  // lower right
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv, ksm * yinv * -1.0f));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv, ksm * yinv * -1.0f));
  
  // lower left
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv * -1.0f, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(uKernelSize * xinv * -1.0f, ksm * yinv * -1.0f));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv * -1.0f, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uTexture, vUV + vec2(ksm * xinv * -1.0f, ksm * yinv * -1.0f));
  
  gl_FragColor = color;
}
