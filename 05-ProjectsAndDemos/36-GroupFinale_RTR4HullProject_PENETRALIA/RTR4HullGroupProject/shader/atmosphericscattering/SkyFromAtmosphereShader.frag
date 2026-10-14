
in vec4 color1;
in vec4 color2;
in vec3 direction;
out vec4 FragColor;

uniform vec3 lightPosition;
uniform float g;
uniform float g2;

const float exposure = 2.0f;

void main(void)
{

	vec4 outColor;
	float fCos = dot(lightPosition, direction) / length(direction);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos * fCos) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
	float fRayleightPhase = 0.75 + 0.75 * fCos * fCos;
					
	FragColor.rgb = color1.rgb * fRayleightPhase + color2.rgb * fMiePhase;
	FragColor.a = FragColor.b;
};