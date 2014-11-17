#include "Skeleton.h"
#include "Ligament.h"
#include <glm/gtc/type_ptr.hpp>

Skeleton::Skeleton(){
	resetTransform();
}

Skeleton::Skeleton(unordered_map<string, int> nM, vector<Ligament> lV)
: nameMap(std::move(nM)), lVec(std::move(lV)), S(1), flip(false){
	mTransform.clear();
}

Skeleton::Skeleton(Drawable * r)
: S(1), flip(false){
	nameMap.clear();
	nameMap["root"]=0;
	lVec.clear();
	lVec.emplace_back(r);
	mTransform.clear();
}

vec3 Skeleton::getOrigin(string s, int idx){
	unordered_map<string, int>::iterator it = nameMap.find(s);
	if (it != nameMap.end())
		return S*lVec[it->second].getOrigin(idx);
	else
		return S*lVec[0].getOrigin(idx);
}

void Skeleton::reflect(){
	flip = !flip;
}

void Skeleton::update(){
	vector<Ligament>::iterator lIt;
	for (lIt=lVec.begin(); lIt!=lVec.end(); lIt++)
		lIt->update();
}

bool Skeleton::flipped(){
	return flip;
}

void Skeleton::print(){
	unordered_map<string, int>::iterator it;
	for (it=nameMap.begin();it!=nameMap.end();it++)
		cout << it->first << ":\t" << "origin: " << lVec[it->second].getOrigin() << "\t Drawable: " << lVec[it->second].getDrPtr() << "\t" << "Scale: " << S << endl;
}

void Skeleton::addToRoot(string k, Ligament l, bool invert){//unique_ptr<Drawable> v){
	lVec.push_back(l);
	nameMap[k] = lVec[0].addChild(lVec.size()-1, invert);
}

void Skeleton::setColor(vec3 v){
	mColor = vec4(v,1);
	vector<Ligament>::iterator lIt;
	for (lIt=lVec.begin(); lIt!=lVec.end(); lIt++)
		lIt->setColor(mColor);
}

void Skeleton::setColor(float r, float g, float b, float a){
	mColor = vec4(r,g,b,a);
	vector<Ligament>::iterator lIt;
	for (lIt=lVec.begin(); lIt!=lVec.end(); lIt++)
		lIt->setColor(mColor);
}

//THIS IS FLAWED: IT WILL ONLY WORK IF sX IS THE 
//LARGEST DIMENSION. FIX ASAP
void Skeleton::draw(vec3 pos){
//	lVec[0].getDrPtr()->uploadColor(mColor);
	float sX(flip ? -S : S);
	if (flip) pos.x += S/2.5f;//WHY 2.5?
	lVec[0].draw(mat4({sX,0,0,0},
	/*a transposed	*/	{0,S,0,0},
	/*version of the*/{0,0,S,0},
	/*world matrix*/	vec4(pos,1)));
}

void Skeleton::draw(mat4 transform){
//	glUniform4fv(lVec[0].getDrPtr()->getColorHandle(), 1, glm::value_ptr(mColor));
//	lVec[0].getDrPtr()->uploadColor(mColor);
	lVec.front().draw(transform);//mTransform.empty() ? transform : transform * getTransformAsMat4());
}

void Skeleton::leftMultMV(mat4 m){
	vector<Ligament>::iterator it;
	for (it=lVec.begin();it!=lVec.end();it++)
		it->leftMultMV(m);
}

void Skeleton::scale(float s){
	S = s;
}

QuatVec Skeleton::getTransform(){
	QuatVec transform(mTransform.front());
   list<QuatVec>::iterator it = mTransform.begin();
   for (it++; it!=mTransform.end(); it++)
      transform = *it * transform;

	return transform;
}

mat4 Skeleton::getTransformAsMat4(){
 // I don't like this
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
	else ret = transform.toMat4();

	return ret;
}

void Skeleton::applyTransform(QuatVec qv){
	mTransform.push_back(qv);
}

void Skeleton::resetTransform(){
	mTransform.clear();//resize(1);
	resetLigaments();
}

void Skeleton::resetLigaments(){
	vector<Ligament>::iterator lIt;
	for (lIt=lVec.begin(); lIt!=lVec.end(); lIt++)
		lIt->resetTransform();
}

float Skeleton::getScale(){
	return S;
}

Ligament * Skeleton::getRoot(){
	return &(lVec.front());
}

Ligament * Skeleton::operator[](string idx){
	return &(lVec[nameMap.at(idx)]);
}

void Skeleton::setColor(vec4 c){
	mColor = c;
	vector<Ligament>::iterator lIt;
	for (lIt=lVec.begin(); lIt!=lVec.end(); lIt++)
		lIt->setColor(mColor);
//	getRoot()->setColor(vec3(c));
}
