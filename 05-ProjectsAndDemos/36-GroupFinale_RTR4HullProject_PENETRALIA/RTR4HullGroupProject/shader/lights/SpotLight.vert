

in vec4 a_position;
in vec4 a_color;
in vec3 a_normals;

out vec3 FragPos;
out vec3 Normal;

uniform int u_lightingEnabled;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec4 a_color_out;

void main(void)
{
	if(u_lightingEnabled == 1)
	{
		FragPos = vec3(u_modelMatrix * a_position);
		Normal = mat3(transpose(inverse(u_modelMatrix))) * a_normals;
		gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
	}
	else
	{
		gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
		a_color_out = a_color;
	}
}
