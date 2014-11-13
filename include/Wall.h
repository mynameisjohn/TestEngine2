#ifndef WALL_H
#define WALL_H

#include "Obstacle.h"

class Wall : public Obstacle{
public:
	Wall();
	Wall(const Entity& e);
	Wall(vec3 translate, vec3 scale, char o);
	~Wall();
	void update();
private:
	char orientation;
};

#endif
