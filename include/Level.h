#ifndef LEVEL_H
#define LEVEL_H

#include <JShader.h>
#include <Soup.h>
#include <Wall.h>

class Level{
public:
	Level();
	Level(int l, Hud& hud, JShader& shader, unordered_map<string, unique_ptr<Drawable> > * dMapPtr);
	vec4 move();
	void update();
	void draw();
	Player * getPlayer();
private:
	vector<unique_ptr<Population> > popVec;
	int activePop;

//	void initWalls(vec3 min, vec3 max, Population * pop, JShader& shader);
};

//unique_ptr<Population> initLevel(JShader& shader, int level);
//unique_ptr<Population> initLevelOne(JShader& shader);
//void initWalls(vec3 min, vec3 max, Population * pop, JShader& shader);
#endif
