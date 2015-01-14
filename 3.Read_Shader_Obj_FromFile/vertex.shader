#version 330
layout(location = 0) in vec3 position;
uniform mat4 uModelViewProjectionMatrix;
void main()
{
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1);
}
