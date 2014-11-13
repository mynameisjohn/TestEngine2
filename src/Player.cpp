#include <SDL2/SDL.h>
#include "Player.h"
#include <glm/gtx/transform.hpp>
//#include <algorithm>
const unsigned int STAND  = 0;
const unsigned int WALK   = 1;
const unsigned int RUN    = 2;
const unsigned int JUMP   = 3;

const vec3 speed(40.f, 40.f, 20.f);

//this is a good number to center things around
const vec3 offset(100,100,0);

Player::Player() : ActiveEnt(), jumping(false), activeProj(0){
	mSpeed = speed;//vec3(40.f, 30.f, 10.f);
	mDash = 1.5f;
	A = 10000.f;	
	sigmaSq = 5000.f;
	sigmaSq *= sigmaSq;
}

Player::Player(const Entity& e)
: ActiveEnt(e), jumping(false), activeProj(0){
   mSpeed = speed;///vec3(40.f, 30.f, 10.f);
   mDash = 1.5f;
   A = 10000.f;
   sigmaSq = 5000.f;
   sigmaSq *= sigmaSq;
   projList.clear();

	//this crap
	mSkel["body"]->set_cycle("stand");
	mSkel["leg1"]->set_cycle("stand");
	mSkel["leg2"]->set_cycle("stand");
	mSkel["arm1"]->set_cycle("stand");
	mSkel["arm2"]->set_cycle("stand");
	mSkel["forearm1"]->set_cycle("stand");
	mSkel["forearm2"]->set_cycle("stand");
}

Player::Player(vec3 translate, vec3 scale)
: ActiveEnt(translate, scale), jumping(false), activeProj(0){
	mSpeed = speed;//vec3(20.f, 40.f, 20.12241f);
	mDash=1.5f;
	A = 10000.f;	
	sigmaSq = 5000.f;
	sigmaSq *= sigmaSq;
}

Player::~Player(){
	//NYI
}

char Player::moveWRT_ent(Entity * e){
	char ret=ActiveEnt::moveWRT_ent(e);
	list<Projectile>::iterator pIt;
	
	for (pIt=projList.begin(); pIt!=projList.end(); pIt++){
		if (pIt->isActive())
			if (pIt->moveWRT_ent(e)){
			pIt->kill();
			pIt = projList.erase(pIt);
		}
	}
	return ret;
}

void Player::move(){
	ActiveEnt::move();

	list<Projectile>::iterator pIt;
	for (pIt=projList.begin();pIt!=projList.end();pIt++){
		if (pIt->isPoised())
			pIt->moveTo(getOrigin("arm1")-pIt->mSkel.getOrigin()+vec3(0,0,pIt->getSkeleton()->getScale()/5));
		else if (pIt->isActive())
			pIt->move();
	}
}

void Player::update(){//EventInfo evInfo){
	if (grounded){
		mVel.y=0.f;
		eReg.worldMouse.x += mVel.x;
	}
	else{
		eReg.worldMouse += vec2(mVel);
		mVel.y -= 2.f;//gravity?
	}

	mSkel.resetTransform();//Ligaments();	
	
	updateSkeleton();
	updateKeys();
	updateMouse();

/*
	static float th(0.f);
	mSkel["bow"]->applyTransform(getRQ(vec4(th,vec3(0,0,1))));
	th += 1.f;
*/
	list<Projectile>::iterator pIt;
	for (pIt=projList.begin();pIt!=projList.end();pIt++){
		if (pIt->isActive())
			pIt->update();
		else if (!pIt->isAlive())
			pIt = projList.erase(pIt);
	}
}

void Player::takeHit(HitEffect hEff){
	mSkel.setColor(1,0,0,1);
}

