in vec3 world_coord;
in vec3 eye_coord;
in vec3 out_normal;

layout (std140) uniform DirectionalLight
{
	// (offset must be multiple of base alignment so actual offset -> aligned offset)
	// where,
	//	base alignment of a variable is the number of bytes required for the type of that variable
	//	actual offset of a variable is the sum of aligned offset and the base alignment of earlier variable
	//	aligned offset of a variable is the actual offset of that variable shifted by padding to the next nearest multiple of the base alignment of that variable

										// base alignment  // aligned offset
    vec4 directionalLightDirection;     // 16              // 0		(base alignment (16), actual offset (0) -> aligned offset (0 = 16 * 0))
    vec3 directionalLightAmbient;		// 16              // 16	(base alignment (16), actual offset (16) -> aligned offset (16 = (0 + 16) + 0))
    vec3 directionalLightDiffuse;		// 16              // 32	(base alignment (16), actual offset (32) -> aligned offset (32 = (16 + 16) + 0))
    vec3 directionalLightSpecular;		// 16              // 48	(base alignment (16), actual offset (48) -> aligned offset (48 = (32 + 16) + 0))
};	// layout size = 64 bytes

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

uniform DirLight u_dirLight[2];
uniform PointLight u_pointLight[2];

in vec2 texcoord;

uniform sampler2D u_textureColor;
uniform vec3 u_cameraPosition;
uniform int u_bLight;
uniform sampler2D u_backgroundTexture;
uniform sampler2D u_rTexture;
uniform sampler2D u_gTexture;
uniform sampler2D u_bTexture;
uniform sampler2D u_blendMap;
uniform vec4 fog_color = vec4(0.7, 0.8, 0.9, 0.0);

out vec4 FragColor;

vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{	
	vec3 lightDirection = normalize(vec3(u_lightPosition) - world_coord);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(lightDirection, normal), 0.0);
	vec3 reflectionVector = reflect(-lightDirection,normal);
	vec3 normalized_viewer_vector = viewDir;
	vec3 specular = light.specular *  pow(max(dot(reflectionVector,normalized_viewer_vector),0.0), 50.0);
	float distance = length(vec3(u_lightPosition) - world_coord);
	float attenuation =  (1.0 + 0.07 * distance + 0.0014 * (distance * distance));	
	ambient = ambient / attenuation;
	diffuse = diffuse / attenuation;
	specular = specular / attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirectionalLightNew light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirection = normalize(vec3(-light.direction));

    vec3 ambient = light.ambient * vec3(0.5f, 0.5f, 0.5f);
	vec3 diffuse = light.diffuse * max(dot(lightDirection, normal), 0.0);
	vec3 reflectionVector = reflect(-lightDirection,normal);
	vec3 normalized_viewer_vector = viewDir;
	vec3 specular = light.specular *  pow(max(dot(reflectionVector,normalized_viewer_vector),0.0), 50.0);	
	return (ambient + diffuse + specular);
}


