#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <Drawable.h>

enum MenuState{
	MENU_QUIT,
	MENU_RESUME,
	MENU_CONTINUE
};

class Menu{
	uint32_t FBO, tex, downSample;
	Drawable * drPtr;
public:
	Menu();
	Menu(Drawable * base, uint32_t w, uint32_t h, uint32_t dS = 1);
	bool isFBOComplete();
	bool grabScreen(uint32_t w, uint32_t h);
	MenuState handleEvent(SDL_Event& e);
	uint32_t update();
	uint32_t render(mat4 pInv);
};

#endif
