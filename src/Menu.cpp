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

const vec3 BG_LAYER, BASE_LAYER(0,0,-.05), PANE_LAYER(0,0,-0.1), CONTROL_LAYER(0,0,-0.15);

//Draw the menu and its components
void Menu::draw(){
   //Scale, translation, active/inactive colors, model view matrix
   vec3 S, T;
   vec4 white(1), activeColor(vec3(0.1),0.87), inactiveColor(vec3(0.5),0.87), baseColor(vec3(0.1),0.87);
   mat4 MV;

	//Draw screen as background
	MV = glm::translate(vec3(-1,-1,0) + BG_LAYER)*glm::scale(vec3(2,2,1));
	qPtr->uploadMV(MV);
	qPtr->uploadColor(white);
	qPtr->draw("screen");

   //Draw base quad
   S = vec3(m_Rect.getDim(),1);// - vec2(0, -m_PaneHeight), 1);
   T = vec3(m_Rect.getPos(),0);// - vec2(0, -m_PaneHeight), 1);
   MV = glm::translate(T + BASE_LAYER)*glm::scale(S);
   qPtr->uploadMV(MV);
   qPtr->uploadColor(baseColor);
   qPtr->draw();

   //Draw Pane tabs
   S = vec3(m_Rect.getDim().x/((float)m_Panes.size()), m_PaneHeight, 1);
	float shift(0);
	unordered_map<string, Pane>::iterator it;
	for (it = m_Panes.begin(); it!=m_Panes.end(); it++){
      T = vec3(m_Rect.left()+shift*S.x, m_Rect.top(), 0) + PANE_LAYER;
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
	if (m_Panes.size() > MENU_MAX_PANES || m_Panes.find(l) != m_Panes.end())
		return false;

	m_Panes.emplace(l, l);

	if (m_Panes.find(m_ActivePane) == m_Panes.end())
		m_ActivePane = l;

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
		if (m_Panes.find(m_ActivePane) != m_Panes.end())
	      ret.update = m_Panes[m_ActivePane].handleEvent(e);
	else cout << "no active pane" << endl;
   // 1. Make the control state a Menu member; clear it after each loop
   // 2. Set the active pane based on mouse position
   // 3. Add anything the active state returns to the current state
   return ret;
}
