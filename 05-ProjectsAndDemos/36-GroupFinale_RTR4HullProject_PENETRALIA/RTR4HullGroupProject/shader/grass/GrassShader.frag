in vec2 texcoord_out;
in float colorVariation_out;
uniform sampler2D u_textureSampler;
out vec4 FragColor;
void main(void)
{
vec4 color = texture(u_textureSampler, texcoord_out);
if (color.a < 0.25) discard;
color.xyz = mix(color.xyz, 0.5 * color.xyz, colorVariation_out);
FragColor = color;
}; 