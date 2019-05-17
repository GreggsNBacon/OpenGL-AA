#version 330

uniform sampler2D texture0;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

uniform int screenWidth;
uniform int screenHeight;
uniform int samples;


void main(void) {
	
	vec4 outColour = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 newColour;
	float tx = 1.0 / (screenWidth * samples);
    float ty = 1.0 / (screenHeight * samples);

	int size = samples / 2;

	int sample2 = ((samples + 1) * (samples+1));

	for (int y = -size; y <= size; y++) {
        for (int x = -size; x <= size; x++) {
			vec2 position = (texCoord + vec2(x, y)) + vec2(tx * x, ty * y);
			newColour = (texture(texture0, position) / sample2);

			outColour += newColour;
		}
	}
	fragColour = outColour;
}

