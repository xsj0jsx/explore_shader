#version 330
layout(triangles) in;
layout(line_strip, max_vertices=6) out;
uniform mat4 uModelViewProjectionMatrix;
in Vertex
{
	vec4 normal;
}vertex[];
out vec3 color;
void main()
{
	int i;
#define UseNormalData
#ifdef UseNormalData
	for(i = 0; i < gl_in.length(); ++i)
	{
		vec3 P = gl_in[i].gl_Position.xyz;
		vec3 N = vertex[i].normal.xyz;
		gl_Position = uModelViewProjectionMatrix * vec4(P, 1.0);
		color = vec3(1,0,0);
		EmitVertex();
		gl_Position = uModelViewProjectionMatrix * vec4(P + N*0.1, 1.0);
		color = vec3(0,1,0);
		EmitVertex();
		EndPrimitive();
	}
#else	// compute normal
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;

	vec3 v0 = p0 - p1;
	vec3 v1 = p2 - p1;
	vec3 n = cross(v1, v0);
	n = normalize(n);
	vec3 center = (p0 + p1 + p2)/3.0;
	gl_Position = uModelViewProjectionMatrix * vec4(center, 1.0);
	color = vec3(0,1,0);
	EmitVertex();
	gl_Position = uModelViewProjectionMatrix * vec4(center + n * 0.2 , 1.0);
	color = vec3(1,0,0);
	EmitVertex();
	EndPrimitive();
#endif
}
