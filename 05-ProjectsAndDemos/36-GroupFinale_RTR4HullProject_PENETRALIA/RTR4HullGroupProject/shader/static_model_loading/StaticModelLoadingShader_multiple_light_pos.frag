
in vec3 out_worldPosition; 
in vec2 out_a_texcoord;
in vec3 out_tangent_light_position; 
in vec3 out_tangent_view_position; 
in vec3 out_tangent_fragment_position;
in vec3 out_transformedNormal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_ao;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;
uniform int u_l_key_pressed; 
uniform float u_material_shininess; 

uniform vec3 u_viewPosition;
uniform vec3 u_lightPosition[4];
uniform vec3 u_lightColor[4];

const float PI = 3.14159265359;

out vec4 fragColor;

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
		for(int i = 0; i < 4; i++)
		{
			vec3 L = normalize(u_lightPosition[i] - out_worldPosition);
			vec3 H = normalize(V+L);
			float distance = length(u_lightPosition[i]-out_worldPosition);
			float attenuation = 1.0 / (distance * distance);
			vec3 radiance = u_lightColor[i] * attenuation;

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
		}

		vec3 F = fresnelSchlickRoughness(max(dot(N,V), 0.0), F0, roughness);

		vec3 kS = F;

		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;

		vec3 ambient = (kD * albedo) * ao;
		vec3 color = ambient + Lo;
				
		color = color / (color + vec3(1.0));
			
		color = pow(color, vec3(1.0/2.2));

    }
    else 
    { 
		vec3 normal_map = texture(texture_normal, out_a_texcoord).rgb; 
        normal_map = normalize(normal_map * 2.0 - 1.0); 
        vec3 texture_color = texture(texture_diffuse, out_a_texcoord).rgb; 

        vec3 ambient = 0.2 * texture_color; 

        vec3 surface_or_light_direction = normalize(out_tangent_light_position - out_tangent_fragment_position); 

        vec3 diffuse = max(dot(surface_or_light_direction, normal_map), 0.0) * texture_color; 

        vec3 view_direction = normalize(out_tangent_view_position - out_tangent_fragment_position); 
        vec3 reflection_vector = reflect(-surface_or_light_direction, normal_map); 
        vec3 halfway_direction = normalize(surface_or_light_direction + view_direction); 
        vec3 specular = vec3(0.2) * pow(max(dot(normal_map, halfway_direction), 0.0), 50.0f); 
        color = ambient + diffuse + specular;
    } 

    fragColor = vec4(color, 1.0);
};