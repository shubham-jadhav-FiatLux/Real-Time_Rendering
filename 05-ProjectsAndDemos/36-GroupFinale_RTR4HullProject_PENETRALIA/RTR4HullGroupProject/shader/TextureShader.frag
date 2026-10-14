
in vec2 a_texcoord_out;
out vec4 FragColor;
uniform sampler2D u_textureSampler;

void main(void)
{
    FragColor = texture(u_textureSampler, a_texcoord_out);
}
