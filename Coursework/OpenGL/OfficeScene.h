#ifndef OFFICE_SCENE_H
#define OFFICE_SCENE_H

#include "Sphere.h"
#include "Camera.h"
#include "Includes.h"
#include <sstream>

class Model;
class Skybox;


class OfficeScene {

private:
	float lightIndex = 0;
	Model *monitorModel;
	Model *deskModel;
	Model *chairModel;
	Model *computerModel;
	Model *bulbModel;
	Model *ceilingLampModel;
	Model *floorModel;
	Model *chair2Model;

	// Move around the earth with a seperate camera to the main scene camera
	Camera						*earthCamera;

	// Textures for multi-texturing the earth model
	GLuint							dayTexture;

	// Shader for multi-texturing the earth
	GLuint							phongShader;


	// Unifom locations for earthShader

	// Texture uniforms
	GLuint							dayTextureUniform;
	GLuint							nightTextureUniform;
	GLuint							maskTextureUniform;

	// Camera uniforms
	GLint							modelMatrixLocation;
	GLint							invTransposeMatrixLocation;
	GLint							viewProjectionMatrixLocation;

	// Directional light uniforms
	GLint							lightDirectionLocation;
	GLint							lightDiffuseLocation;
	GLint							lightSpecularLocation;
	GLint							lightSpecExpLocation;
	GLint							cameraPosLocation;

	GLuint	 cubemapShader;

	Skybox *skybox;

	struct lightVariables {
		GLuint gDiffuse;
		GLuint gSpecColour;
		GLuint gSpecExp;
		GLuint gAttenuation;
		GLuint gPosition;
	};

	lightVariables pointlights[2];
	//
	// Animation state
	//
	float							sunTheta; // Angle to the Sun in the orbital plane of the Earth (the xz plane in the demo)
	float							earthTheta;


	//
	// Framebuffer Object (FBO) variables
	//

	// Actual FBO
	GLuint							demoFBO;


	// Colour texture to render into
	GLuint							fboColourTexture;

	// Depth texture to render into
	GLuint							fboDepthTexture;

	// Flag to indicate that the FBO is valid
	bool							fboOkay;

	int screenWidth, screenHeight;
public:

	OfficeScene(int sWidth, int sHeight, int samples);
	~OfficeScene();

	// Accessor methods
	Camera* getOfficeSceneCamera();
	GLuint getOfficeSceneTexture();
	float getSunTheta();
	void updateSunTheta(float thetaDelta);
	void modelRender(Model* m, glm::mat4 T, glm::mat4 transform);
	// Scene update
	void update(const float timeDelta);

	// Rendering methods
	void render();
};

#endif