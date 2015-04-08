#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out vec3 color;


float SpotAttenuation(in vec3 Pos)
{
	vec3 LightPosition = vec3(0.,1.,-1.);
	
	vec3 FocalPoint = vec3(0,0,0);
	vec3 vLight = normalize(FocalPoint - LightPosition);
	vec3 vPos = normalize(Pos - LightPosition);
	float cosTheta = cos(radians(2.0));//inner
	float cosPhi = cos(radians(10.0));//outer
	float falloff = 1.5;
	
	// compute spot attenuation
	float cosAlpha = dot(vLight, vPos);
	float spotAttenuation = 0.0;
	if(cosAlpha > cosTheta)
		spotAttenuation = 1.0;
	else if(cosAlpha > cosPhi)
		spotAttenuation = pow( (cosAlpha - cosPhi)/(cosTheta - cosPhi),  falloff);
	
	return spotAttenuation;
}
float SpotAttenuationInEyeSpace( in vec3 Pos)
{
	vec3 LightPosition = vec3( uModelViewMatrix * vec4(0.,1.,-1., 1.0) );
	
	vec3 FocalPoint = vec3( uModelViewMatrix * vec4(0,0,0, 1.0) );
	vec3 vLight = normalize(FocalPoint - LightPosition);
	vec3 vPos = normalize(Pos - LightPosition);
	
	float cosTheta = cos(radians(2.0));//inner
	float cosPhi = cos(radians(10.0));//outer
	float falloff = 1.5;
	
	// compute spot attenuation
	float cosAlpha = dot(vLight, vPos);
	float spotAttenuation = 0.0;
	if(cosAlpha > cosTheta)
		spotAttenuation = 1.0;
	else if(cosAlpha > cosPhi)
		spotAttenuation = pow( (cosAlpha - cosPhi)/(cosTheta - cosPhi),  falloff);
	
	return spotAttenuation;
}
vec3 ADSLightModel(in vec3 eyeNormal, in vec3 eyePosition)
{
	vec4 LightPosition = uModelViewMatrix * vec4(0.,1.,-1., 1.);
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
	float atten = SpotAttenuationInEyeSpace(eyePosition);
	return clamp( ambient + (diffuse + specular) * atten, 0.0, 1.0);
}
void main()
{
	vec4 eyePosition = uModelViewMatrix * vec4(position, 1.0) ;
	vec4 eyeNormal = uNormalViewMatrix * vec4(normal, 1.0);
	
	color = ADSLightModel(eyeNormal.xyz, eyePosition.xyz);
	
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1.0);
}
