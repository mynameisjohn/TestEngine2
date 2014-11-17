#include <Ligament.h>
#include <Util.h>
#include <Rig.h>

#include <typeinfo>
#include <glm/gtx/transform.hpp>

Ligament::Ligament(){
	resetTransform();
}

//To Do: change constructor so that the QuatVec list starts off empty,
//since any initial transform goes in MV. yuck
Ligament::Ligament(Drawable * dPtr, string ctex)
: mDrawable(dPtr), cTex(ctex), from(""), to(""), active(false), shifted(false), state(L_CYCLIC){
   mTransform.clear();

	origins.clear();
	if (Rig * r = dynamic_cast<Rig *>(mDrawable)){
		origins = r->getOrigins();
		from = to = r->getFirstCycle();
	}
	else
		origins.push_back(mDrawable->getOrigin());
}

void Ligament::setState(int state){
	this->state = state;
}

void Ligament::setColor(vec4 c){
	mColor = c;
}
int Ligament::addChild(int offset, bool invert){
	//in future check if already exists
	children.push_back(offset);

	return children.back();
}

void Ligament::set_cycle(string s){
	to = from = s;
}

void Ligament::resetTransform(){
	mTransform.clear();//resize(1);
}

QuatVec Ligament::getTransform(){
	QuatVec transform(mTransform.front());
	list<QuatVec>::iterator it = mTransform.begin();
	for (it++; it!=mTransform.end(); it++)
		transform = *it * transform;

	return transform;
}

mat4 Ligament::getTransformAsMat4(){
 // i don't like this
	mat4 ret;
	bool s(false);
	QuatVec transform(mTransform.front());
	list<QuatVec>::iterator it = mTransform.begin();

	for (it++; it!=mTransform.end(); it++){
		if (*it & transform)
			transform = *it * transform;
		else{
			s=true;
			ret = transform.toMat4();
			break;
		}
	}

	if (s){
		for (; it!=mTransform.end(); it++)
			ret = ret * it->toMat4();
	}
	else return transform.toMat4();

	return ret;
}

void Ligament::update(){
	const float eps = 0.001f;
   const float dt = 0.1f;

	//active = false;
	if (state == L_ONCE){
		if (Rig * r = dynamic_cast<Rig *>(mDrawable)){
			float length(r->getShift()*2);
			if (u.y > length){
				state = L_CYCLIC;
				from = to = old;
				if (shifted)
					shift();
			}
		}
		return;
	}
	

   if (1.f-u.x < eps){
      from = to;
      u.x=0.f;
		if (state == L_ACTIVE)
			active=true;
   }

	if (from != to || state == L_ACTIVE)
		u.x += dt;
	else
		active = false;
}

