
in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord;
in vec3 a_tangent;

struct DirectionalLightNew
{
	vec4 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

const int max_directional_lights_count = 10;

layout (std140) uniform Lights
{
																			// base alignment	// aligned offset
	DirectionalLightNew directionalLights[max_directional_lights_count];	// 16				// 0		(base alignment (16), actual offset (0) -> aligned offset (0 = 16 * 0))
	int directionalLightCount;												// 4				// 640		(base alignment (4), actual offset (640) -> aligned offset (640 = (0 + 640) + 0))
};	// layout size = 644 bytes

struct PointLightNew {
						// base alignment
    vec3 position;		// 16
    vec3 ambient;		// 16
    vec3 diffuse;		// 16
    vec3 specular;		// 16
    float constant;		// 4
    float linear;		// 4
    float quadratic;	// 4
};	// actual struct size = 76, size for layout calculation = 76 + 4 (padding) = 80 = 16 * 5

const int max_point_lights_count = 10;

layout (std140) uniform PointLights
{
																			// base alignment	// aligned offset
	PointLightNew pointLights[max_point_lights_count];						// 16				// 0		(base alignment (16), actual offset (0) -> aligned offset (0 = 16 * 0))
	int pointLightCount;													// 4				// 800		(base alignment (4), actual offset (800) -> aligned offset (800 = (0 + 800) + 0))
};	// layout size = 804 bytes

struct DirLight {
	vec4 direction;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;    
    float constant;
    float linear;
    float quadratic;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight u_dirLight[1];
uniform PointLight u_pointLight[1];

#define MAX_COUNT 50

uniform mat4 u_modelMatrix[MAX_COUNT];
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform vec4 u_lightPosition;
uniform vec3 u_viewPosition;
out vec3 out_worldPosition;
out vec2 out_a_texcoord;
out vec3 out_tangent_directional_light_direction;
out vec3 out_tangent_point_light_poistion[max_point_lights_count];
out vec3 out_tangent_view_position;
out vec3 out_tangent_fragment_position;
out vec3 out_transformedNormal;
void main(void)
{
	out_worldPosition = vec3(u_modelMatrix[gl_InstanceID] * a_position);
	out_a_texcoord = a_texcoord;
	
	mat3 normal_matrix = transpose(inverse(mat3(u_modelMatrix[gl_InstanceID])));
	vec3 T = normalize(normal_matrix * a_tangent);
	vec3 N = normalize(normal_matrix * a_normal);
	T = normalize( T - dot(T, N) * N);
	vec3 B = cross(N,T);
	mat3 TBN = transpose(mat3(T, B, N));
	out_transformedNormal= N;
	out_tangent_directional_light_direction = TBN * vec3(directionalLights[0].direction);
	for(int pointLightIndex = 0; pointLightIndex < pointLightCount; pointLightIndex++)
	{
		out_tangent_point_light_poistion[pointLightIndex] = TBN * vec3(pointLights[pointLightIndex].position);
	}
	out_tangent_view_position = TBN * u_viewPosition;
	out_tangent_fragment_position = TBN * out_worldPosition;
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix[gl_InstanceID] * a_position;
}
