#include "OfficeScene.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include <iostream>



using namespace std;



OfficeScene::OfficeScene(int sWidth, int sHeight, int samples) {

	screenWidth = sWidth * samples;
	screenHeight = sHeight * samples;
	// Camera settings
	//							  width, heigh, near plane, far plane
	Camera_settings camera_settings{ screenWidth, screenHeight, 0.1, 1000.0 };


	monitorModel = new Model("Resources\\Models\\Monitor\\monitor_reducedPoly.obj");

	deskModel = new Model("Resources\\Models\\Desk\\desk_reducedPoly.obj");
	chairModel = new Model("Resources\\Models\\Chair\\chair_reducedPoly.obj");
	bulbModel = new Model("Resources\\Models\\Bulb\\light.obj");
	computerModel = new Model("Resources\\Models\\Computer\\computer.obj");
	ceilingLampModel = new Model("Resources\\Models\\Ceiling\\ceiling.obj");
	chair2Model = new Model("Resources\\Models\\Chair2\\chair2.obj");
	floorModel = new Model("Resources\\Models\\Floor\\floor.obj");
	// Instanciate the camera object with basic data
	earthCamera = new Camera(camera_settings, glm::vec3(0.0, 0.5, 2.0));

	//
	// Setup textures for rendering the Earth model
	//


	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Phong_shader.vert"),
		string("Resources\\Shaders\\Phong_shader.frag"),
		&phongShader);


	GLSL_ERROR glsl_err1 = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Cubemap_shader.vert"),
		string("Resources\\Shaders\\Cubemap_shader.frag"),
		&cubemapShader);

	// Setup main scene objects
	skybox = new Skybox(
		string("Resources\\Models\\cubemap_snow2\\"),
		string("snow"),
		string(".png")
	);
	// Setup uniform locations for shader

	dayTextureUniform = glGetUniformLocation(phongShader, "texture0");

	modelMatrixLocation = glGetUniformLocation(phongShader, "modelMatrix");

	invTransposeMatrixLocation = glGetUniformLocation(phongShader, "invTransposeModelMatrix");

	viewProjectionMatrixLocation = glGetUniformLocation(phongShader, "viewProjectionMatrix");

	lightDirectionLocation = glGetUniformLocation(phongShader, "dir1.lightDirection");

	lightDiffuseLocation = glGetUniformLocation(phongShader, "dir1.lightDiffuseColour");

	lightSpecularLocation = glGetUniformLocation(phongShader, "dir1.lightSpecularColour");

	lightSpecExpLocation = glGetUniformLocation(phongShader, "dir1.lightSpecularExponent");

	cameraPosLocation = glGetUniformLocation(phongShader, "cameraPos");

	// get unifom locations in shader
	for (int i = 0; i < 2; i++)
	{
		std::stringstream lpositionss;
		lpositionss << "pointLights[" << i << "].position";
		std::stringstream lspecColour;
		lspecColour << "pointLights[" << i << "].lightSpecularColour";
		std::stringstream lspecExp;
		lspecExp << "pointLights[" << i << "].lightSpecularExponent";
		std::stringstream ldiffusess;
		ldiffusess << "pointLights[" << i << "].lightDiffuseColour";
		std::stringstream lattenuationss;
		lattenuationss << "pointLights[" << i << "].attinuation";


		pointlights[i].gPosition = glGetUniformLocation(phongShader, lpositionss.str().c_str());
		pointlights[i].gSpecColour = glGetUniformLocation(phongShader, lspecColour.str().c_str());
		pointlights[i].gSpecExp = glGetUniformLocation(phongShader, lspecExp.str().c_str());
		pointlights[i].gDiffuse = glGetUniformLocation(phongShader, ldiffusess.str().c_str());
		pointlights[i].gAttenuation = glGetUniformLocation(phongShader, lattenuationss.str().c_str());
	}


	glUseProgram(phongShader);
	//Directional Light
	// Set the light direction uniform vector in world coordinates based on the Sun's position
	glUniform4f(lightDirectionLocation, 0.0f, -2.0f, 0.0, 0.0f);
	glUniform4f(lightDiffuseLocation, 0.05f, 0.041f, 0.033f, 0.5f); // white diffuse light
	glUniform4f(lightSpecularLocation, 0.1f, 0.1f, 0.1f, 1.0f); // white specular light
	glUniform1f(lightSpecExpLocation, 8.0f); // specular exponent / falloff

	glUniform4f(pointlights[0].gPosition, 0.0f, 2.5f, 5.0f, 0.0f);
	glUniform3f(pointlights[0].gAttenuation, 1.0f, 0.5f, 0.05f);
	glUniform4f(pointlights[0].gDiffuse, 0.5f, 0.5f, 1.0f, 1.0f);        //Warm white diffuse light
	glUniform4f(pointlights[0].gSpecColour, 0.3f, 0.3f, 0.3f, 1.0f);    //Warm white specular light
	glUniform1f(pointlights[0].gSpecExp, 15.0f);                        // specular exponent / falloff


	glUseProgram(0);


	//
	// Setup FBO (which Earth rendering pass will draw into)
	//

	glGenFramebuffers(1, &demoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);

	//
	// Setup textures that will be drawn into through the FBO
	//

	// Setup colour buffer texture.
	// Note:  The texture is stored as linear RGB values (GL_RGBA8).  
	//There is no need to pass a pointer to image data - 
	//we're going to fill in the image when we render the Earth scene at render time!
	glGenTextures(1, &fboColourTexture);
	glBindTexture(GL_TEXTURE_2D, fboColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Setup depth texture

	glGenTextures(1, &fboDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//
	// Attach textures to the FBO
	//

	// Attach the colour texture object to the framebuffer object's colour attachment point #0
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		fboColourTexture,
		0);

	// Attach the depth texture object to the framebuffer object's depth attachment point
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fboDepthTexture,
		0);


	//
	// Before proceeding make sure FBO can be used for rendering
	//

	GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {

		fboOkay = false;
		cout << "Could not successfully create framebuffer object to render texture!" << endl;

	}
	else {

		fboOkay = true;
		cout << "FBO successfully created" << endl;
	}

	// Unbind FBO for now! (Plug main framebuffer back in as rendering destination)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


