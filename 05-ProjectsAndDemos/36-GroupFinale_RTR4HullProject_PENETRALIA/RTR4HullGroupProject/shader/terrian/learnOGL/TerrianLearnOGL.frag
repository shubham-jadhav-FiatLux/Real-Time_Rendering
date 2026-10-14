in float out_height;
in vec2 out_texcoord;
in vec3 world_coord;
in vec3 out_normal;
out vec4 FragColor;

uniform sampler2D u_textureColor;

struct DirectionalLightNew
{
	vec4 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

const int max_directional_lights_count = 10;
const int max_point_lights_count = 10;

layout (std140) uniform Lights
{
																			// base alignment	// aligned offset
	DirectionalLightNew directionalLights[max_directional_lights_count];	// 16				// 0		(base alignment (16), actual offset (0) -> aligned offset (0 = 16 * 0))
	int directionalLightCount;												// 4				// 640		(base alignment (4), actual offset (640) -> aligned offset (640 = (0 + 640) + 0))
};	// layout size = 644 bytes

struct PointLightNew {
						
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

layout (std140) uniform PointLights
{
																			// base alignment	// aligned offset
	PointLightNew pointLights[max_point_lights_count];						// 16				// 0		(base alignment (16), actual offset (0) -> aligned offset (0 = 16 * 0))
	int pointLightCount;													// 4				// 800		(base alignment (4), actual offset (800) -> aligned offset (800 = (0 + 800) + 0))
};	// layout size = 804 bytes


uniform vec3 u_cameraPosition;

uniform int u_bLight; 

vec3 CalcDirLight(DirectionalLightNew light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirection = normalize(vec3(-light.direction));

    vec3 ambient = light.ambient * vec3(0.5f, 0.5f, 0.5f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);	
	return (ambient + diffuse + specular);
}

vec3 CalPointLight(PointLightNew light, vec3 normal, vec3 fragPos, vec3 viewDir)
{	
	vec3 lightDirection = normalize(light.position - world_coord);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);
	
	float distance = length(light.position - fragPos);
	float attenuation =  1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient = ambient * attenuation;
	diffuse = diffuse * attenuation;
	specular = specular * attenuation;
	return (ambient + diffuse + specular);
}

void main(void)
{
	vec4 texture = texture(u_textureColor, out_texcoord);
	vec3 color;

	vec3 view_direction = normalize(u_cameraPosition - world_coord);
	vec3 norm_out_normal = normalize(out_normal);
	if(u_bLight == 1) 
    {
		color = CalcDirLight(directionalLights[0], norm_out_normal, view_direction);

		for(int pointLightIndex = 0; pointLightIndex < pointLightCount; pointLightIndex++)
		{
			color +=  CalPointLight(pointLights[pointLightIndex], norm_out_normal, world_coord, view_direction);
		}
		
		color *= vec3(texture);

	}
	else
	{
		color = vec3(texture);
	}
	FragColor = vec4(color, 1.0);
};

/*

in float out_height;
in vec2 out_texcoord;
in vec3 world_coord;
in vec3 out_normal;
out vec4 FragColor;

uniform sampler2D u_textureColor;

struct DirLight {
	vec3 direction;	
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

uniform vec3 u_cameraPosition;
uniform DirLight u_dirLight[1];
uniform PointLight u_pointLight[5];

uniform int u_bLight; 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirection = normalize(vec3(-light.direction));

    vec3 ambient = light.ambient * vec3(0.5f, 0.5f, 0.5f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);	
	return (ambient + diffuse + specular);
}

vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{	
	vec3 lightDirection = normalize(light.position - world_coord);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);
	
	float distance = length(light.position - fragPos);
	float attenuation =  1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient = ambient * attenuation;
	diffuse = diffuse * attenuation;
	specular = specular * attenuation;
	return (ambient + diffuse + specular);
}

void main(void)
{
	vec4 texture = texture(u_textureColor, out_texcoord);
	vec3 color;

	vec3 view_direction = normalize(u_cameraPosition - world_coord);
	vec3 norm_out_normal = normalize(out_normal);
	if(u_bLight == 1) 
    {
		color = CalcDirLight(u_dirLight[0], norm_out_normal, view_direction);
		for(int i = 0 ;i < 5; i++)
		{
			color +=  CalPointLight(u_pointLight[i], norm_out_normal, world_coord, view_direction);
		}
		
		color *= vec3(texture);

	}
	else
	{
		color = vec3(texture);
	}
	FragColor = vec4(color, 1.0);
};


*/