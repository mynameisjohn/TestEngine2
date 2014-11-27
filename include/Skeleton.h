#ifndef SKELETON_H
#define SKELETON_H

#include <map>
#include <memory>
#include <unordered_map>

#include <Drawable.h>
#include <Ligament.h>

class Skeleton{
public:
	Skeleton();
//	Skeleton(map<string, Ligament>, string r, mat4 mv=mat4(), QuatVec qv=QuatVec());
	Skeleton(Drawable * r);
	Skeleton(unordered_map<string,uint32_t> nM, vector<Ligament> lV);
	void print();
	void addToRoot(string, Ligament l);
	void draw(vec3 pos);
	void draw(mat4 transform);
	void update();
	void reflect();
	void leftMultMV(mat4 m);
	void scale(float s);
	void applyTransform(QuatVec qv);
	void resetTransform();
	void resetLigaments();
	void setColor(vec4 c);
	void setColor(vec3 v);
	void setColor(float r, float g, float b, float a=1.f);
	bool flipped();
	float getScale();
	QuatVec getTransform();
	mat4 getTransformAsMat4();
	Ligament * getRoot();
	Ligament * operator[](string idx);
	vec3 getOrigin(string s="", uint32_t idx=0);
private:
	//map<string, Ligament> lMap;
	//Ligament * root;
	//map<string, Ligament>::iterator itRoot;
	mat4 MV;
	vec4 mColor;
	list<QuatVec> mTransform;
	//string R;
	unordered_map<string, uint32_t> nameMap;
	vector<Ligament> lVec;
	float S;
	bool flip;
};

#endif
