in vec4 out_clip_space;
in vec2 out_texcoord;
in vec3 out_to_camera_position;
in vec3 out_fromLightVector;

out vec4 FragColor;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform sampler2D dudv_map_texture;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float moveFactor;

const float waveStrength = 0.02;
const float shineDamper = 20.0f;
const float reflectivity = 0.5f;

void main(void)
{
	vec2 ndc = (out_clip_space.xy/out_clip_space.w)/2.0 + 0.5;
    vec2 refraction_texcoord = vec2(ndc.x, ndc.y);
    vec2 reflection_texcoord = vec2(ndc.x, -ndc.y);
		
    float near = 0.1f;
    float far = 10000.0f;
    float depth = texture(depthMap, refraction_texcoord).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

    float waterDepth = floorDistance - waterDistance;

    vec2 distortedTexCoords = texture(dudv_map_texture, vec2(out_texcoord.x + moveFactor, out_texcoord.y)).rg*0.1;
    distortedTexCoords = out_texcoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
    vec2 totalDistortion = (texture(dudv_map_texture, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
        
    refraction_texcoord += totalDistortion;
    refraction_texcoord = clamp(refraction_texcoord, 0.001, 0.999);

    reflection_texcoord += totalDistortion;
    reflection_texcoord.x = clamp(reflection_texcoord.x, 0.001, 0.999);
    reflection_texcoord.y = clamp(reflection_texcoord.y, -0.999, -0.001);

    vec4 reflectionColor = texture(reflection_texture, reflection_texcoord);
    vec4 refractionColor = texture(refraction_texture, refraction_texcoord);

    vec3 viewVector = normalize(out_to_camera_position);
    float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, 5.0);

    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0f - 1.0f, normalMapColor.b, normalMapColor.g * 2.0f - 1.0f);
    normal = normalize(normal);

    vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
    vec3 reflectedLight = reflect(normalize(out_fromLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlights = lightColour * specular * reflectivity;

    FragColor = mix(reflectionColor, refractionColor, 0.1f);
    FragColor = mix(FragColor, vec4(0.2, 0.65, 1.0, 1.0), 0.2) + vec4(specularHighlights, 0.0f);
    FragColor.a = clamp(waterDepth / 10.0f, 0.0f, 1.0f);
};