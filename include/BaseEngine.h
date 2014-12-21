#ifndef BASEENGINE_H
#define BASEENGINE_H

//#include <Drawable.h>
#include <Camera.h>
//#include <JShader.h>
//#include <Population.h>
#include <Level.h>
#include <Menu.h>

#include <memory>
#include <SDL.h>
#include <iostream>


class BaseEngine{
public:
	BaseEngine();
	~BaseEngine();
	bool init(std::string vs, std::string fs);
	void update();
	void move();
	void draw();
	void bindShader();
	void unBindShader();
	void updateProjMat();
	void clearKeyState();
	bool grabScreen();
	vec2 getScreenDim();
	GameState handleEvent(SDL_Event * e);
	GameState iterate();
	Drawable * getDrawablePtr(string name);
	mat4 getProjMat();
private:
	GameState m_Status;
	Camera cam;
	JShader shader;
	Hud hud;
	unique_ptr<Menu> menu;
	unique_ptr<Level> level;
	bool motionHandled;
	void handleMotion(float x, float y);
	unordered_map<string, unique_ptr<Drawable> > dMap;
	float volume;
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

#endif
