#include "Seeker.h"
#include <glm/gtc/random.hpp>

Seeker::Seeker()
: ActiveEnt()
{}

Seeker::Seeker(const Entity& e)
: ActiveEnt(e) {
	//this crap
   mSkel["leg1"]->set_cycle("walk");
   mSkel["leg2"]->set_cycle("walk");
   mSkel["arm1"]->set_cycle("walk");
   mSkel["arm2"]->set_cycle("walk");
/*
   mSkel["forearm1"]->set_cycle("walk");
   mSkel["forearm2"]->set_cycle("walk");
*/
}
/*
Seeker::Seeker(Entity * e)
: ActiveEnt(e)
{}
*/
Seeker::Seeker(vec3 translate, vec3 scale)
: ActiveEnt(translate, scale)
{}

void Seeker::update(){//vec3 goal){
	ActiveEnt::update();
	//acc = vec3();	
/*
	//a += glm::linearRand(-vec3(.25f,.25f,.25f), vec3(.25f,.25f,.25f));
   vec3 dt(mVel.x*goal.x < 0.f ? 7.5f : 0.75f, 0.f, mVel.z*goal.z < 0.f ? 7.5f : 0.75f);
   mVel = glm::clamp(mVel+dt*goal, -mSpeed, mSpeed);
   //jump
   if (grounded && fabs(goal.y) > 0.25f)
      mVel.y += mSpeed.y;
*/
/*
	if (health > 0)
		mSkel.setColor(health/100.f,health/100.f,health/100.f);
	else
		mSkel.setColor(0,0,0);//health/100.f,health/100.f,health/100.f)
*/
	bool right = mVel.x < 0;
	if ((!right && !mSkel.flipped()) || (right && mSkel.flipped()))
      reflect();

}

void Seeker::addPotential(Entity * e){
	unordered_map<Entity *, PotInfo>::iterator it=dispositions.find(e);
	if (it != dispositions.end()){
		vec3 diff(e->getPos()-getPos());
		float distSq(glm::dot(diff,diff)), A(dispositions[e].A), sig(dispositions[e].sig);
		vec3 dt(mVel.x*diff.x < 0.f ? 7.5f : 0.75f, 0.f, mVel.z*diff.z < 0.f ? 7.5f : 0.75f);
		vec3 acc((float)(-2.f*A*exp(-distSq/pow(sig,2))/pow(sig,2))*diff);
		vec3 rand(glm::linearRand(vec3(-.1),vec3(.1)));
		mVel = glm::clamp(mVel+dt*(acc+rand), -mSpeed, mSpeed);


		if (A<0){//one goal
			int state(L_CYCLIC);
			string to("walk");
			attacking = false;
			if (distSq < 10000){
				to="strike";
				attacking = true;
				aRect = BoundRect(vec2(center())+mSkel.getScale()*vec2(0.25,0), mSkel.getScale()*vec2(0.5,0.5));
				state = L_ONCE;	
			}
			else if (distSq < 500000)
				to="pre_strike";
			else
				to="walk";
			
			mSkel["arm2"]->set_to(to, state);
			mSkel["arm1"]->set_to(to, state);
		}
	}
}

void Seeker::predispose(Entity * e, PotInfo pI){
	dispositions[e] = pI;
}

void Seeker::takeHit(HitEffect hEff){
	health -= hEff.damage;
	if (health < 0)
		cout << "im ded" << endl;
}

//this doesn't really need to run ActiveEnt::overlapsWith
bool Seeker::overlapsWith(Entity * e){
	bool ret(ActiveEnt::overlapsWith(e));
	if (attacking && softCol(e)){
		if (e->getCollider()->overlapsWith(aRect))
			e->takeHit({10,{0,0,0}});
//cout << "gotcha" << endl;
	}
	return ret;
}
