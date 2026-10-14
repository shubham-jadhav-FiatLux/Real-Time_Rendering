in vec2 out_a_texcoord;
uniform sampler2D u_colorTexture;
out vec4 FragColor;
void main(void)
{
	vec4 bright_color;
	vec3 result = texture(u_colorTexture, out_a_texcoord).rgb;
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 0.9)
		bright_color = vec4(result, 1.0);
	else
		bright_color = vec4(0.0, 0.0, 0.0, 1.0);
	FragColor = bright_color;
};