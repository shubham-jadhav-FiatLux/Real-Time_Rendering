in vec4 a_position;
in vec2 a_texcoord;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
out vec2 out_a_texcoord;
void main(void)
{
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
	out_a_texcoord = a_texcoord;
}
