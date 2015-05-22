// blurs data in uBloomMap and renders to frame buffer
uniform sampler2D uBloomMap;
// uKernelSize should be between 0-3
uniform int uKernelSize;
// window size (for edge cases)
uniform vec2 uWindowSize;

void main()
{
  /*int ksm = uKernelSize - 1;
  //vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  
  // upper right
  /*color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) +
  vec2(uKernelSize, uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) +
  vec2(uKernelSize, ksm), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm,
  uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm,
  ksm), vec2(0.0f, 0.0f), uWindowSize));

  // upper left
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1 * uKernelSize, uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * uKernelSize, ksm), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * ksm, uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * ksm, ksm), vec2(0.0f, 0.0f), uWindowSize));

  // lower right
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) +
  vec2(uKernelSize, -1 * uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) +
  vec2(uKernelSize, -1 * ksm), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm,
  -1 * uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm,
  -1 * ksm), vec2(0.0f, 0.0f), uWindowSize));

  // lower left
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * uKernelSize, -1 * uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * uKernelSize, -1 * ksm), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * ksm, -1 * uKernelSize), vec2(0.0f, 0.0f), uWindowSize));
  color += 0.0625f * texture2D(uBloomMap, clamp(vec2(gl_FragCoord) + vec2(-1
  * ksm, -1 * ksm), vec2(0.0f, 0.0f), uWindowSize));

  gl_FragColor = color;*/
  gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}
