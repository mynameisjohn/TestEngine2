/*
	BUG : Not really, but since the panes are in an unordered map I don't really
	have a say in what order they're in (and what order they get drawn in.)
	Resolve it. 
*/

#ifndef MENU_H
#define MENU_H

#include <Drawable.h>
#include <BoundRect.h>
#include <memory>

void drawRect(BoundRect br, Drawable * drPtr, vec3 offset=vec3(), vec4 color=vec4(1), string tex="");
BoundRect getMouseRect(vec2 mousePos);

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
   bool handleEvent(vec2 mousePos, bool rmbDown);
	bool update();
private:
   float value, minValue, maxValue, * handle;
	BoundRect getHandleRect(), getLabelRect(), getRangeRect();
	bool grabbed;
};


class Switch : public Control{
public:
   Switch();
   Switch(string l, BoundRect rect, Drawable * d,unordered_map<string, uint32_t> o, uint32_t * h, 
																															string a="");
   void draw();
   bool handleEvent(vec2 mousePos, bool rmbDown);
	bool update();
private:
	string active;
	unordered_map<string, uint32_t> options;
   uint32_t * handle;
	BoundRect getOptionRect(int i);
};


class Pane{
public:
	Pane(string l = "pane");
	void draw();
	bool addControl(Control& c);
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
	MenuState handleEvent(vec2 mousePos, bool rmbDown);
private:
	BoundRect m_Rect;
	float m_PaneHeight;
	Drawable * qPtr;
	uint32_t m_FBO, m_Tex;
	int m_ActivePane;//string m_ActivePane;
	vector<Pane> m_Panes;
	BoundRect getPaneRect(int i);//IMPLEMENT THIS
};

#endif
