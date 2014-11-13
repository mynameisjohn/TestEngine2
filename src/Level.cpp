#include "Level.h"
#include "Rig.h"
#include <glm/gtx/transform.hpp> 

//get rid of this ASAP
#include <tinyxml.h>
#include <functional>


unique_ptr<Population> Level::initLevelOne(JShader& shader){

	const vec3 wallMin(-1000, -600, -1000);
   const vec3 wallMax(7000, 2000, -4000);

	if (!s)
		s = new Soup(&dMap);

	return s->createPopulation("res/level/one.xml", shader);
}

Level::Level()
: s(nullptr), activePop(0) {
}

//this was segfaulting, and now it isn't. be careful
Level::Level(int l, JShader& shader)
: Level(){
//this is bogus for now
	popVec.push_back(std::move(initLevelOne(shader)));
}

vec4 Level::move(){
	return popVec[activePop]->move();
}

void Level::update(){
	popVec[activePop]->update();
}

void Level::draw(){
	popVec[activePop]->draw();
}

Player * Level::getPlayer(){
	return popVec[activePop]->getPlayer();
}
