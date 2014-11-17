#include <Obstacle.h>

Obstacle::Obstacle(){
	colorOsc = 0.f;
	A = 50000.f;	
	sigmaSq = 200.f;
	sigmaSq *= sigmaSq;
}

Obstacle::Obstacle(const Entity& e)
: Entity (e){
   colorOsc = 0.f;
   A = 50000.f;
   sigmaSq = 200.f;
   sigmaSq *= sigmaSq;
}
/*
Obstacle::Obstacle(Entity * f) : Entity(f){
	colorOsc = 0.f;
	A = 50000.f;	
	sigmaSq = 200.f;
	sigmaSq *= sigmaSq;
	cout << "how many times" << endl;
}
*/
Obstacle::Obstacle(vec3 translate, vec3 scale)
: Entity(translate,scale){
	colorOsc = 0.f;
	A = 50000.f;	
	sigmaSq = 200.f;
	sigmaSq *= sigmaSq;
}
void Obstacle::update(){
/* //DISCO
	float color = 0.5f + 0.5f*sin(colorOsc);
	colorOsc += 0.05f;
	mDrawables[0].get()->setColor(vec3(color, 1.f-color, 0.5f*color));
	mSkel.setColor(vec3(color, 1.f-color, 0.5f*color));
*/
}
