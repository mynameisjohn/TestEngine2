#include "Projectile.h"
#include <glm/gtx/transform.hpp>

//these have to be formally deactivated (by a function)
//also add a counter

const int DEAD(0), POISED(1), ACTIVE(2), LIFETIME(500);

Projectile::Projectile()
: ActiveEnt(), state(0), ticker(0){
}
/*
Projectile::Projectile(Entity * e)
: ActiveEnt(e), state(0), ticker(0){
}
*/

Projectile::Projectile(const Entity& e)
: ActiveEnt(e), state(0), ticker(0) {
}

Projectile::Projectile(vec3 translate, vec3 scale)
: ActiveEnt(translate, scale), state(0), ticker(0){

}

void Projectile::draw(){
//	cout << &mSkel << "\t" << mSkel["root"] << endl;
	if (state==ACTIVE){//fix center of rotation
		mSkel.getRoot()->applyTransform(getQuatFromVec2(vec2(mVel)));
		mSkel.draw(getPos());//glm::translate(getPos()));// * getAlignMat(mVel, 'x'));
	}
	else if (state == POISED)
		mSkel.draw(getPos());
//	if (state)
//		cout << (&mSkel) << "\t" << mSkel.getRoot() << "\t" <<mSkel.getRoot()->mDrawable << endl;
	
	mSkel.resetTransform();

}

void Projectile::update(){
	if (grounded)
      mVel.y=0.f;
   else
      mVel.y -= 2.f;

	if (++ticker > LIFETIME)
		kill();
}

char Projectile::moveWRT_ent(Entity * e){
	char last = ActiveEnt::moveWRT_ent(e);

	switch (last){
		case 'x':
		case 'y':
		case 'z':	
//			state = false;
//			mVel = vec3();
			e->takeHit({10.f,{0,0,0}});
			//cout << last << endl;
		default:
			break;
	}

	return last;
}


void Projectile::kill(){
	state=DEAD;
	mVel=vec3();
//	moveTo({0,0,0});
}

bool Projectile::overlapsWith(Entity * e){
	bool ret = Entity::overlapsWith(e);	
	if (ret){
		e->takeHit({10.f,{0,0,0}});
		ticker = LIFETIME-1;
	}


	return ret;
}


void Projectile::launch(vec3 dir, float speed){
	mVel = speed * glm::normalize(dir);//vec3(10,0,0);
//	mSkel.resetLigaments(); //not sure about this
}

void Projectile::launch(vec2 dir, float speed){
	mVel = speed * vec3(glm::normalize(dir),0);//vec3(10,0,0);
	//cout << mVel << endl;
}

void Projectile::setState(int s){
	state = s;
	if (state != DEAD)
		grounded = false;
}

bool Projectile::isAlive(){
	return state != DEAD;
}
bool Projectile::isPoised(){

	return state == POISED;
}

bool Projectile::isActive(){
	return state == ACTIVE;
}
