#include <Level.h>
#include <Rig.h>

const string LEVEL_ONE("res/level/one.xml");

Level::Level()
: activePop(0) {
}

//this was segfaulting, and now it isn't. be careful
Level::Level(int l, JShader& shader)
: Level(){
//this is bogus for now
	Soup s(&dMap);
	popVec.push_back(std::move(s.createPopulation(LEVEL_ONE, shader)));
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
