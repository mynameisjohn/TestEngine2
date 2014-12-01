#include <GL_Includes.h>

#include <Textures.h>
#include <Menu.h>
#include <Util.h>

#include <glm/gtx/transform.hpp>

Menu::Menu(){
}

Menu::Menu(Drawable * base, uint32_t w, uint32_t h, uint32_t dS)
: FBO(0), tex(0), downSample(dS), drPtr(base){
	//Generate blank texture, add it to drawable
	tex = initTexture(NULL, w/downSample, h/downSample);
	drPtr->addTex("screen", tex);

	//Generate Frame buffer and attach texture
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);	

	//Rebind back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//check for Frame Buffer completeness
bool Menu::isFBOComplete(){
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	return (status == GL_FRAMEBUFFER_COMPLETE);
}

//Handle motion
MenuState Menu::handleEvent(SDL_Event& e){
//	float x(e.motion.x), y(e.motion.y);

	//If the escape key is pressed again, get out of the menu
	if (e.key.keysym.sym == SDLK_ESCAPE && 
		 e.type == SDL_KEYDOWN && 
		 e.key.repeat==0)
		return MENU_RESUME;

	return MENU_CONTINUE;
}

uint32_t Menu::update(){
	//Handle mouse clicks, hovers, etc
	return (uint32_t)isFBOComplete();
}

//Grab screen and blit it into our FBO Texture
bool Menu::grabScreen(uint32_t w, uint32_t h){
	//Bind Read and Draw buffers for Blit
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	//Blit with downsampling for fun
	glBlitFramebuffer(0, 0, w, h, 0, h / downSample, w / downSample, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//Return back buffer into the default slot
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return isFBOComplete();
}

//Render (MAKE A NEW SHADER FOR THIS ASAP)
uint32_t Menu::render(mat4 pInv){
	mat4 MV(pInv*glm::translate(vec3(-1, -1, 0))*glm::scale(vec3(2, 2, 1)));
	vec4 color(1);
	drPtr->uploadMV(MV);
	drPtr->uploadColor(color);
	drPtr->draw("screen");

	MV = pInv*glm::translate(vec3(-.75, -.75, -0.5))*glm::scale(vec3(1.5,1.5,1));
	color = vec4(0.1,0.1,0.1,.93);
	drPtr->uploadMV(MV);
	drPtr->uploadColor(color);
	drPtr->draw();

	return (uint32_t)isFBOComplete();
}
