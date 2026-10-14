in vec4 a_position;
in vec4 a_color;

uniform float time;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

flat out vec4 starColor;

void main(void)
{
	vec4 newVertex = a_position;
	newVertex.z += time;
	newVertex.z = fract(newVertex.z);
	float size = (100.0 * newVertex.z * newVertex.z);
	starColor = smoothstep(1.0, 3.0, size) * a_color * a_color;
	newVertex.z = (999.9 * newVertex.z) - 1000.0;

	gl_Position = u_projectionMatrix * u_modelMatrix * u_viewMatrix * newVertex;
	gl_PointSize = size;
}