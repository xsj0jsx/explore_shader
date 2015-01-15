
#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out vec3 color;
void main()
{
	color = vec3(0.2,0.2,0.2);
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1);
}

