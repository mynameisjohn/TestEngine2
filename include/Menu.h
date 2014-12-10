#ifndef MENU_H
#define MENU_H

#include <SDL.h>

#include <Drawable.h>
#include <BoundRect.h>
#include <set>

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

#endif
