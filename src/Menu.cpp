#include <GL_Includes.h>

#include <Textures.h>
#include <Menu.h>
#include <Util.h>

#include <glm/gtx/transform.hpp>

//Max number of Panes per menu / Controls per Pane
const int MENU_MAX_PANES(5), PANE_MAX_CONTROLS(5);
const vec3 BG_LAYER, BASE_LAYER(0,0,-.05), PANE_LAYER(0,0,-0.1), CONTROL_LAYER(0,0,-0.15);

void drawRect(BoundRect br, Drawable * drPtr, vec3 offset, vec4 color, string tex){
	mat4 MV = glm::translate(vec3(br.getPos(),0)+offset)*glm::scale(vec3(br.getDim(),1));
	drPtr->uploadMV(MV);
	drPtr->uploadColor(color);
	drPtr->draw(tex);
}

Control::Control()
 : drPtr(nullptr) { }

Control::Control(string l, BoundRect r, Drawable * d)
 : label(l), colRect(r), drPtr(d) { }

string Control::getLabel(){
	return label;
}

BoundRect Control::getRect(){
	return colRect;
}

Slider::Slider()
 : Control(), handle(nullptr) { }

Slider::Slider(string l, BoundRect r, Drawable * d, float v, float m, float M, float * h)
 : Control(l, r, d), value(v), minValue(m), maxValue(M), handle(h) { }


//Don't
const vec2 rangePos(.1,.6), labelPos(.05,.05), handleSize(.04,.12);
const float labelHeight(0.25f), rangeHeight(.05);

//I only need this one
BoundRect Slider::getHandleRect(){
	vec2 pos(colRect.getPos()), dim(colRect.getDim());
	float rangeWidth = dim.x-2.f*rangePos.x*dim.x;
	pos += vec2(lerp(0,rangeWidth,value),-rangeHeight*dim.y/2.f)+dim*(rangePos-handleSize/2.f);
	dim = max(dim)*handleSize;
	return BoundRect(pos,dim);
}

BoundRect Slider::getLabelRect(){
	vec2 pos(colRect.getPos()), dim(colRect.getDim());
	pos = pos+dim*labelPos;
	dim = vec2(dim.x-2.f*labelPos.x*dim.x, dim.y*labelHeight);
	return BoundRect(pos,dim);
}

BoundRect Slider::getRangeRect(){
	vec2 pos(colRect.getPos()), dim(colRect.getDim());
	pos = pos+dim*rangePos;
	dim = vec2(dim.x-2.f*rangePos.x*dim.x, dim.y*rangeHeight);
	return BoundRect(pos,dim);
}

Switch::Switch()
 : Control(), state(0), handle(nullptr) { }

Switch::Switch(string l, BoundRect r, Drawable * d, vector<string> o, uint32_t s, uint32_t * h)
 : Control(l, r, d), options(o), state(s), handle(h) { }

void Switch::draw(){
}

bool Switch::update(){
	return false;
}

//I'll be using real textures...eventually
void Slider::draw(){
	vec4 aColor(vec3(0.3),1), bColor(vec3(0.5),1), cColor(vec3(0.7),1), dColor(vec3(0.9),1);
	vec3 off(0,0,-0.05);
	BoundRect A(colRect);
	BoundRect B(getLabelRect());//A.getPos()+c, {A.getDim().x-2*c.x,lWidth});
	BoundRect C(getRangeRect());//A.getPos()+b, {A.getDim().x-2*b.x,bWidth});

	//Put this one in memory
	BoundRect D(getHandleRect());
//C.getPos()+vec2(lerp(0,C.getDim().x,value),0)-A.getDim()*h/2.f,max(A.getDim())*h);

	static float osc(0);
	value = 0.5f*sin(osc)+0.5f;
	osc += 0.01f;

	//Draw base rectangle
	drawRect(A, drPtr, 1.5f*off, aColor);
	//Draw label
	drawRect(B, drPtr, 1.755f*off, bColor);

	//Draw Slider line
	drawRect(C, drPtr, 2.f*off, cColor);

	//Draw Slider handle
	drawRect(D, drPtr, 2.25f*off, dColor);

}

bool Slider::update(){
	if (handle)
		*handle = lerp(minValue, maxValue, clamp(value,0,1));
	return false;
}

//Main constructor
Pane::Pane(string l)
   :  m_Label(l) 
{}

bool Pane::update(){
	bool ret(false);
   vector<unique_ptr<Control> >::iterator cIt;
   for (cIt=m_Controls.begin(); cIt!=m_Controls.end(); cIt++)
      if ((*cIt)->update())
			ret = true;
	return ret;
}

//Draw all of the pane's controls
void Pane::draw(){
   vector<unique_ptr<Control> >::iterator cIt;
   for (cIt=m_Controls.begin(); cIt!=m_Controls.end(); cIt++)
      (*cIt)->draw();
}

//Probably just send this normalized mouse position...
bool Slider::handleEvent(SDL_Event * e){
	if (e->type == SDL_MOUSEMOTION){
		float x(e->motion.x), y(e->motion.y);
		cout << vec2(x,y) << endl;
	}
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
   vec4 white(1), activeColor(vec3(0.1),0.95), inactiveColor(vec3(0.5),0.95), baseColor(vec3(0.1),0.95);
   mat4 MV;

	//Draw screen as background
	drawRect(BoundRect({-1,-1},{2,2}),qPtr,vec3(),vec4(1),"screen");

   //Draw base quad
	drawRect(m_Rect, qPtr, BASE_LAYER, baseColor);

	//Clean this up later
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

bool Menu::update(){
	bool ret(false);
	unordered_map<string, Pane>::iterator it;
	for (it = m_Panes.begin(); it!=m_Panes.end(); it++)
		if (it->second.update())
			ret = true;
	return ret;
}

//Add a pane with string label l (if possible)
bool Menu::addPane(string l){
	if (m_Panes.size() > MENU_MAX_PANES || m_Panes.find(l) != m_Panes.end())
		return false;

	m_Panes.emplace(l, l);
	qPtr->addTex(l, fromTextString(l));

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
   else{
		switch (e->type){
			case SDL_MOUSEBUTTONDOWN:
				//Here we'd have to check if the player has clicked a new active pane
				//won't work with the unordered_map setup
			default: break;
		}	
		if (m_Panes.find(m_ActivePane) != m_Panes.end())
			ret.update = m_Panes[m_ActivePane].handleEvent(e);
	}

   return ret;
}
