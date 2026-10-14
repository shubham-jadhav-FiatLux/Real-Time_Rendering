flat in vec4 starColor;

uniform sampler2D u_texture_star;

out vec4 FragColor;

void main(void)
{
	const vec4 color1 = vec4(0.9, 0.9, 1.0, 1.5);
	const vec4 color2 = vec4(0.0, 0.0, 5.0, 0.5);

	vec2 temp = gl_PointCoord - vec2(0.5);
	float f = dot(temp, temp);

	if (f > 0.2)
		discard;

	FragColor = starColor * mix(color1, color2, smoothstep(0.1, 0.1, f)) ;
};