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

//Font and border should be a function of string length
Control::Control(string l, BoundRect r, Drawable * d)
 : label(l), colRect(r), drPtr(d){
	vec2 border(0.5f, 0.1f);
	drPtr->addTex(label, fromTextString(label, 100, vec3(), vec3(1), border));
}

string Control::getLabel(){
	return label;
}

BoundRect Control::getRect(){
	return colRect;
}

Slider::Slider()
 : Control(), handle(nullptr) { }

Slider::Slider(string l, BoundRect r, Drawable * d, float v, float m, float M, float * h)
 : Control(l, r, d), value(remap(v,m,M,0.f,1.f)), minValue(m), maxValue(M), handle(h), grabbed(false) { }


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
 : Control(), handle(nullptr) { }

Switch::Switch(string l, BoundRect r, Drawable * d, unordered_map<string, uint32_t> o, uint32_t * h,
string a)
 : Control(l, r, d), active(a), options(o), handle(h){ 
	
}

BoundRect Switch::getOptionRect(int i){
	const vec2 optionsPos(0.1,0.4);
   const float height(0.35);
   float width = colRect.getDim().x*(1.f-2.f*optionsPos.x);
	float off(i), N(options.size());
	vec2 dim(width/N, height*colRect.getDim().y);
	vec2 pos(colRect.getPos()+optionsPos*colRect.getDim()+vec2(off*width/N, 0));

	return BoundRect(pos,dim);
}

