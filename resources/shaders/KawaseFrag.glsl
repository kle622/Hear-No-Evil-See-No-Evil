// blurs data in uBloomMap and renders to frame buffer
uniform sampler2D uBloomMap;
// uKernelSize should be between 0-3
uniform float uKernelSize;
// window size (for edge cases)
uniform vec2 uWindowSize;
varying vec2 vUV;

void main()
{
  float ksm = uKernelSize - 1.0f;
  vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  float xinv = 1.0f / uWindowSize.x;
  float yinv = 1.0f / uWindowSize.y;
  
  // upper right
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(uKernelSize * xinv, uKernelSize * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(uKernelSize * xinv, ksm * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(ksm * xinv, uKernelSize * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(ksm * xinv, ksm * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  
  // upper left
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(uKernelSize * xinv * -1.0f, uKernelSize * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(uKernelSize * xinv * -1.0f, ksm * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(ksm * xinv * -1.0f, uKernelSize * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  color += 0.0625f * texture2D(uBloomMap, clamp(vUV + vec2(ksm * xinv * -1.0f, ksm * yinv), vec2(0.0f, 0.0f), vec2(1.0f, 1.0f)));
  
  // lower right
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(uKernelSize * xinv, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(uKernelSize * xinv, ksm * yinv * -1.0f));
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(ksm * xinv, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(ksm * xinv, ksm * yinv * -1.0f));
  
  // lower left
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(uKernelSize * xinv * -1.0f, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(uKernelSize * xinv * -1.0f, ksm * yinv * -1.0f));
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(ksm * xinv * -1.0f, uKernelSize * yinv * -1.0f));
  color += 0.0625f * texture2D(uBloomMap, vUV + vec2(ksm * xinv * -1.0f, ksm * yinv * -1.0f));
  
  gl_FragColor = color;
  //gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  //gl_FragColor = texture2D(uBloomMap, vUV);
}
