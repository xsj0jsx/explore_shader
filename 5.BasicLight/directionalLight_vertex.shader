#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out vec3 color;
void main()
{
	vec3 vLight = vec3(0,1,-1);
	float NdotL = max(0.0f, dot(normal, vLight));
	color = vec3(0.1,0,0) + vec3(1,0,0) * NdotL;
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1);
}
