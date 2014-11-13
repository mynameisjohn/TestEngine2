//Probably make this abstract at some point

#ifndef ACTIVEENT_H
#define ACTIVEENT_H
#include <stdlib.h>
#include "Entity.h"
#include <stdio.h>

class ActiveEnt: public Entity{
	public:
		ActiveEnt();
		ActiveEnt(const Entity&);
		ActiveEnt(vec3 translate, vec3 scale);
		void setHealth(float h);
		virtual void update();//vec3 a);//NYI
		virtual char moveWRT_ent(Entity * e); //make this virtual
		void moveToEdge(int, Entity *);
		virtual void move();//Make this pure asap
		vec3 getVel();
		char isGrounded();
	protected:
		vec3 mVel; //Active velocity
		vec3 mSpeed; //Individual speed
		char grounded; //Whether it's on top of something
		char falling;
		float speedOsc;
		float health;
};

#endif