void Ligament::draw(glm::mat4 parent){
	//upload color here, for debugging purposes
	mDrawable->uploadColor(mColor);

	if (Rig * r = dynamic_cast<Rig *>(mDrawable)){
		switch(state){
			case L_ONCE:
			case L_CYCLIC:{
				//get current pose
				Pose p(r->getCurrentPose(from,to,u));
				//get first joint's QV, plus whatever transform this ligament has
				QuatVec qv(mTransform.empty() ? p.joints[0] : getTransform()*p.joints[0]);
				//Translate to origin, rotate, translate out, then translate this ligament's transform
				mat4 m(glm::translate(qv.trans+vec3(origins[0]))*
						 glm::mat4_cast(qv.rot)*
						 glm::translate(-vec3(origins[0])));

				//this all seems rather expensive
				vector<mat4> R(origins.size());
				R[0] = m;//parent * m * MV;
	
				for (uint32_t i=1; i<R.size(); i++){
					R[i] = R[i-1]*
								  glm::translate(vec3(origins[i])+p.joints[i].trans)*
								  glm::mat4_cast(p.joints[i].rot)*
								  glm::translate(-vec3(origins[i]));
				}
				R.back() = parent*R.back();
				for (vector<int>::iterator it = children.begin(); it!=children.end(); it++)
					this[*it].draw(R.back());
				R.back() = R.back()*MV;
	
				for (uint32_t i=0; i<R.size()-1; i++)
					R[i]=parent*R[i]*MV;

				r->uploadRigMats(R);
				r->draw(cTex,true);

				update();
				break;
			}
			case L_ACTIVE:{
				if (active){
					if (!mTransform.empty()){
						QuatVec qv(getTransform());
						parent = parent * glm::translate(qv.trans+vec3(origins[0]))*
												glm::mat4_cast(qv.rot)*
												glm::translate(-vec3(origins[0]));
					}
					mat4 m = parent*MV;
					r->uploadMV(m);
					r->draw(cTex, false);
					for (vector<int>::iterator it = children.begin(); it!=children.end(); it++)
						this[*it].draw(parent);
					break;
				}
				Pose p(r->getCurrentPose(from,from,u));
				vector<QuatVec> qvVec(p.getNumJoints());
				qvVec[0] = getTransform();
				Pose q(qvVec);
				p = p.blend(q, u.x);
				
				vector<mat4> R(origins.size());
				R[0] = glm::translate(p.joints[0].trans+vec3(origins[0]))*
						 glm::mat4_cast(p.joints[0].rot)*
						 glm::translate(-vec3(origins[0]));

				//draw children?
				for (uint32_t i=1; i<R.size(); i++)
               R[i] = R[i-1]*
                          glm::translate(vec3(origins[i])+p.joints[i].trans)*
                          glm::mat4_cast(p.joints[i].rot)*
                          glm::translate(-vec3(origins[i]));

				R.back() = parent*R.back();
				for (vector<int>::iterator it = children.begin(); it!=children.end(); it++)
					this[*it].draw(R.back());
				R.back() = R.back()*MV;
	
				for (uint32_t i=0; i<R.size()-1; i++)
					R[i]=parent*R[i]*MV;

				r->uploadRigMats(R);
				r->draw(cTex,true);

				update();
				break;
			}
			default:
				break;
		}
	}
	else{
		if (!mTransform.empty()){
//			parent = parent*getTransformAsMat4();
			QuatVec qv(getTransform());
			parent = parent * glm::translate(qv.trans+vec3(origins[0]))*
									glm::mat4_cast(qv.rot)*
									glm::translate(-vec3(origins[0]));
		}
		mat4 m = parent * MV;
		mDrawable->uploadMV(m);
		mDrawable->draw(cTex);
		for (vector<int>::iterator it = children.begin(); it!=children.end(); it++)
			this[*it].draw(parent);
	}


/*
	if (!mTransform.empty())
		parent=parent*getTransformAsMat4();
	mat4 m = parent*MV;
	mDrawable->uploadMV(m);
*/
/*
	parent = mTransform.empty() ? parent * MV : parent * getTransformAsMat4() * MV;
	mDrawable->uploadMV(parent * );
*/
/*
	if (Rig * r = dynamic_cast<Rig *>(mDrawable)){
		if (active)
				r->draw(cTex);
		else{
			mat4 back = r->draw(mat4(), u, from, to, cTex);
			back = glm::translate(vec3(origin))*back*glm::translate(-vec3(origin));
			parent = parent*back;//parent*r->draw(mat4(), u, from, to, cTex);
			update();
		}
	}
	else
		mDrawable->draw(cTex);//, color);//curTex);
//		parent = mDrawable->draw(parent, curTex);
*/
/*
	//lambda
	for_each(children.begin(), children.end(), [&](int idx){
		this[idx].draw(parent);
	});
*/
/*
	for (vector<int>::iterator it = children.begin(); it!=children.end(); it++)
		this[*it].draw(parent);
*/
}

bool Ligament::isActive(){
	return active;
}

void Ligament::setCurTex(string ct){
	cTex = ct;
}

vec2 Ligament::get_u(){
	return u;
}
/*
void Ligament::setCurTex(unsigned int ct){
	curTex = ct;
}
*/
void Ligament::set_u(vec2 u){
	this->u = u;
}

void Ligament::set_to(string t, int s){//unsigned int t){
	if (typeid(*mDrawable) == typeid(Rig)){
		if (s==L_CYCLIC)
			to = t;
		if (s==L_ONCE && state != L_ONCE){
			old = to;
			from = to = t;
			state = L_ONCE;
			u.y=0;
		}
	}
}

void Ligament::setActive(bool b){
	active = b;
}

void Ligament::shift(){
	if (Rig * r = dynamic_cast<Rig *>(mDrawable))
		u.y += r->getShift();
	shifted = true;
}

void Ligament::applyTransform(QuatVec qv){
	mTransform.push_back(qv);
}

void Ligament::applyTransform(vec3 t){
	mTransform.emplace_back(t,fquat());
}

void Ligament::applyTransform(fquat q){
	mTransform.emplace_back(vec3(),q);
}

void Ligament::leftMultMV(mat4 m){
	MV = m*MV;
	origins.clear();
	if (Rig * r = dynamic_cast<Rig *>(mDrawable))
		origins = r->getOrigins(MV);
	else
		origins.push_back(mDrawable->getOrigin(MV));
}

bool Ligament::hasChild(Ligament * l){
	for (vector<int>::iterator it = l->children.begin(); it!=l->children.end(); it++){
		
	}
/*	if (children.size() == 0)
		return false;
	
	vector<Ligament *>::iterator childIt;
	for (childIt=children.begin(); childIt!=children.end(); childIt++)
		if (l == (*childIt))
			return true;
		
	for (childIt=children.begin(); childIt!=children.end(); childIt++)
		return (*childIt)->hasChild(l);
*/
	return false;
}

vec3 Ligament::getOrigin(uint32_t idx){
	return vec3(origins[idx > origins.size()-1 ? origins.size()-1 : idx]);
}

mat4 Ligament::getMV(){
	return MV;
}

Drawable * Ligament::getDrPtr(){
	return mDrawable;
}
