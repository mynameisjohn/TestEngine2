#ifndef PLAYER_H
#define PLAYER_H

#include <Projectile.h>
#include <ActiveEnt.h>
#include <EventRegister.h>

#include <list>

class Player: public ActiveEnt{
	public:
		Player();
		Player(const Entity&);
		Player(vec3 translate, vec3 scale);
		int setChildren();
		void move();
		char moveWRT_ent(Entity * e);
		void draw();
		void update();//EventInfo);
		void updateKeys();
		void updateMouse();
		void updateSkeleton();
		void takeHit(HitEffect hEff);
		void setProjectile(Projectile p);
		bool overlapsWith(Entity * e);
		float getCharge();
		EventRegister * getRegPtr();
	private:
		list<Projectile> projList;
		Projectile mProj;
		float mDash;
		bool jumping;
		float charge;
public:
		EventRegister eReg;
};

#endif
