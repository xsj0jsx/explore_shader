#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalViewMatrix;
out vec3 color;
void main()
{
	vec3 lightInWorld = vec3(1,1,1);
	vec3 posInWorld = position;
	vec3 vLight = normalize(lightInWorld - posInWorld);
	float distance = distance(lightInWorld, posInWorld);
	//light property
	float lightRange = 10;
	vec3 vLightDirection = vec3(-1,-1,-1);
	float cosTheta = cos(radians(5.0));//inner
	float cosPhi = cos(radians(30.0));//outer
	float falloff = 1.5;
	// attenuation based on distance, for demonstration
	// purposes this is simply linear rather than the inverse quadratic.
    // This allows for the spot-light attenuation to be more apparent.
	float linearAttenuation = mix(1.0,0.0, distance/lightRange);
	// compute spot attenuation
	float cosAlpha = max(0.0, dot(vLight, -vLightDirection));
	float spotAttenuation = 0.0;
	if(cosAlpha > cosTheta)
		spotAttenuation = 1.0;
	else if(cosAlpha > cosPhi)
		spotAttenuation = pow( (cosAlpha - cosPhi)/(cosTheta - cosPhi),  falloff);
	float attenuation = linearAttenuation * spotAttenuation;
	float NdotL = max(0.0f, dot(normal, vLight));
	color = vec3(0.1,0,0) + vec3(0,1,1) * NdotL * attenuation;
	gl_Position = uModelViewProjectionMatrix * vec4(position, 1);
}
