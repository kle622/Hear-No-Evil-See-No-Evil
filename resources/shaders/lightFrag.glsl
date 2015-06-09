uniform sampler2D g_pos_map;
uniform sampler2D g_col_map;
uniform sampler2D g_norm_map;
uniform vec2 screenSize;

void main() {
  vec2 texCoord = gl_FragCoord.xy / screenSize;
  vec3 worldPos = texture2D(g_pos_map, texCoord).xyz;
  vec3 color = texture2D(g_col_map, texCoord).xyz;
  vec3 normal = texture2D(g_norm_map, texCoord).xyz;
  normal = normalize(normal);

  gl_FragColor = vec4(color, 1.0f);
}
