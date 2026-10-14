
in vec2 a_texcoord_out;
out vec4 FragColor;
uniform sampler2D u_textureSampler;
uniform vec4 u_color;

void main(void)
{
    FragColor = u_color;
}