void Player::updateMouse(){
//	vec2 worldMouse = eReg.getWorldMouse();

	vec2 dir = eReg.worldMouse - vec2(center());
	bool right = dir.x > 0;

	//this is about as good as it'll get for now
   if ((!right && !mSkel.flipped()) || (right && mSkel.flipped()))
		reflect();

	//this is gonna need some work
	vec2 launchOffset( (mSkel.flipped() ? -2 : 1) * 100,250);

	//make non static
	static float charge(0.f);
	static bool charging(false), notched(false);
	const float dCharge(0.05f), chargeMax(1), notch(0.5);

	//A NOTE ON REFLECTION:
	//a fair armount of stuff has to be accounted for when the model is reflected about the y axis
	//WRT mouse rotation x direction. I am leaving that off for now, though it is very annoying. 
	if (eReg.lmb){
		if (charging){
			const float armDraw_m(420), armDraw_M(200);
			const fquat r90(.707f, .707f*vec3(0,0,1));
			const vec3 armOffset(-.035,.035,0);

			charge = min(chargeMax, charge+dCharge);
			vec2 r(glm::normalize(eReg.worldMouse - vec2(getOrigin("arm1"))));//mSkel.getOrigin("arm1"))));
			vec2 armDif(mSkel["arm1"]->getOrigin()-mSkel["arm2"]->getOrigin());

			fquat mouseRot(r.x>0?getQuatFromVec2(r):getQuatFromVec2(r*vec2(-1,1))), armRot(cos(charge),sin(charge)*vec3(-1,0,0)), arrowRot(getQuatFromVec2(r));
//			float aDo(1.5*mProj.getSkeleton()->getScale()), arrowDraw_M(-mProj.getSkeleton()->getScale());//*(1-charge));

			mSkel["arm1"]->setState(L_ACTIVE);
			mSkel["arm1"]->applyTransform({armOffset,r90*mouseRot*armRot});
			
			mSkel["arm2"]->setState(L_ACTIVE);
			mSkel["arm2"]->applyTransform({vec3(armDif,0), r90*mouseRot});
			
			mSkel["forearm1"]->setCurTex("forearm_draw");
			mSkel["forearm1"]->applyTransform(glm::conjugate(armRot));

			if (charge > notch){
				if (!notched){
					notched=true;
					projList.emplace_back(mProj);//push_back(std::move(mProj));
					projList.back().setState(1);
				}
				projList.back().getSkeleton()->getRoot()->applyTransform(arrowRot);
				projList.back().translate(lerp(armDraw_m, armDraw_M,charge)*vec3(r,0));
			}
		}
		else
			charging = true;
	}
	else if (charging){
		projList.back().setState(2);
		vec2 launchDir = eReg.worldMouse - vec2(offset + center());
		float speed(lerp(50, 200, charge));
		projList.back().launch(launchDir, speed);
		charge = 0.f;
		charging = notched = false;
		mSkel["forearm1"]->setCurTex("forearm");
		mSkel["arm1"]->setState(L_CYCLIC);
		mSkel["arm2"]->setState(L_CYCLIC);
	}
}

void Player::updateSkeleton(){
	//I'm convinced I'll need this one day
}

void Player::draw(){
	Entity::draw();

	list<Projectile>::iterator pIt;
	for (pIt=projList.begin(); pIt!=projList.end(); pIt++)
		pIt->draw();
}

bool Player::overlapsWith(Entity * e){
	list<Projectile>::iterator pIt;
	for (pIt=projList.begin(); pIt!=projList.end(); pIt++){
		if (pIt->isActive()){
			if (pIt->overlapsWith(e)){
				cout << "Projectile Hit" << endl;
				//pIt->kill();
				//pIt = projList.erase(pIt);
			}
		}
	}

	return Entity::overlapsWith(e);
}

