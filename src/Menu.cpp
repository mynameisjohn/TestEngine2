#include <GL_Includes.h>

#include <Textures.h>
#include <Menu.h>
#include <Util.h>

#include <glm/gtx/transform.hpp>

//THIS IS NONSENSE:
#include <Camera.h>

Button::Button(){

}

Button::Button(vec2 dim)
	: rect(dim) {

}

Button::Button(vec2 pos, vec2 dim)
	: rect(pos, dim) {

}

void Button::moveTo(vec2 pos){
	rect.moveTo(pos);
}

void Button::clearState(){
	state.clear();
}

void Button::addToState(ButtonState bS){
	state.insert(bS);
}

void Button::removeFromState(ButtonState bS){
	state.erase(bS);
}

bool Button::containsState(ButtonState bS){
	return (bool)state.count(bS);
}

BoundRect Button::getRect(){
	return rect;
}

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

	vec2 bDim(0.2, 0.1), bPos;
	emplace_back(bPos - bDim / 2.f, bDim);
}

void Menu::push_back(Button b){
	buttons.push_back(b);
}

void Menu::emplace_back(vec2 pos, vec2 dim){
	buttons.emplace_back(pos, dim);
}

void Menu::clearButtons(){
	vector<Button>::iterator bIt;
	for (bIt = buttons.begin(); bIt != buttons.end(); bIt++)
		bIt->clearState();
}

//check for Frame Buffer completeness
bool Menu::isFBOComplete(){
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	return (status == GL_FRAMEBUFFER_COMPLETE);
}

//Handle motion
MenuState Menu::handleEvent(SDL_Event& e){
	
	ButtonState bS(BUT_MouseOver);
	bool clear(false);
	switch (e.type){
	case SDL_KEYDOWN:
		if (e.key.keysym.sym == SDLK_ESCAPE && e.key.repeat == 0)
			return MENU_RESUME;
	case SDL_MOUSEMOTION:
		bS = BUT_MouseOver;
		break;
	case SDL_MOUSEBUTTONUP:
		clear = true;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT)
			bS = BUT_LeftClicked;
		else if (e.button.button == SDL_BUTTON_RIGHT)
			bS = BUT_RightClicked;
		break;
	default:
		return MENU_CONTINUE;
	}

	//This should be centered
	float x(e.motion.x), y(e.motion.y);
	const vec2 m1(0, 1), m2(1, 0), m3(-1, -1), m4(1, 1);
	vec2 mousePos = remap(vec2(x, y) / SCREEN_DIM, m1, m2, m3, m4);//fix this

	const vec2 mouseSize(.05);
	BoundRect mouseRect(mousePos, mouseSize);

	vector<Button>::iterator bIt;
	for (bIt = buttons.begin(); bIt != buttons.end(); bIt++){
		if (bIt->getRect().collidesWith(mouseRect))
			if (clear)
				bIt->removeFromState(bS);
			else
				bIt->addToState(bS);
		else
			bIt->clearState();
	}

	return MENU_CONTINUE;
}

uint32_t Menu::update(){
	//vector<Button>::iterator bIt;
	//for (bIt = buttons.begin(); bIt != buttons.end(); bIt++){
	//	cout << "Button State: ";
	//	if (bIt->containsState(BUT_LeftClicked))
	//		cout << "Left Clicked\t";
	//	if (bIt->containsState(BUT_RightClicked))
	//		cout << "Right Clicked\t";
	//	if (bIt->containsState(BUT_MouseOver))
	//		cout << "Mouse Over\t";
	//	if (bIt->containsState(BUT_MiddleClicked))
	//		cout << "Middle Clicked\t";
	//	cout << endl;
	//}

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

	MV = pInv*glm::translate(vec3(-.75, -.75, -0.05))*glm::scale(vec3(1.5,1.5,1));
	color = vec4(0.1,0.1,0.1,.93);
	drPtr->uploadMV(MV);
	drPtr->uploadColor(color);
	drPtr->draw();

	vector<Button>::iterator bIt;
	for (bIt = buttons.begin(); bIt != buttons.end(); bIt++){
		BoundRect rect(bIt->getRect());
		MV = pInv*glm::translate(vec3(rect.getPos(),-.071)) * glm::scale(vec3(rect.getDim(),1));
		color = bIt->containsState(BUT_RightClicked) ? vec4(vec3(0.5),1) : bIt->containsState(BUT_LeftClicked) ? vec4(1) : vec4(vec3(.02), 1);
		drPtr->uploadMV(MV);
		drPtr->uploadColor(color);
		drPtr->draw();
		
	}

	return (uint32_t)isFBOComplete();
}