OfficeScene::~OfficeScene() {

}


// Accessor methods

Camera* OfficeScene::getOfficeSceneCamera() {

	return earthCamera;
}


GLuint OfficeScene::getOfficeSceneTexture() {

	return fboColourTexture;
}


float OfficeScene::getSunTheta() {

	return sunTheta;
}


void OfficeScene::updateSunTheta(float thetaDelta) {

	sunTheta += thetaDelta;
}


// Scene update
void OfficeScene::update(const float timeDelta) {


}

//Renders the models
void OfficeScene::modelRender(Model* m, glm::mat4 T, glm::mat4 transform) {
	if (m) {
		// Modelling transform
		glm::mat4 modelTransform = transform;
		// Calculate inverse transpose of the modelling transform for correct transformation of normal vectors
		glm::mat4 inverseTranspose = glm::transpose(glm::inverse(modelTransform));;
		glUseProgram(phongShader);

		// Get the location of the camera in world coords and set the corresponding uniform in the shader
		glm::vec3 cameraPos = earthCamera->getCameraPosition();
		glUniform3fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set the model, view and projection matrix uniforms (from the camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelTransform));
		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, glm::value_ptr(inverseTranspose));
		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(T));
		// Activate and Bind the textures to texture units
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, dayTexture);
		//Render the model
		m->draw(phongShader);
		// Restore default OpenGL shaders (Fixed function operations)
		glUseProgram(0);
	}
}
// Rendering methods

void OfficeScene::render() {

	if (!fboOkay)
		return; // Don't render anything if the FBO was not created successfully

				// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);
	// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

	// Clear the screen (i.e. the texture)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to specified texture size (see above)
	glViewport(0, 0, screenWidth, screenHeight);

	// Get view-projection transform as a CGMatrix4
	glm::mat4 T = earthCamera->getProjectionMatrix() * earthCamera->getViewMatrix();

	float frequency = 0.01;
	lightIndex+= 1;
	if (lightIndex > 1280) {
		lightIndex = 0;
	}
	float red = sin(frequency*lightIndex + 0) * 127 + 128;
	float green = cos(frequency*lightIndex + 0) * 127 + 128;
	float blue = sin(frequency*lightIndex + 2) * 127 + 128;

	red = red / 255;
	green = green / 255;
	blue = blue / 255;

	glUseProgram(phongShader);
	glUniform4f(pointlights[1].gPosition, 0.6f, 0.3f, 0.0f, 0.0f);
	glUniform3f(pointlights[1].gAttenuation, 1.0f, 0.5f, 0.05f);
	glUniform4f(pointlights[1].gDiffuse, red, green, blue, 1.0f);        //Warm white diffuse light
	glUniform4f(pointlights[1].gSpecColour, 0.2f, 0.2f, 0.2f, 1.0f);    //Warm white specular light
	glUniform1f(pointlights[1].gSpecExp, 15.0f);                        // specular exponent / falloff
	glUseProgram(0);
	//model Rendering

	//Floor
	modelRender(floorModel, T, glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -2.35f, 3.0f)));

	//Monitor
	glm::mat4 monitorTransform = glm::translate(glm::mat4(1.0), glm::vec3(-0.8f, 0.0f, 0.1f));
	monitorTransform = glm::rotate(monitorTransform, 25.0f * (3.1459f / 180.0f), glm::vec3(0.0, 1.0, 0.0f));
	modelRender(monitorModel, T, monitorTransform);

	//Chair
	modelRender(chairModel, T, glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.0f, 1.2f)));

	//Computer
	glm::mat4 chair2Transform = glm::translate(glm::mat4(1.0), glm::vec3(3.0f, -0.7f, 5.0f));
	chair2Transform = glm::rotate(chair2Transform, 35.0f * (3.1459f / 180.0f), glm::vec3(0.0, 1.0, 0.0f));
	modelRender(chair2Model, T, chair2Transform);

	//Desk
	modelRender(deskModel, T, glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -0.07f, 0.35f)));

	//Computer
	glm::mat4 computerTransform = glm::translate(glm::mat4(1.0), glm::vec3(0.4f, 0.0f, -0.27f));
	computerTransform = glm::rotate(computerTransform, -25.0f * (3.1459f / 180.0f), glm::vec3(0.0, 1.0, 0.0f));
	modelRender(computerModel, T, computerTransform);
	
	//Ceiling lamp
	modelRender(ceilingLampModel, T, glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 3.0f, 5.0f)));

	glFrontFace(GL_CW);
	glm::mat4 skyModelMat = glm::scale(glm::mat4(1.0), glm::vec3(50.0, -50.0, 50.0));
	skyModelMat = glm::translate(skyModelMat, glm::vec3(0.0f, 0.0f, 0.0f));
	skyModelMat = glm::rotate(skyModelMat, 90.0f * (3.1459f / 180.0f), glm::vec3(0.0, 1.0, 0.0f));
	skybox->render(T * skyModelMat);
	glFrontFace(GL_CCW);
	// Set OpenGL to render to the MAIN framebuffer (ie. the screen itself!!)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
