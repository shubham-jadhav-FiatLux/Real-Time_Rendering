
in vec4 a_position;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
out vec2 out_texcoords;
void main(void)				
{							
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position.xy, 0.0 , 1.0);
	out_texcoords = a_position.zw;
};