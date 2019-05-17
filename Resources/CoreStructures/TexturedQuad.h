#ifndef TEXTURED_QUAD_H
#define TEXTURED_QUAD_H

#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

struct TextureGenProperties;

// Model a simple textured quad oriented to face along the +z axis (so the textured quad faces the viewer in (right-handed) eye coordinate space.  The quad is modelled using VBOs and VAOs and rendered using the basic texture shader in Resources\Shaders\basic_texture.vs and Resources\Shaders\basic_texture.fs

class TexturedQuad{

private:

	GLuint					quadVertexArrayObj;

	GLuint					quadVertexBuffer;
	GLuint					quadTextureCoordBuffer;

	GLuint					quadShader;

	GLuint					texture;

	//
	// Private API
	//

	GLuint screenWidth, screenHeight, samples;



	void loadShader(bool SSAAEnabled = false);
	void setupVAO(bool invertV);


	//
	// Public API
	//

public:

	TexturedQuad(const std::string& texturePath, bool invertV = false);
	TexturedQuad(GLuint initTexture, bool invertV = false, bool SSAAEnabled = false, int sWidth = 0, int sHeight = 0, int sSamples = 0);
	TexturedQuad(const std::string& texturePath, const TextureGenProperties& textureProperties, bool invertV = false);

	~TexturedQuad();

	void render();
};

#endif