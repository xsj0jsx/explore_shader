#version 330
 in vec3 eyePos;
 in vec3 eyeNormal;
out vec4 outputColor;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
vec3 ADSLightModel(in vec3 eyeNormal, in vec3 eyePosition)
{
	vec4 LightPosition = uModelViewMatrix * vec4(0.,20.,-20., 1.);
	vec3 eyeLightPosition = LightPosition.xyz;
	const vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
	const vec3 lightDiffuse = vec3(1., 1., 1.);
	const vec3 lightSpecular = vec3(1., 1., 1.);
	//
	const vec3 matAmbient = vec3(1., 0.5, 0.);
	const vec3 matDiffuse = vec3(1., 0.5, 0.);
	const vec3 matSpecular = vec3(0.6, 0.6, 0.6);
	const float matShininess = 80;
	//
	vec3 norm =normalize(eyeNormal);
	vec3 lightv = normalize(eyeLightPosition - eyePosition);
	vec3 viewv = normalize(vec3(0.,0.,0.) - eyePosition);
	vec3 reflv = reflect(vec3(0.,0.,0.) -lightv, norm);
	//
	vec3 ambient = matAmbient * lightAmbient;
	vec3 diffuse = max(0.0, dot(lightv, norm)) * matDiffuse * lightDiffuse;
	vec3 specular = vec3(0.0, 0.0, 0.);
	//if(dot(lightv, viewv) > 0.0){
		specular = pow( max(0.0, dot(viewv, reflv)), matShininess) * matSpecular * lightSpecular;
		//}
	return clamp(diffuse + specular, 0.0, 1.0);
}
void main()
{
	outputColor = vec4(ADSLightModel(eyeNormal, eyePos), 1.0f);
}