//this is a mess
void Player::updateKeys(){
// uncomment to prohibit mid-air movement
//	if (mCollider.isGrounded){
	bool jump = eReg.getKeyState(SDLK_SPACE);
	bool dash = eReg.getKeyState(SDLK_LSHIFT);
	float dashSpeed = mSpeed.x*(dash ? mDash : 1.f);
	
	mVel.x=0;
	if (eReg.getKeyState(SDLK_a))
		mVel.x -= dashSpeed;
	if (eReg.getKeyState(SDLK_d))
		mVel.x += dashSpeed;
	
	mVel.y += jump && grounded ? mSpeed.y : 0;
	
	mVel.z=0;
	if (eReg.getKeyState(SDLK_w))
		mVel.z -= mSpeed.z;
	if (eReg.getKeyState(SDLK_s))
		mVel.z += mSpeed.z;
//	}

	//Handle the pose changing here
	string to;
	bool moving = (eReg.getKeyState(SDLK_w) || 
					   eReg.getKeyState(SDLK_a) || 
					   eReg.getKeyState(SDLK_s) || 
					   eReg.getKeyState(SDLK_d));

//This whole moving to jump thing's all messed up

	if (moving){
		if (dash){
			to = "run";//RUN;
//			mSkel["arm1"]->setCurTex("arm_bent");
//			mSkel["arm2"]->setCurTex("arm_bent");
		}
		else
			to = "walk";//WALK;
	}
	else
		to = "stand";//STAND;

	if (jump) mSkel["leg1"]->set_to("jump", L_ONCE);
	else mSkel["leg1"]->set_to(to);
	if (jump) mSkel["leg2"]->set_to("jump", L_ONCE);
	else mSkel["leg2"]->set_to(to);
	mSkel["arm1"]->set_to(to);
	mSkel["arm2"]->set_to(to);
	mSkel["forearm1"]->set_to(to);
	mSkel["forearm2"]->set_to(to);
	mSkel["body"]->set_to(to);

/*
	if (grounded){
		if (jumping){
			((Rig *)(mSkel["leg1"]))->shift();
			jumping = false;
		}
		else if (jump){
	      ((Rig *)(mSkel["leg1"]))->set_pose(2);
	      ((Rig *)(mSkel["leg2"]))->set_pose(2);
			to = JUMP;
			jumping = true;
		}
	}
*/
/*
	//Automate this for all rigs in skeleton
	r = (Rig *)mSkel["leg1"];
	r->set_to(to);

	r = (Rig *)mSkel["leg2"];
	r->set_to(to);

	r = (Rig *)mSkel["arm1"];
	r->set_to(to);

	if (dash){
		r->setCurTex(1);
		r = (Rig *)mSkel["arm2"];
		r->setCurTex(1);
		r = (Rig *)mSkel["forearm1"];
		r->setVisibility(true);
		r = (Rig *)mSkel["forearm2"];
		r->setVisibility(true);
	}
	else{
		r->setCurTex(0);
		r = (Rig *)mSkel["arm2"];
		r->setCurTex(0);
		r = (Rig *)mSkel["forearm1"];
		r->setVisibility(false);
		r = (Rig *)mSkel["forearm2"];
		r->setVisibility(false);
	}

	r = (Rig *)mSkel["arm2"];
	r->set_to(to);
	
	r = (Rig *)mSkel["body"];
	r->set_to(to);
*/
/*
	Rig * r = (Rig *)mSkel["leg1"];
	float c;
	if (eReg.getKeyState(SDLK_a) || eReg.getKeyState(SDLK_d) || eReg.getKeyState(SDLK_s) || eReg.getKeyState(SDLK_w)){
		if (eReg.getKeyState(SDLK_LSHIFT))
			c=1.f;//r->set_u({1,mod});
		else
			c=0.f;//r->set_u({0,mod});
	}
	else
		c=-1.f;//r->set_u({-1,mod});
	r->inc_u(c);

	r = (Rig *)mSkel["leg2"];
	r->inc_u(c);
	r = (Rig *)mSkel["arm1"];
	r->inc_u(c);
	r = (Rig *)mSkel["arm2"];
	r->inc_u(c);
*/
}

void Player::setProjectile(Projectile p){
	mProj = Projectile(p);
}

//Why do they have to be unique_ptrs? I may have goofed
void Player::addProjectile(Projectile p){
//	unique_ptr<Projectile> q(new Projectile(p));
//	mProjectiles.push_back(std::move(q));

	projList.emplace_back(p);
	//projList.back().mSkel.print();
/*
	cout << "Projectile " << mProjectiles.size() << ": ";
	mProjectiles.back()->mSkel.print();
	cout << endl;
*/
//	Projectile P(p);
//	mProjectiles.push_back(std::move(P));
}

/*
void Player::addProjectile(const Projectile p){
	unique_ptr<Projectile> q(new Projectile());

	mProjectiles.emplace_back(q.get());

//	mProjectiles.push_back(std::move(p));//std::move(p));
}
*/

EventRegister * Player::getRegPtr(){
	return &eReg;
}
