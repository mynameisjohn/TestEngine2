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
	bool init(std::string vs, std::string fs);
	void update();
	void move();
	void render();
	void handleEvent(SDL_Event& e);
	void bindShader();
	void unBindShader();
	void updateProjMat();
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
