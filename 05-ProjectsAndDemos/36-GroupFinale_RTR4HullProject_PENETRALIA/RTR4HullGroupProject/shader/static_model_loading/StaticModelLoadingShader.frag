
const int max_point_lights_count = 10;

in vec3 out_worldPosition; 
in vec2 out_a_texcoord;
in vec3 out_tangent_directional_light_direction;
in vec3 out_tangent_point_light_poistion[max_point_lights_count];
in vec3 out_tangent_view_position; 
in vec3 out_tangent_fragment_position;
in vec3 out_transformedNormal;

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

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_ao;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;
uniform int u_l_key_pressed;
uniform int u_pbr_enabled;
uniform float u_material_shininess;
uniform float u_quadratic;
uniform float u_linear;


uniform vec3 u_viewPosition;
uniform vec4 u_lightPosition;
uniform vec3 u_lightColor = vec3(150.0f, 150.0f, 150.0f);

const float PI = 3.14159265359;

out vec4 fragColor;

vec3 CalcDirLight(DirectionalLightNew light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirection = normalize(vec3(-out_tangent_directional_light_direction));

    vec3 ambient = light.ambient * vec3(0.5f, 0.5f, 0.5f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);	
	return (ambient + diffuse + specular);
}

/* WORKING DIRECTIONAL LIGHT
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDirection = normalize(vec3(-out_tangent_directional_light_direction));

    vec3 ambient = light.ambient * vec3(0.5f, 0.5f, 0.5f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);	
	return (ambient + diffuse + specular);
}
*/

vec3 CalPointLight(PointLightNew light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 outTangentPointLightPosition)
{	
	vec3 lightDirection = normalize(outTangentPointLightPosition - out_tangent_fragment_position);

    vec3 ambient = light.ambient * vec3(0.2f, 0.2f, 0.2f);
	vec3 diffuse = light.diffuse * max(dot(normal, lightDirection), 0.0);
	
	vec3 halfway_direction = normalize(lightDirection + viewDir);
	vec3 specular = light.specular *  pow(max(dot(normal ,halfway_direction),0.0), 50.0);
	
	float distance = length(outTangentPointLightPosition - out_tangent_fragment_position);
	float attenuation =  1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient = ambient * attenuation;
	diffuse = diffuse * attenuation;
	specular = specular * attenuation;
	return (ambient + diffuse + specular);
}


vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(texture_normal, out_a_texcoord).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(out_worldPosition);
	vec3 Q2 = dFdy(out_worldPosition);
	vec2 st1 = dFdx(out_a_texcoord);
	vec2 st2 = dFdy(out_a_texcoord);

	vec3 N = normalize(out_transformedNormal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return (normalize(TBN * tangentNormal));
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N,H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom =(NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
			
	return (nom/denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
			
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
			
	return (nom/denom);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return (ggx1 * ggx2);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return( F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0));
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)\
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);\
}

void main(void) 
{ 
    vec3 color;
    if(u_l_key_pressed == 1) 
    {
		if(u_pbr_enabled == 1)
		{
			vec3 albedo = pow(texture(texture_diffuse, out_a_texcoord).rgb, vec3(2.2));
			float metallic = texture(texture_metallic, out_a_texcoord).r;
			float roughness = texture(texture_roughness, out_a_texcoord).r;
			float ao = texture(texture_ao, out_a_texcoord).r;

			vec3 N = getNormalFromMap();
			vec3 V = normalize(u_viewPosition - out_worldPosition);
			vec3 R = reflect(-V, N);

     		vec3 F0 = vec3(0.04);
			F0 = mix(F0, albedo, metallic);

			vec3 Lo = vec3(0.0);
			
			vec3 L = normalize(vec3(u_lightPosition) - out_worldPosition);
			vec3 H = normalize(V+L);
			float distance = length(vec3(u_lightPosition)-out_worldPosition);
			float attenuation = 1.0 / (distance * distance);
			vec3 radiance = u_lightColor * attenuation;

			float NDF = DistributionGGX(N, H, roughness);
			float G = GeometrySmith(N, V, L, roughness);
			vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

			vec3 numerator = NDF * G * F;
			float denominator = 4.0 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0) + 0.0001;
			vec3 specular = numerator / denominator;

			vec3 kS = F;

			vec3 kD = vec3(1.0) - kS;

			kD *= 1.0 - metallic;
				
			float NdotL = max(dot(N,L), 0.0);
			Lo += (kD * albedo / PI + specular) * radiance * NdotL;
			

			vec3 FF = fresnelSchlickRoughness(max(dot(N,V), 0.0), F0, roughness);

			vec3 kkS = FF;

			vec3 kkD = 1.0 - kkS;
			kkD *= 1.0 - metallic;

			vec3 ambient = (kkD * albedo);
			color = ambient + Lo;
				
			/*color = color / (color + vec3(1.0));*/
			
			/*color = pow(color, vec3(1.0/2.2));*/
		}
		else 
		{
			float constant = 1.0f;
			vec3 normal_map = texture(texture_normal, out_a_texcoord).rgb; 
			normal_map = normalize(normal_map * 2.0 - 1.0); 
			vec3 texture_color = texture(texture_diffuse, out_a_texcoord).rgb; 

			vec3 view_direction = normalize(out_tangent_view_position - out_tangent_fragment_position); 

			vec3 totalDirectionalLight = vec3(0.0f, 0.0f, 0.0f);

			for(int directionalLightIndex = 0; directionalLightIndex < directionalLightCount; directionalLightIndex++)
			{
				totalDirectionalLight = totalDirectionalLight + CalcDirLight(directionalLights[directionalLightIndex], normal_map, view_direction);
			}

			color = totalDirectionalLight;
			// color = CalcDirLight(u_dirLight[0],normal_map, view_direction);

			for(int pointLightIndex = 0; pointLightIndex < pointLightCount; pointLightIndex++)
			{
				color += CalPointLight(pointLights[pointLightIndex], normal_map, out_tangent_fragment_position, view_direction, out_tangent_point_light_poistion[pointLightIndex]);
			}
			color *= texture_color;
		}
    }
    else
	{
		vec3 texture_color = texture(texture_diffuse, out_a_texcoord).rgb;
		color = texture_color;
	}

    fragColor = vec4(color, 1.0);
};
