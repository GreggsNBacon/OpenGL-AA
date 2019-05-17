#include "TexturedQuad.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"

using namespace std;

// Geometry data for textured quad (this is rendered directly as a triangle strip)

static float quadPositionArray[] = {

	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	1.0f,  1.0f, 0.0f, 1.0f
};


static float quadTextureCoordArray[] = {

	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f
};


static float quadTextureCoordArray_v_inverted[] = {

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
};



//
// Private API
//

void TexturedQuad::loadShader(bool SSAAEnabled) {

	// setup shader for textured quad
	if (SSAAEnabled) {
		GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(string("Resources\\Shaders\\ssaa.vert"), string("Resources\\Shaders\\ssaa.frag"), &quadShader);
	}
	else {
		GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(string("..\\..\\Resources\\CoreStructures\\Shaders\\basic_texture.vs"), string("..\\..\\Resources\\CoreStructures\\Shaders\\basic_texture.fs"), &quadShader);
	}

	GLuint screenWidthLocation = glGetUniformLocation(quadShader, "screenWidth");
	GLuint screenHeightLocation= glGetUniformLocation(quadShader, "screenHeight");
	GLuint samplesLocation = glGetUniformLocation(quadShader, "samples");
	glUseProgram(quadShader);
	glUniform1i(screenWidthLocation, screenWidth);
	glUniform1i(screenHeightLocation, screenHeight);
	glUniform1i(samplesLocation, samples);
	glUseProgram(0);

}


void TexturedQuad::setupVAO(bool invertV) {

	// setup VAO for textured quad object
	glGenVertexArrays(1, &quadVertexArrayObj);
	glBindVertexArray(quadVertexArrayObj);



	// setup vbo for position attribute
	glGenBuffers(1, &quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositionArray), quadPositionArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);




	// setup vbo for texture coord attribute
	glGenBuffers(1, &quadTextureCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadTextureCoordBuffer);



	if (invertV) {

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordArray), quadTextureCoordArray_v_inverted, GL_STATIC_DRAW);
	}
	else {

		glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordArray), quadTextureCoordArray, GL_STATIC_DRAW);
	}

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	// enable vertex buffers for textured quad rendering (vertex positions and colour buffers)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	// unbind textured quad VAO
	glBindVertexArray(0);
}


TexturedQuad::TexturedQuad(const string& texturePath, bool invertV)
{


	loadShader();
	setupVAO(invertV);

	// Load texture
	texture = TextureLoader::loadTexture(texturePath, TextureGenProperties(GL_SRGB8_ALPHA8, GL_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true));
}


TexturedQuad::TexturedQuad(GLuint initTexture, bool invertV, bool SSAAEnabled, int sWidth, int sHeight, int sSamples)
{
	if (SSAAEnabled) {
		screenWidth = sWidth;
		screenHeight = sHeight;
		samples = sSamples;
	}
	loadShader(SSAAEnabled);
	setupVAO(invertV);

	texture = initTexture;
}

TexturedQuad::TexturedQuad(const string& texturePath, const TextureGenProperties& textureProperties, bool invertV) 
{
	loadShader();
	setupVAO(invertV);

	// Load texture
	texture = TextureLoader::loadTexture(texturePath, textureProperties);
}


TexturedQuad::~TexturedQuad() {

	// unbind textured quad VAO
	glBindVertexArray(0);

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &quadVertexBuffer);
	glDeleteBuffers(1, &quadTextureCoordBuffer);

	glDeleteVertexArrays(1, &quadVertexArrayObj);

	glDeleteShader(quadShader);
}


void TexturedQuad::render() {

	// use identity matrix(or translation/scaling matrix) for mvpMatrix aligned quad
	glm::mat4 T = glm::mat4(1.0);

	static GLint mvpLocation = glGetUniformLocation(quadShader, "mvpMatrix");

	glUseProgram(quadShader);

	// don’t need camera matrices as quad will be axis aligned
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(T));

	glActiveTexture(GL_TEXTURE0);

	// bind fboColourTexture
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(quadVertexArrayObj);

	// draw quad directly - no indexing needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// unbind VAO for textured quad
	glBindVertexArray(0);

}


