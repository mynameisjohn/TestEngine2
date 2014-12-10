#ifndef BASEENGINE_H
#define BASEENGINE_H

#include <Drawable.h>
#include <Camera.h>
#include <JShader.h>
#include <Population.h>
#include <Level.h>

#include <memory>
#include <SDL.h>
#include <iostream>


class BaseEngine{
public:
	BaseEngine();
	~BaseEngine();
	bool init(std::string vs, std::string fs, vec2 screenDim);
	void update();
	void move();
	void render();
	void handleEvent(SDL_Event& e);
	void bindShader();
	void unBindShader();
	void updateProjMat();
	void clearKeyState();
	vec2 getScreenDim();
	Drawable * getDrawablePtr(string name);
	mat4 getProjMat();
private:
	Camera cam;
	JShader shader;
	Hud hud;
	unique_ptr<Level> level;
	bool motionHandled;
	void handleMotion(float x, float y);
	unordered_map<string, unique_ptr<Drawable> > dMap;
};

int keyCode(SDL_Event& e);

#endif
