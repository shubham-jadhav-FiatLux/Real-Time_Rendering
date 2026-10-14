
in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec4 u_lightPosition;
		

uniform int u_lightingEnabled;

out vec3 transformedNormal_out;
out vec3 lightDirection_out;
out vec3 viewVector_out;
out vec2 a_texcoord_out;

void main(void)
{
	if(u_lightingEnabled == 1)
	{
		/*phong light inverse chi garaj nahi aahe*/

		vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;
		mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);
		transformedNormal_out = normalMatrix * a_normal;
		lightDirection_out = vec3(u_lightPosition) - eyeCoordinates.xyz;
		viewVector_out = -eyeCoordinates.xyz;
				
	}
	a_texcoord_out = a_texcoord;
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;\

};