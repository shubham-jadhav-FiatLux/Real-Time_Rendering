in vec3 eyeSpacePos;
in vec3 worldSpaceNormal;
in vec3 eyeSpaceNormal;
uniform vec4 deepColor;
uniform vec4 shallowColor;
uniform vec4 skyColor;
uniform vec3 lightDir;
out vec4 FragColor;
void main(void)
{
vec3 eyeVector= normalize(eyeSpacePos);
vec3 eyeSpaceNormalVector   = normalize(eyeSpaceNormal);
vec3 worldSpaceNormalVector = normalize(worldSpaceNormal);
float facing    = max(0.0, dot(eyeSpaceNormalVector, -eyeVector));
float fresnel   = pow(1.0 - facing, 5.0);
float diffuse   = max(0.0, dot(worldSpaceNormalVector, lightDir));
vec4 waterColor = mix(shallowColor, deepColor, facing);
FragColor = waterColor*diffuse + skyColor*fresnel;
}