#include "Entity.h"
#include <glm/gtx/transform.hpp>

Entity::Entity()
: sigmaSq(1.f){}

Entity::Entity(vec3 translate, vec3 scale)
: mCollider(BoundBox(scale))//, flip(false)
{
	this->translate(translate);
}

Entity::Entity(Collider c, Skeleton s)
: mCollider(c), mSkel(move(s)){//flip(false){
}

void Entity::setCol(Collider c){
	mCollider = c;
}

void Entity::setBB(BoundBox bb){
	mCollider.setBB(bb);
}

void Entity::setSkeleton(Skeleton s){
	mSkel = move(s);
}

//come back to this
void Entity::reflect(){
	mSkel.reflect();
//	mat4 refl = glm::translate(vec3(mCollider.getDim().x,0,0))*glm::scale(vec3(-1,1,1));
 //	mSkel.getRoot()->leftMultMV(refl);
//   flip = !flip;
}

void Entity::draw(){
	mSkel.draw(getPos());
//	mSkel.draw(glm::translate(getPos()));//getRoot()->draw(glm::translate(getPos()));
}

char Entity::collidesWith(Entity * e){
	char last = mCollider.collidesWith(e->mCollider);
	return last;
}

bool Entity::softCol(Entity * e){
	return mCollider.softCol(e->mCollider);
}

bool Entity::overlapsWith(Entity * e){
//	if (mCollider.overlapsWith(e->mCollider)) cout << this << " hit " << e << endl;
	return mCollider.overlapsWith(e->mCollider);
}

void Entity::moveTo(vec3 pos){
	mCollider.moveTo(pos);
}

void Entity::translate(vec3 trans){
	mCollider.translate(trans); 
} 
vec3 Entity::getPos(){ 
	return mCollider.getPos(); 
}

vec3 Entity::getPotential(vec3 pos){
	vec3 diff, a;
	float distSq;
	
	diff = pos - getPos();
	distSq = glm::dot(diff, diff);
	a = (float)(2.f*A*exp(-distSq/sigmaSq)/sigmaSq) * diff;

	return a;
}

float Entity::toLeft(Entity * e){ 
	return mCollider.toLeft(e->mCollider); 
} 
float Entity::toRight(Entity * e){ 
	return mCollider.toRight(e->mCollider); 
} 
float Entity::toBottom(Entity * e){ 
	return mCollider.toBottom(e->mCollider); 
} 
float Entity::toTop(Entity * e){ 
	return mCollider.toTop(e->mCollider); 
} 
float Entity::toNear(Entity * e){ 
	return mCollider.toNear(e->mCollider); 
} 
float Entity::toFar(Entity * e){ 
	return mCollider.toFar(e->mCollider); 
} 

vec3 Entity::center(){ 
	return mCollider.center(); 
}

vec3 Entity::getOrigin(string s, int idx){
	return getPos()+mSkel.getOrigin(s,idx);
}