//Need a label rect
void Switch::draw(){
	vec4 white(1), baseColor(vec3(0.3),1), color(vec3(),1);	

	drawRect(colRect, drPtr, CONTROL_LAYER, baseColor);
	
	int i(0);
	unordered_map<string, uint32_t>::iterator it;
	for (it=options.begin(); it!=options.end(); it++)
		drawRect(getOptionRect(i++), drPtr, vec3(0,0,-.1)+CONTROL_LAYER, 
			active == it->first ? vec4(1,0.5,0.5,1) : white);
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

	//Draw base rectangle
	drawRect(A, drPtr, 1.5f*off, aColor);
	//Draw label
	drawRect(B, drPtr, 1.755f*off, bColor, label);

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

BoundRect getMouseRect(vec2 mousePos){
	const vec2 mouseDim(0.01);
	return BoundRect(mousePos-mouseDim/2.f,mouseDim);
}

//I should make a BoundCircle class...
bool Slider::handleEvent(vec2 mousePos, bool lmbDown){
	BoundRect hRect(getHandleRect());
	BoundRect mouseRect(getMouseRect(mousePos));//mousePos-mouseDim/2.f,mouseDim);

	//if grabbed, find diff between mouseRect.center and hRect.center
	//remap this difference from between 0, rangeRect.dim.x to 0,1
	//this is the new value

	if (!grabbed){
		if (hRect.collidesWith(mouseRect) && lmbDown){
			grabbed = true;
		}
	}
	else
		grabbed = lmbDown;
	
	if (grabbed){
		BoundRect rangeRect(getRangeRect());
		float dx_range = mouseRect.center().x - hRect.center().x;
		float dx_value = remap(dx_range, 0.f, rangeRect.getDim().x, 0.f, 1.f);
		value = clamp(value+dx_value, 0, 1);
	}

	return false;
}
/*
//Probably just send this normalized mouse position...
bool Slider::handleEvent(SDL_Event * e){
	if (e->type == SDL_MOUSEMOTION){
		float x(e->motion.x), y(e->motion.y);
		cout << vec2(x,y) << endl;
	}
   //if event is mousedown, start tracking horizontal movement (make note of x pos, I guess)
   //if mouseUp, and if the mouse was already down (store this as toggle), reassign value
   //handle = lerp(value, minValue, maxValue); //implying value [0,1]
	return true;
}
*/
bool Switch::handleEvent(vec2 mousePos, bool lmbDown){
	BoundRect mouseRect(getMouseRect(mousePos));

	int i(0);
   unordered_map<string, uint32_t>::iterator it;
   for (it=options.begin(); it!=options.end(); it++){
		BoundRect optionRect(getOptionRect(i++));
		if (mouseRect.collidesWith(optionRect) && lmbDown){
//			*handle = it->second;
			active = it->first;
			//cout << it->second << endl;
			return true;
		}
	}

	return false;	
}
/*
bool Switch::handleEvent(SDL_Event * e){
	if (e->type == SDL_MOUSEMOTION){
		float x(e->motion.x), y(e->motion.y);
		cout << vec4(x,y,7,8) << endl;
	}
   //check collision with all the switches (divide up the dim by the number of options, make quads)
   //if one was mouse'd down and is mouse'd (') up again, make that the current state
   //state = idxOf(options, selected);
   //handle should probably be an enum (or passed via constructor) //map<uint32_t, string>;
	return true;
}
*/
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

bool Pane::handleEvent(vec2 mousePos, bool lmbDown){
	bool update(false);
   vector<unique_ptr<Control> >::iterator cIt;
   for (cIt=m_Controls.begin(); cIt!=m_Controls.end(); cIt++)
      if ((*cIt)->handleEvent(mousePos, lmbDown))
         update = true;
   return update;
}
/*
bool Pane::handleEvent(SDL_Event * e){
   //Should I check if the mouse collides with a control?
   bool update(false);
   vector<unique_ptr<Control> >::iterator cIt;
   for (cIt=m_Controls.begin(); cIt!=m_Controls.end(); cIt++)
      if ((*cIt)->handleEvent(e))
         update = true;
   return update;
}
*/
//Main constructor
Menu::Menu(BoundRect r, float pH, Drawable * d)
      :  m_Rect(r), 
			m_PaneHeight(pH), 
			qPtr(d), 
			m_FBO(0), 
			m_Tex(0),
			m_ActivePane(0)
{}

BoundRect Menu::getPaneRect(int i){
	float shift(i);
	vec2 dim(m_Rect.getDim().x/((float)m_Panes.size()), m_PaneHeight);
	vec2 pos(m_Rect.left()+shift*dim.x, m_Rect.top());
	return BoundRect(pos, dim);
}

//Draw the menu and its components
void Menu::draw(){
   //Scale, translation, active/inactive colors, model view matrix
   vec4 white(1), activeColor(vec3(0.1),0.95), inactiveColor(vec3(0.5),0.95), baseColor(vec3(0.1),0.95);
   mat4 MV;

	//Draw screen as background
	drawRect(BoundRect({-1,-1},{2,2}),qPtr,vec3(),vec4(1),"screen");

   //Draw base quad
	drawRect(m_Rect, qPtr, BASE_LAYER, baseColor);

	for (int i=0; i<m_Panes.size(); i++)
		drawRect(getPaneRect(i), qPtr, PANE_LAYER, 
			(i == m_ActivePane ? activeColor : inactiveColor), m_Panes[i].getLabel());
	
/*
	//WE NEED A DIFFERENT DATA STRUCTURE (so I can order the panes my own way)
	//Clean this up later
   //Draw Pane tabs
	vec2 dim(m_Rect.getDim().x/((float)m_Panes.size()), m_PaneHeight);
	float shift(0);
	unordered_map<string, Pane>::iterator it;
	for (it = m_Panes.begin(); it!=m_Panes.end(); it++){
		BoundRect pRect(vec2(m_Rect.left()+shift*dim.x, m_Rect.top()), dim);
		drawRect(pRect, qPtr, PANE_LAYER, (it->first == m_ActivePane ? activeColor : inactiveColor),
			it->second.getLabel());
		shift += 1.f;
   }
*/
	m_Panes[clamp(m_ActivePane, 0, m_Panes.size()-1)].draw();
/*
   //Draw active pane
	it = m_Panes.find(m_ActivePane);
	if (it != m_Panes.end())
		it->second.draw();*/
}

//Why all of them?
bool Menu::update(){
	bool ret(false);
	vector<Pane>::iterator it;
	for (it=m_Panes.begin(); it!=m_Panes.end(); it++)
		if (it->update())
			ret = true;/*
	unordered_map<string, Pane>::iterator it;
	for (it = m_Panes.begin(); it!=m_Panes.end(); it++)
		if (it->second.update())
			ret = true;*/
	return ret;
}

//Add a pane with string label l (if possible)
bool Menu::addPane(string l){
	if (m_Panes.size() > MENU_MAX_PANES)// || m_Panes.find(l) != m_Panes.end())
		return false;

	vector<Pane>::iterator it;
	for (it=m_Panes.begin(); it!=m_Panes.end(); it++)
		if (it->getLabel() == l)
			return false;

	m_Panes.push_back(l);
	vec2 border(m_Rect.getDim().x/2.f, 0.1f);
	qPtr->addTex(l, fromTextString(l, 100, vec3(), vec3(1), border));
/*

	m_Panes.emplace(l, l);
	qPtr->addTex(l, fromTextString(l));

	if (m_Panes.find(m_ActivePane) == m_Panes.end())
		m_ActivePane = l;
*/
	return true;
}

//Get a pointer to the pane at label idx (returns null if non existent)
Pane * Menu::operator[](string idx){
	vector<Pane>::iterator it;
	for (it=m_Panes.begin(); it!=m_Panes.end(); it++)
		if (it->getLabel() == idx)
			return &(*it);
	return nullptr;
/*
	unordered_map<string, Pane>::iterator it(m_Panes.find(idx));
	if (it == m_Panes.end())
		return nullptr;
	return &(m_Panes[idx]);
*/
}

MenuState Menu::handleEvent(vec2 mousePos, bool lmbDown){
	MenuState ret = { RESUME_MENU, false };
	BoundRect mouseRect(getMouseRect(mousePos));

	for (int i=0; i<m_Panes.size(); i++)
		if (mouseRect.collidesWith(getPaneRect(i)) && lmbDown)
			m_ActivePane = i;
		//We could probably just return here


	ret.update = m_Panes[clamp(m_ActivePane, 0, m_Panes.size()-1)].handleEvent(mousePos, lmbDown);
/*
	if (m_Panes.find(m_ActivePane) != m_Panes.end())
         ret.update = m_Panes[m_ActivePane].handleEvent(mousePos, lmbDown);
*/
	return ret;
}
/*
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
*/
