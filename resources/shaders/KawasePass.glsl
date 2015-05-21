// blurs data in bloomMap and renders to frame buffer
uniform sampler2D bloomMap;
// kernelSize should be between 0-3
uniform int kernelSize;
// window size (for edge cases)
uniform vec2 windowSize;

void main()
{
  int ksm = kernelSize - 1;
  vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  
  // upper right
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(kernelSize, kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(kernelSize, ksm), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm, kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm, ksm), vec2(0.0f, 0.0f), windowSize));

  // upper left
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * kernelSize, kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * kernelSize, ksm), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * ksm, kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * ksm, ksm), vec2(0.0f, 0.0f), windowSize));

  // lower right
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(kernelSize, -1.0f * kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(kernelSize, -1.0f * ksm), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm, -1.0f * kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(ksm, -1.0f * ksm), vec2(0.0f, 0.0f), windowSize));

  // lower left
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * kernelSize, -1.0f * kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * kernelSize, -1.0f * ksm), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * ksm, -1.0f * kernelSize), vec2(0.0f, 0.0f), windowSize));
  color += 0.0625f * texture2D(bloomMap, clamp(vec2(gl_FragCoord) + vec2(-1.0f * ksm, -1.0f * ksm), vec2(0.0f, 0.0f), windowSize));

  gl_FragColor = color;
}
