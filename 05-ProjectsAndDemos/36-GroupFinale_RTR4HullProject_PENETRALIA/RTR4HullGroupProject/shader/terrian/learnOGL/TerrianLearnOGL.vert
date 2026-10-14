in vec3 a_position;
in vec2 a_texcoord;

out vec2 out_a_texcoord;

void main(void)
{
	out_a_texcoord = a_texcoord;
	gl_Position = vec4(a_position, 1.0);
};
