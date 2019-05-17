#version 330

uniform samplerCube cubemap;

in vec3 eyeVec, normal;

layout (location = 0) out vec4 fragColour;

void main(void)
{
	vec3 E = normalize(eyeVec);
	vec3 N = normalize(normal);

	vec3 R = reflect(E, N); // R is the reflected vector
	
	R = normalize(R);
		
	fragColour = texture(cubemap, R);
}
