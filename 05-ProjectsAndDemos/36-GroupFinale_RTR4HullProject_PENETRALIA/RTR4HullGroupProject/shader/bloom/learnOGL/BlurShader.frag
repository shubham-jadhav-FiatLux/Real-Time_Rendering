in vec2 out_a_texcoord;
uniform int u_horizontal;
uniform sampler2D u_image;

out vec4 fragColor;

uniform float weights[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};
void main(void)
{
	vec2 tex_offset = 1.0 / textureSize(u_image, 0);
	vec3 result = texture(u_image, out_a_texcoord).rgb * weights[0];
	if(u_horizontal == 1)
	{
		for(int i = 1 ;i < 5; i++)
		{
			result += texture(u_image, out_a_texcoord + vec2(tex_offset.x *i, 0.0)).rgb * weights[i];
			result += texture(u_image, out_a_texcoord - vec2(tex_offset.x *i, 0.0)).rgb * weights[i];
		}
	}
	else
	{
		for(int i = 1 ;i < 5; i++)
		{
			result += texture(u_image, out_a_texcoord + vec2(0.0, tex_offset.y *i)).rgb * weights[i];
			result += texture(u_image, out_a_texcoord - vec2(0.0, tex_offset.y *i)).rgb * weights[i];
		}
	}
	fragColor = vec4(result, 1.0f);
};