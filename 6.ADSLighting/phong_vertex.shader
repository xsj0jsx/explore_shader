#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out vec3 eyePos;
out vec3 eyeNormal;
void main()
{
	eyeNormal = normalize(vec3( uNormalViewMatrix * vec4(normal, 1.0) ));
	eyePos = vec3(uModelViewMatrix * vec4(position, 1.0));
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1.0);
}
