
in vec4 a_position;

const float tiling = 6.0;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec3 u_cameraPosition;
uniform vec3 u_lightPosition;
out vec4 out_clip_space;
out vec2 out_texcoord;
out vec3 out_to_camera_position;
out vec3 out_fromLightVector;

void main(void)
{
	vec4 worldPosition = u_modelMatrix * vec4(a_position.x, 0.0, a_position.z, 1.0);
	out_clip_space = u_projectionMatrix * u_viewMatrix * worldPosition;
	gl_Position = out_clip_space;
	out_texcoord = vec2(a_position.x/2.0 + 0.5, a_position.z/2.0 + 0.5) * tiling;
	out_to_camera_position = u_cameraPosition - worldPosition.xyz;
	out_fromLightVector = worldPosition.xyz - u_lightPosition;
} ;
