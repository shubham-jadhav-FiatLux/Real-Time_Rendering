

in vec2 out_texcoords;
out vec4 fragColor;
uniform sampler2D text;
uniform vec3 textColor;
void main(void)
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, out_texcoords).r);
	fragColor = vec4(textColor, 1.0) * sampled;
};