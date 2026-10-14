in vec2 out_a_texcoord;
uniform sampler2D u_scene_texture;
uniform sampler2D u_bloom_blur_texture;
uniform int u_bloom;
uniform float u_exposure;
out vec4 FragColor;
void main(void)
{
	const float gamma = 2.2;
	vec3 hdr_color = texture(u_scene_texture, out_a_texcoord).rgb;
	vec3 bloom_color = texture(u_bloom_blur_texture, out_a_texcoord).rgb;
	if(u_bloom == 1)
		hdr_color += bloom_color;
	vec3 result = vec3(1.0) - exp(-hdr_color * u_exposure);
	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(hdr_color, 1.0);
};