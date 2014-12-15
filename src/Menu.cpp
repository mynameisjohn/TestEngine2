#include <GL_Includes.h>

#include <Textures.h>
#include <Menu.h>
#include <Util.h>

#include <glm/gtx/transform.hpp>

//Max number of Panes per menu / Controls per Pane
const int MENU_MAX_PANES(5), PANE_MAX_CONTROLS(5);

Control::Control()
 : drPtr(nullptr) { }

Control::Control(string l, BoundRect r, Drawable * d)
 : label(l), colRect(r), drPtr(d) { }

Slider::Slider()
 : Control(), handle(nullptr) { }

Slider::Slider(string l, BoundRect r, Drawable * d, float v, float m, float M, float * h)
 : Control(l, r, d), value(v), minValue(m), maxValue(M), handle(h) { }

Switch::Switch()
 : Control(), state(0), handle(nullptr) { }

Switch::Switch(string l, BoundRect r, Drawable * d, vector<string> o, uint32_t s, uint32_t * h)
 : Control(l, r, d), options(o), state(s), handle(h) { }

void Switch::draw(){

}

void Slider::draw(){

}

//Main constructor
Pane::Pane(string l)
   :  m_Label(l) 
{}

//Draw all of the pane's controls
void Pane::draw(){
   vector<unique_ptr<Control> >::iterator cIt;
   for (cIt=m_Controls.begin(); cIt!=m_Controls.end(); cIt++)
      (*cIt)->draw();
}

bool Slider::handleEvent(SDL_Event * e){
   //if event is mousedown, start tracking horizontal movement (make note of x pos, I guess)
   //if mouseUp, and if the mouse was already down (store this as toggle), reassign value
   //*handle = lerp(value, minValue, maxValue); //implying value [0,1]
	return true;
}

bool Switch::handleEvent(SDL_Event * e){
   //check collision with all the switches (divide up the dim by the number of options, make quads)
   //if one was mouse'd down and is mouse'd (') up again, make that the current state
   //state = idxOf(options, selected);
   //*handle should probably be an enum (or passed via constructor) //map<uint32_t, string>;
	return true;
}

//Function for adding controls; I need unique ones for now
bool Pane::addControl(Control& c){
	if (m_Controls.size() > PANE_MAX_CONTROLS)
      return false;
	if (Slider * s = dynamic_cast<Slider *>(&c)){
		m_Controls.emplace_back(new Slider(*s));
		return true;
	}
	if (Switch * s = dynamic_cast<Switch *>(&c)){
		m_Controls.emplace_back(new Switch(*s));
		return true;
	}
	return false;
}

//Return string label of Pane
string Pane::getLabel(){
   return m_Label;
}

bool Pane::handleEvent(SDL_Event * e){
   //Should I check if the mouse collides with a control?
   bool update(false);
   vector<unique_ptr<Control> >::iterator cIt;
   for (cIt=m_Controls.begin(); cIt!=m_Controls.end(); cIt++)
      if ((*cIt)->handleEvent(e))
         update = true;
   return update;
}

//Main constructor
Menu::Menu(BoundRect r, float pH, Drawable * d)
      :  m_Rect(r), 
			m_PaneHeight(pH), 
			qPtr(d), 
			m_FBO(0), 
			m_Tex(0),
			m_ActivePane("")
{}

//Draw the menu and its components
void Menu::draw(){
   //Scale, translation, active/inactive colors, model view matrix
   vec3 S, T;
   vec4 activeColor(1), inactiveColor(vec3(0.5),1);
   mat4 MV;

	//Draw screen as background
	MV = glm::translate(vec3(-1,-1,0))*glm::scale(vec3(2,2,1));
	qPtr->uploadMV(MV);
	qPtr->uploadColor(activeColor);
	qPtr->draw("screen");

	return;

   //Draw base quad
   S = vec3(m_Rect.getDim() - vec2(0, -m_PaneHeight), 1);
   T = vec3(m_Rect.getPos() - vec2(0, -m_PaneHeight), 1);
   MV = glm::translate(T)*glm::scale(S);
   qPtr->uploadMV(MV);
   qPtr->uploadColor(activeColor);
   qPtr->draw();

   //Draw Pane tabs
   S = vec3(m_Rect.getDim().x/((float)m_Panes.size()), m_PaneHeight, 1);
	float shift(0);
	unordered_map<string, Pane>::iterator it;
	for (it = m_Panes.begin(); it!=m_Panes.end(); it++){
      T = vec3(m_Rect.left()+shift*S.x, m_Rect.top(), 0);
      MV = glm::translate(T)*glm::scale(S);
      qPtr->uploadMV(MV);
      qPtr->uploadColor(it->first == m_ActivePane ? activeColor : inactiveColor);
      qPtr->draw(it->second.getLabel());
		shift += 1.f;
   }

   //Draw active pane
	it = m_Panes.find(m_ActivePane);
	if (it != m_Panes.end())
		it->second.draw();
}

//Add a pane with string label l (if possible)
bool Menu::addPane(string l){
	if (m_Panes.size() > MENU_MAX_PANES)
		return false;
	m_Panes.emplace(l, l);
	return true;
}

//Get a pointer to the pane at label idx (returns null if non existent)
Pane * Menu::operator[](string idx){
	unordered_map<string, Pane>::iterator it(m_Panes.find(idx));
	if (it == m_Panes.end())
		return nullptr;
	return &(m_Panes[idx]);
}

//Handle event, return a set indicating what's happened to the menu items
MenuState Menu::handleEvent(SDL_Event * e){
   MenuState ret = { RESUME_MENU, false };

   //We need some default buttons for the home pane that do things
   //like quit everything and resume the game. add to constructor asap
   if (e->type == SDL_QUIT)
      ret.gs = QUIT_GAME;
   else if (e->type == SDL_KEYUP && e->key.keysym.sym == SDLK_ESCAPE)
      ret.gs = RESUME_GAME;
   else
      ret.update = m_Panes[m_ActivePane].handleEvent(e);
   // 1. Make the control state a Menu member; clear it after each loop
   // 2. Set the active pane based on mouse position
   // 3. Add anything the active state returns to the current state
   return ret;
}

/*
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
uint32_t Menu::rener(mat4 pInv){
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
*/
