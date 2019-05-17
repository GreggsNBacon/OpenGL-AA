#version 330

uniform mat4 modelMatrix;
uniform mat4 invTransposeModelMatrix;
uniform mat4 modelViewProjectionMatrix;

uniform vec3 cameraWorldPos; // camera pos in world coords

layout (location=0) in vec4 vertexPos;
layout (location=2) in vec3 vertexNormal;

out vec3 eyeVec, normal;

void main(void)
{
	vec4 vertexWorldCoord = modelMatrix * vertexPos;
	vec3 N = normalize(vertexNormal);

	eyeVec = vertexWorldCoord.xyz - cameraWorldPos;
	normal = (invTransposeModelMatrix * vec4(N, 0.0)).xyz;

	gl_Position = modelViewProjectionMatrix * vertexPos;
}
