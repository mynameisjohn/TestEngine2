#include "Drawable.h"
#include "Camera.h"
#include "JShader.h"
#include "Population.h"
#include "Level.h"

#include <memory>
#include <SDL2/SDL.h>
#include <iostream>


class BaseEngine{
public:
	BaseEngine();
	~BaseEngine();
	bool init(std::string vs, std::string fs);
	void update();
	void move();
	void render();
	void handleEvent(SDL_Event& e);
private:
	Camera cam;
	JShader shader;
	unique_ptr<Population> pop;
	Level level;
//	vector<Drawable> drawables;
};

int keyCode(SDL_Event& e);
/*
bool initGL();

bool initLevel();

Drawable initQuad();

int update(void * data);

void move();

void closeShader();

void handleEvent(SDL_Event& e);

void render();

void initPlayer();

void initObstacle(int x, int y);
*/
