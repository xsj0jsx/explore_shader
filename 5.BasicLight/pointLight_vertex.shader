#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out vec3 color;
void main()
{
	vec3 lightInWorld = vec3(0,1,1);
	vec3 posInWorld = position;
	vec3 vLight = normalize(lightInWorld - posInWorld);
	float distance = distance(lightInWorld, posInWorld);
	float a = 0.5;
	float b = 0.1;
	float c = 1.0;
	float attenuation = 1.0/(a * distance * distance + b * distance + c);	
	float NdotL = max(0.0f, dot(normal, vLight));
	color = vec3(0.1,0,0) + vec3(0,0,1) * NdotL * attenuation;
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1);
}
