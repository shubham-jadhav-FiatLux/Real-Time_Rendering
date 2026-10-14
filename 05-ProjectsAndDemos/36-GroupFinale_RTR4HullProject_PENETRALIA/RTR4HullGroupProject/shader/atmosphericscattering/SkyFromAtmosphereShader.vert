
in vec3 a_position;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec4 inWaveLength;

uniform float cameraHeight;
uniform float cameraHeight2;
uniform float outerRadius;
uniform float outerRadius2;
uniform float innerRadius;
uniform float innerRadius2;

uniform float krESun;
uniform float kmESun;
uniform float kr4PI;
uniform float km4PI;
uniform float scale;
uniform float scaleDepth;
uniform float scaleOverScaleDepth;

out vec4 color1;
out vec4 color2;
out vec3 direction;

const int nSamples = 2;
const float fSamples = 2.0f;

float scale_func(float fCos)
{
	float x = 1.0f - fCos;
	return (scaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))))); 
}

void main(void)
{
	vec3 v3Ray = (a_position - cameraPosition);
	float fFar = length(v3Ray);
	v3Ray = v3Ray / fFar;
			
	vec3 v3Start = cameraPosition;
	float fHeight = length(v3Start);
	float fStartDepth = exp(scaleOverScaleDepth * (innerRadius - cameraHeight));
	float fStartAngle = dot(v3Ray, v3Start) / fHeight;
	float fStartOffset = fStartDepth * scale_func(fStartAngle);

	float fSampleLength = fFar / fSamples;
	float fScaledLength = fSampleLength * scale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;

	vec4 v3FrontColor = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < nSamples; i++)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(scaleOverScaleDepth * (innerRadius - fHeight));
		float fLightAngle = dot(normalize(lightPosition), v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth * (scale_func(fLightAngle) - scale_func(fCameraAngle)));
		vec4 v3Attenuate = exp(-fScatter * (inWaveLength * kr4PI + km4PI));
		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	color1 =  v3FrontColor * (inWaveLength * krESun);
	color2 =  v3FrontColor * (kmESun);
	direction = cameraPosition - a_position;
	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position, 1.0);
};