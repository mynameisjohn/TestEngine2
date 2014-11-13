#ifndef SOUP_H
#define SOUP_H

#include "Seeker.h"
#include "Player.h"
#include "Wall.h"
#include <unordered_map>
#include "Population.h"

/*
//add fields for projectiles?
typedef struct EntInfo{
	string name;
   vec3 translate;
   vec4 rotate;
   vec3 scale;
   vec3 color;
	string projFileName;
	int numProj;
} EntInfo;
*/
class Soup{
public:
	Soup(unordered_map<string, unique_ptr<Drawable> > * dMP = nullptr)
	: dMapPtr(dMP){}

	//This is some shit that I added
	unique_ptr<Population> createPopulation(string levelFile, JShader& shader);
/*
	unique_ptr<Player> initPlayer(EntInfo eI, JShader& shader);
	Projectile initProjectile(string fileName, JShader& shader);
	unique_ptr<Obstacle> initObstacle(EntInfo eI, JShader& shader);
	unique_ptr<ActiveEnt> initAe(EntInfo eI, JShader& shader);
	unique_ptr<Seeker> initSeeker(EntInfo eI, JShader& shader);
	unique_ptr<Wall> initWall(EntInfo eI, char orientation, JShader& shader);
*/
private:
	unordered_map<string, unique_ptr<Drawable> > * dMapPtr;
//	Entity loadEntFromFile(EntInfo eI, JShader& shader);
};

#endif
