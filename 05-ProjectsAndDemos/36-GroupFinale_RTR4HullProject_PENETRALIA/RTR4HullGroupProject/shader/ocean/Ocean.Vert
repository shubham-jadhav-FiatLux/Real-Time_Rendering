in vec4 a_position;
in float a_height;
in vec2 a_texcoord;
uniform float heightScale;
uniform float chopiness;
uniform vec2  size;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
out vec3 eyeSpacePos;
out vec3 worldSpaceNormal;
out vec3 eyeSpaceNormal;
void main(void)
{
float height     = a_height;
vec2  slope= a_texcoord.xy;
vec3 normal= normalize(cross( vec3(0.0, slope.y*heightScale, 2.0 / size.x), vec3(2.0 / size.y, slope.x*heightScale, 0.0)));
worldSpaceNormal = normal;
vec4 pos   = vec4(a_position.x, height * heightScale, a_position.z, 1.0);
gl_Position= ((u_projectionMatrix) * (u_viewMatrix) * (u_modelMatrix)  * pos);
eyeSpacePos= (mat3(u_projectionMatrix * u_viewMatrix * u_modelMatrix)  * pos.xyz);
eyeSpaceNormal   = (transpose(inverse(mat3(u_projectionMatrix * u_viewMatrix *u_modelMatrix)))  * normal).xyz;
}