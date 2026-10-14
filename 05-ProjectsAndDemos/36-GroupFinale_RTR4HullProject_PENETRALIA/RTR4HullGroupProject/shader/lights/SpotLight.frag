

in vec3 FragPos; 
in vec3 Normal; 
in vec4 a_color_out;

uniform vec3 u_la;
uniform vec3 u_ld;
uniform vec3 u_ls;
uniform vec4 u_lposition;
uniform vec3 u_ldirection;
uniform vec3 u_lviewpos;
uniform float u_lcutOff;
uniform float u_louterCutOff;
uniform float u_lconstant;
uniform float u_llinear;
uniform float u_lquadratic;
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_materialShininess;
uniform int u_lightingEnabled;

out vec4 FragColor;

void main(void)
{
	if(u_lightingEnabled == 1)
	{
		vec3 lightDir = vec3(normalize(u_lposition.xyz - FragPos));
		float theta = dot(lightDir, normalize(-u_ldirection));

		if(theta > u_lcutOff)
		{
			vec3 ambient = u_la;
			vec3 norm = normalize(Normal);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = u_ld * diff;
			vec3 viewDir = normalize(u_lviewpos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_materialShininess);
			vec3 specular = u_ls * spec;
			float distance= length(u_lposition.xyz - FragPos);
			float attenuation = 1.0 / (u_lconstant + u_llinear* distance + u_lquadratic * (distance * distance));
			diffuse   *= attenuation;
			specular *= attenuation;
			vec3 result = ambient + diffuse + specular;
			FragColor = vec4(result, 1.0);
		}
		else
		{
			FragColor = vec4(u_la,1.0f);
		}
	}
	else
	{
		FragColor = a_color_out;
	}
}
