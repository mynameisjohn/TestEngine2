#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "ActiveEnt.h"

class Projectile: public ActiveEnt{
	public:
		Projectile();
		Projectile(const Entity&);
		Projectile(vec3 translate, vec3 scale);
		void update();
//		void move();
		bool overlapsWith(Entity * e);
		void draw();
		char moveWRT_ent(Entity * e);
		void launch(vec3 dir=vec3(1,0,0), float speed=20.f);
		void launch(vec2 dir=vec2(1,0), float speed=20.f);
		void setState(int s);
		void kill();
		bool isActive();
		bool isAlive();
		bool isPoised();
	protected:
		int state, ticker;
};

#endif
