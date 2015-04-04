#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out Vertex
{
	vec4 normal;
} vertex;
void main()
{
	vertex.normal = vec4(normal,1);
	gl_Position = vec4(position, 1);
}
