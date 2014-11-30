#ifndef SOUP_H
#define SOUP_H

#include <unordered_map>

#include <Seeker.h>
#include <Player.h>
#include <Wall.h>
#include <Population.h>
#include <Hud.h>

class Soup{
public:
	Soup(unordered_map<string, unique_ptr<Drawable> > * dMP = nullptr)
	: dMapPtr(dMP){}
	unique_ptr<Population> createPopulation(string levelFile, Hud& hud, JShader& shader);
private:
	unordered_map<string, unique_ptr<Drawable> > * dMapPtr;
};

#endif
