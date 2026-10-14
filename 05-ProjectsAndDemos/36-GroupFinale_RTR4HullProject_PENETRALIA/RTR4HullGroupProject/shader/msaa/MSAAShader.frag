
in vec2 a_texcoord_out;
out vec4 FragColor;
uniform sampler2D u_screenTextureSampler;

void main(void)
{
    vec3 color = texture(u_screenTextureSampler, a_texcoord_out).rgb;
    /*float grayscale = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;*/
    FragColor = vec4(vec3(color), 1.0);
}
