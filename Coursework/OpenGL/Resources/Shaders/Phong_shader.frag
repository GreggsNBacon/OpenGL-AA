//
// This demo performs lighting in world coordinate space and uses a texture to provide basic diffuse surface properties.
//

#version 330
#define PLIGHTS 2
//
// basic directional light model
//
struct Directional{
 vec4		lightDirection; // direction light comes FROM (specified in World Coordinates)
 vec4		lightDiffuseColour;
 vec4       lightSpecularColour;
 float      lightSpecularExponent;
};
struct PointLight {
	vec4		position;
	vec3		attinuation; //// x=constant, y=linear, z=quadratic
	vec4		lightDiffuseColour;
	vec4        lightSpecularColour;
	float       lightSpecularExponent;
};
uniform PointLight pointLights[PLIGHTS];
uniform Directional dir1;


vec3 CalculateDirectionalLight(Directional dir);
vec3 CalculatePointLight(PointLight light);
uniform vec3        cameraPos; // to calculate specular lighting in world coordinate space, we need the location of the camera since the specular light
    // term is viewer dependent

uniform sampler2D texture0;

//
// input fragment packet (contains interpolated values for the fragment calculated by the rasteriser)
//
in vec4 posWorldCoord;
in vec4 colour;
in vec3 normalWorldCoord;
in vec2 texCoord;

//
// output fragment colour
//
layout (location = 0) out vec4 fragColour;




void main(void) {

	vec3 result = CalculateDirectionalLight(dir1);
	for (int i = 0; i < PLIGHTS; i++)
	{
		result += CalculatePointLight(pointLights[i]);
	}

    //fragColour = vec4(rgbColour, 1.0);
    // Output final gamma corrected colour to framebuffer
    vec3 P = vec3(1.0 / 2.2);
    fragColour = vec4(pow(result, P), 1.0);
}
vec3 CalculateDirectionalLight(Directional dir){
	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(dir.lightDirection);
    
	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(normalWorldCoord);
	
    // calculate lambertian term
    float lambertian = clamp(dot(L.xyz, N), 0.0, 1.0);

    //
	// calculate diffuse light colour
    vec4 texColour = texture(texture0, texCoord);
    vec3 diffuseColour = texColour.rgb * dir.lightDiffuseColour.rgb * lambertian; // input colour actually diffuse colour
    //

    //
    // calculate specular light colour
    //

    // vectors needed for specular light calculation...
    vec3 E = cameraPos - posWorldCoord.xyz; // vector from point on object surface in world coords to camera
    E = normalize(E);
    vec3 R = reflect(-L.xyz, N); // reflected light vector about normal N

    float specularIntensity = pow(max(dot(R, E), 0.0), dir.lightSpecularExponent);
    vec3 specularColour = vec3(1.0f, 1.0f, 1.0f) * dir.lightSpecularColour.rgb * specularIntensity * lambertian;


    //
    // combine colour components to get final pixel / fragment colour
    //
    vec3 rgbColour = diffuseColour + specularColour;

	return rgbColour;
    // Output final gamma corrected colour to framebuffer
    //vec3 P = vec3(1.0 / 2.2);
    //fragColour = vec4(pow(rgbColour, P), 1.0);
}
vec3 CalculatePointLight(PointLight light){

	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(light.position - posWorldCoord);
    
	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(normalWorldCoord);
	
    // lambertian term
    float lambertian = clamp(dot(L.xyz, N), 0.0, 1.0);

	// calculate diffuse light colour
    vec4 texColour = texture(texture0, texCoord);
    vec3 diffuseColour = texColour.rgb * light.lightDiffuseColour.rgb * lambertian; // input colour actually diffuse colour

    // calculate specular light colour
    vec3 E = cameraPos - posWorldCoord.xyz; // vector from point on object surface in world coords to camera
    E = normalize(E);
    vec3 R = reflect(-L.xyz, N); // reflected light vector about normal N

    float specularIntensity = pow(max(dot(R, E), 0.0), light.lightSpecularExponent);
    vec3 specularColour = vec3(1.0f, 1.0f, 1.0f) * light.lightSpecularColour.rgb * specularIntensity * lambertian;

	float d = length(light.position - posWorldCoord);
	float a = 0.0f;
    a = (1.0 / (light.attinuation.x + light.attinuation.y*d + light.attinuation.z*d*d));

    vec3 rgbColour = (diffuseColour * a) + (specularColour * a);

	return rgbColour;
}

