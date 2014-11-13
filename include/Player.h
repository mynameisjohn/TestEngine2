#ifndef PLAYER_H
#define PLAYER_H

#include "Projectile.h"

#include "ActiveEnt.h"
#include "EventRegister.h"
#include "Rig.h"

#include <list>

class Player: public ActiveEnt{
	public:
		Player();
		Player(const Entity&);
		Player(vec3 translate, vec3 scale);
		~Player();
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
		void addProjectile(Projectile p);
		bool overlapsWith(Entity * e);
		EventRegister * getRegPtr();
	private:
		//vector<unique_ptr<Projectile> > mProjectiles;
		list<Projectile> projList;
		Projectile mProj;
//		KeyboardHandler mHandler;
		float mDash;
		bool jumping;
		int activeProj;
public:
		EventRegister eReg;
};

#endif
