#include "Wall.h"

Wall::Wall()
: Obstacle(){
	orientation='u';
}
/*
Wall::Wall(Entity * f) : Obstacle(f){
}
*/
Wall::Wall(const Entity& e)
: Obstacle(e){}

Wall::Wall(vec3 translate, vec3 scale, char o){
	float tmp;
	switch(o){
		case 'x':
			tmp = scale.z;
			scale.z = scale.x;
			scale.x = tmp;
		case 'y':
			tmp = scale.z;
			scale.z = scale.y;
			scale.y = tmp;
		case 'z':
			break;
	}
	orientation=o;
	mCollider = Collider(BoundBox(scale));
	mCollider.addSub(BoundRect(vec2(scale)));
	mCollider.scale(vec3(1,1,1000));//this is to give the walls some back thickness
	this->translate(translate);
}

Wall::~Wall(){
	//NYI
}

void Wall::update(){
	//NYI
}
