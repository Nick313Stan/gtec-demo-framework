#version 310 es

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std140, binding = 0) uniform UBO
{
  // Tonemapping
  uniform float Exposure;
}
g_ubo;

mediump layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput g_hdrColorAttachment;

layout(location = 0) out vec4 FragColor;

void main()
{
  vec3 hdrColor = subpassLoad(g_hdrColorAttachment).rgb;

  // Exposure
  hdrColor *= g_ubo.Exposure;

  // Simple reinhard
  hdrColor = hdrColor / (1.0f + hdrColor);

  // Gamma correction
  hdrColor = pow(hdrColor, vec3(1.0 / 2.2));
  FragColor = vec4(hdrColor, 1.0);
}