void main(void)
{
	vec4 blendMapColor = texture(u_blendMap, texcoord);
	float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = texcoord * 40.0;
	vec4 backgroundTextureColor = texture(u_backgroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTexture = texture(u_rTexture, tiledCoords) * blendMapColor.r;
	vec4 gTexture = texture(u_gTexture, tiledCoords) * blendMapColor.g;
	vec4 bTexture = texture(u_bTexture, tiledCoords) * blendMapColor.b;
	vec4 totalColor = backgroundTextureColor + rTexture + gTexture + bTexture;
	vec3 phong_ads_light;

	vec3 norm = normalize(out_normal);
	vec3 viewDirection = normalize(u_cameraPosition - world_coord);;

	if(u_bLight == 1)
	{
		vec3 totalDirectionalLight = vec3(0.0f, 0.0f, 0.0f);

		for(int directionalLightIndex = 0; directionalLightIndex < directionalLightCount; directionalLightIndex++)
		{
			totalDirectionalLight = totalDirectionalLight + CalcDirLight(directionalLights[directionalLightIndex],norm, viewDirection);
		}

		/*DirLight tempDirLight;
		tempDirLight.direction = vec4(directionalLightDirection);
		tempDirLight.ambient = vec3(directionalLightAmbient);
		tempDirLight.diffuse = vec3(directionalLightDiffuse);
		tempDirLight.specular = vec3(directionalLightSpecular);

		totalDirectionalLight = totalDirectionalLight + CalcDirLight(tempDirLight,norm, viewDirection);*/

		/*if(directionalLights[1].direction.x == -50.0)
		{
			totalDirectionalLight = vec3(1.0, 0.0, 0.0);
		}
		else
		{
			totalDirectionalLight = vec3(0.0, 0.0, 1.0);
		}*/

		FragColor = (totalColor) * vec4(totalDirectionalLight, 1.0f);
	}
	else
	{
		FragColor = (totalColor);
	}
};


/*
in vec2 texcoord;
in vec3 world_coord;
in vec3 eye_coord;
in vec3 out_normal;

uniform sampler2D u_textureColor;
uniform vec4 u_lightPosition;
uniform int u_bLight;
uniform sampler2D u_backgroundTexture;
uniform sampler2D u_rTexture;
uniform sampler2D u_gTexture;
uniform sampler2D u_bTexture;
uniform sampler2D u_blendMap;
uniform vec4 fog_color = vec4(0.7, 0.8, 0.9, 0.0);
uniform vec4 u_cameraPosition;

struct DirLight {
	vec3 direction;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
/*
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

out vec4 FragColor;

vec4 fog(vec4 c)
{
	float z = length(eye_coord);
	float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - world_coord.y);
	float di = 0.045 * (smoothstep(0.0, 40.0, 20.0 - world_coord.y));
	float extinction = exp(-z * de);
	float inscattering = exp(-z * di);
	return c * extinction + fog_color * (1.0 - inscattering);
}

vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{	
	vec3 lightDirection = normalize(vec3(u_lightPosition) - world_coord);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(lightDirection, normal), 0.0);
	vec3 reflectionVector = reflect(-lightDirection,normal);
	vec3 normalized_viewer_vector = viewDir;
	vec3 specular = light.specular *  pow(max(dot(reflectionVector,normalized_viewer_vector),0.0), 50.0);
	float distance = length(vec3(u_lightPosition) - world_coord);
	float attenuation =  (1.0 + 0.07 * distance + 0.0014 * (distance * distance));	
	ambient = ambient / attenuation;
	diffuse = diffuse / attenuation;
	specular = specular / attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirection = normalize(-light.direction);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(lightDirection, normal), 0.0);
	vec3 reflectionVector = reflect(-lightDirection,normal);
	vec3 normalized_viewer_vector = viewDir;
	vec3 specular = light.specular *  pow(max(dot(reflectionVector,normalized_viewer_vector),0.0), 50.0);	
	return (ambient + diffuse + specular);
}
/*
vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{	
	vec3 lightDirection = normalize(vec3(u_lightPosition) - world_coord);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(lightDirection, normal), 0.0);
	vec3 reflectionVector = reflect(-lightDirection,normal);
	vec3 normalized_viewer_vector = viewDir;
	vec3 specular = light.specular *  pow(max(dot(reflectionVector,normalized_viewer_vector),0.0), 50.0);
	float distance = length(vec3(u_lightPosition) - world_coord);
	float attenuation =  (1.0 + 0.07 * distance + 0.0014 * (distance * distance));	
	ambient = ambient / attenuation;
	diffuse = diffuse / attenuation;
	specular = specular / attenuation;
	return (ambient + diffuse + specular);
}

void main(void)
{
	vec4 blendMapColor = texture(u_blendMap, texcoord);
	float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = texcoord * 40.0;
	vec4 backgroundTextureColor = texture(u_backgroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTexture = texture(u_rTexture, tiledCoords) * blendMapColor.r;
	vec4 gTexture = texture(u_gTexture, tiledCoords) * blendMapColor.g;
	vec4 bTexture = texture(u_bTexture, tiledCoords) * blendMapColor.b;
	vec4 totalColor = backgroundTextureColor + rTexture + gTexture + bTexture;
	
	if(u_bLight == 1)
	{
		vec3 norm = out_normal;
		vec3 viewDirection = normalize(vec3(u_cameraPosition) - world_coord);
		vec3 light_result = CalcDirLight(u_dirLight[0], norm, vec3(viewDirection));
		/*light_result += CalcPointLight(pointLights, norm, world_coord, viewDirection);
		vec4 final_light = vec4((light_result),0.0);
		FragColor = (totalColor) * final_light;
	}
	else
	{
		FragColor = (totalColor);
	}
};
*/
