/*
	BUG : Not really, but since the panes are in an unordered map I don't really
	have a say in what order they're in (and what order they get drawn in.)
	Resolve it. 
*/

#ifndef MENU_H
#define MENU_H

#include <SDL.h>

#include <Drawable.h>
#include <BoundRect.h>
#include <memory>

void drawRect(BoundRect br, Drawable * drPtr, vec3 offset=vec3(), vec4 color=vec4(1), string tex="");

enum GameState{
	RESUME_MENU=1,
	RESUME_GAME=2,
	QUIT_GAME=0
};

struct MenuState{
	GameState gs;
	bool update;
};

class Control{
public:
   Control();
   Control(string, BoundRect, Drawable *);
   virtual void draw() = 0;
   virtual bool handleEvent(SDL_Event * e) = 0;
   virtual bool handleEvent(vec2 mousePos, bool rmbDown) = 0;
	virtual bool update() = 0;
	string getLabel();
	BoundRect getRect();
protected:
   string label;
   BoundRect colRect;
   Drawable * drPtr;
};


class Slider : public Control{
public:
   Slider();
   Slider(string l, BoundRect rect, Drawable * drPtr, float v, float m, float M, float * h);
   void draw();
   bool handleEvent(SDL_Event * e);
   bool handleEvent(vec2 mousePos, bool rmbDown);
	bool update();
private:
   float value, minValue, maxValue, * handle;
	BoundRect getHandleRect(), getLabelRect(), getRangeRect();
	bool grabbed;
	float x0,x1;;
};


class Switch : public Control{
public:
   Switch();
   Switch(string l, BoundRect rect, Drawable * d,unordered_map<string, uint32_t> o, uint32_t * h, 
																															string a="");
   void draw();
   bool handleEvent(SDL_Event * e);
   bool handleEvent(vec2 mousePos, bool rmbDown);
	bool update();
private:
	string active;
	unordered_map<string, uint32_t> options;
//   vector<string> options;
   uint32_t * handle;
};


class Pane{
public:
	Pane(string l = "pane");
	void draw();
	bool addControl(Control& c);
	bool handleEvent(SDL_Event * e);
   bool handleEvent(vec2 mousePos, bool rmbDown);
	bool update();
	string getLabel();
private:
	string m_Label;
	vector<unique_ptr<Control> > m_Controls;
};


class Menu{
public:
	Menu(BoundRect r = BoundRect(vec2(-0.5f), vec2(1)), float pH = 0.15f, Drawable * d = nullptr);
	void draw();
	bool addPane(string l);
	bool update();
	uint32_t * getFBO(){ return &m_FBO; }
	uint32_t * getTex(){ return &m_Tex; }
	
	Pane * operator[](string idx);
	MenuState handleEvent(SDL_Event * e);
	MenuState handleEvent(vec2 mousePos, bool rmbDown);
private:
	BoundRect m_Rect;
	float m_PaneHeight;
	Drawable * qPtr;
	uint32_t m_FBO, m_Tex;
	string m_ActivePane;
	unordered_map<string, Pane> m_Panes;
};







/*
template <class T>
class Set{
public:
	set<T> data;
	Set();
	bool operator && (T& item){
		return (data.find(item) != data.end());
	}
	void operator |= (T& item){
		data.emplace(T);
	}
};
*/

/*
enum ButtonState{
	BUT_MouseOver = 1,
	BUT_RightClicked = 2,
	BUT_LeftClicked = 4,
	BUT_MiddleClicked = 8,
	BUT_Toggled = 16
};

class Button{
	BoundRect rect;
	set<ButtonState> state;
public:
	Button();
	Button(vec2 dim);
	Button(vec2 pos, vec2 dim);
	void moveTo(vec2 pos);
	void clearState();
	void addToState(ButtonState bS);
	void removeFromState(ButtonState bS);
	bool containsState(ButtonState bS);
	BoundRect getRect();
};

class Pane{
public:
	Pane();
	Pane(BoundRect a, BoundRect b, Drawable * drPtr);
	bool handleEvent(SDL_Event& e);
	vector<unique_ptr<Control> > controls;
private:
	BoundRect main, tab;
	Drawable * drPtr;
};

enum MenuState{
	MENU_QUIT,
	MENU_RESUME,
	MENU_CONTINUE
};

class Menu{
	uint32_t FBO, tex, downSample;
	Drawable * drPtr;
	vector<Button> buttons;
public:
	Menu();
	Menu(Drawable * base, vec2 screenDim, uint32_t dS = 1);
	void push_back(Button b);
	void emplace_back(vec2 pos, vec2 dim);
	void clearButtons();
	bool isFBOComplete();
	bool grabScreen(vec2 screenDim);
	MenuState handleEvent(SDL_Event& e, vec2 screenDim);
	uint32_t update();
	uint32_t render(mat4 pInv);
};
*/
#endif
