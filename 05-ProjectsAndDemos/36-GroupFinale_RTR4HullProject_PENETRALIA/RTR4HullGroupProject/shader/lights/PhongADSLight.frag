
in vec3 transformedNormal_out;
in vec3 lightDirection_out;
in vec3 viewVector_out;
in vec2 a_texcoord_out;

uniform vec3 u_lA;
uniform vec3 u_lD;
uniform vec3 u_lS;
uniform vec3 u_kA;
uniform vec3 u_kD;
uniform vec3 u_kS;
uniform float u_materialShininess;
uniform int u_lightingEnabled;
uniform sampler2D u_textureSampler;
out vec4 FragColor;

void main(void)
{
    vec3 phong_ads_light = vec3(0.0);

    /*texture sampling*/
    vec4 tex = texture(u_textureSampler, a_texcoord_out);
    
    if(u_lightingEnabled == 1)
    {
        vec3 transformedNormal_out_normalize = normalize(transformedNormal_out);
        vec3 lightDirection_out_normalize = normalize(lightDirection_out);
        vec3 viewVector_out_normalize = normalize(viewVector_out);

        vec3 ambient = u_lA * u_kA;

        vec3 diffuse = u_lD * u_kD * max(dot(lightDirection_out_normalize, transformedNormal_out_normalize), 0.0);

        vec3 refectionVector = reflect(-lightDirection_out_normalize, transformedNormal_out_normalize);

        vec3 specular = u_lS * u_kS * pow(max(dot(refectionVector, viewVector_out_normalize), 0.0), u_materialShininess);
        phong_ads_light = ambient + diffuse + specular;
    }
    else
    {
        phong_ads_light = vec3(1.0, 1.0, 1.0);
    }

    FragColor = tex * vec4(phong_ads_light, 1.0);
};