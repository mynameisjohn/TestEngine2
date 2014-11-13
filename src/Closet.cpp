#include "Util.h"
#include "Closet.h"
#include <glm/gtx/transform.hpp>

Closet::Closet(unordered_map<string, unique_ptr<Drawable> > * dMP, JShader * shader)
:dMapPtr(dMP){
	if (dMapPtr->find("cube") == dMapPtr->end()){
      unique_ptr<Drawable> drPtr(new Drawable(initCube(*shader)));
      (*dMapPtr)["cube"] = move(drPtr);
   }

	if (dMapPtr->find("quad") == dMapPtr->end()){
      unique_ptr<Drawable> drPtr(new Drawable(initQuad(*shader)));
      (*dMapPtr)["quad"] = move(drPtr);
   }

	if (dMapPtr->find("texQuad") == dMapPtr->end()){
      unique_ptr<Drawable> drPtr(new Drawable(initTexQuad(*shader)));
      (*dMapPtr)["texQuad"] = move(drPtr);
   }
}

Skeleton Closet::createSkeleton(TiXmlElement * skeleton, JShader& shader){
	TiXmlElement * root = skeleton->FirstChildElement("drawable");
   if (root){
		vector<Ligament> lVec;
		unordered_map<string, int> nameMap;
		fill(lVec, nameMap, root, shader);
		Skeleton s(nameMap, lVec);

		return s;
	}
	else
		cout << "Invalid XML file" << endl;

   return Skeleton();
}

int Closet::fill(vector<Ligament>& lVec, unordered_map<string, int>& nameMap, TiXmlElement * el, JShader& shader){
   string name = el->Attribute("name");

	if (nameMap.find(name) != nameMap.end()){
		cout << "Cycle created in Ligament map: " << name << " already exists. \n Segfault Imminent." << endl;
		return 0;
	}

	int curIdx = lVec.size();
	pair<string, int> p(name, curIdx);
	nameMap[name]=curIdx;
	lVec.push_back(createLigament(el, shader));

	for (TiXmlElement * i = el->FirstChildElement("drawable"); i; i=i->NextSiblingElement("drawable"))
		lVec[curIdx].addChild(fill(lVec, nameMap, i, shader) - curIdx);

	return curIdx;
}

Ligament Closet::createLigament(TiXmlElement * el, JShader& shader){
	int shift(0);
	vec3 scale(1), translate;
	vec4 rotate(0,0,0,1);
	
	const string d(",");
	string fileName(el->Attribute("fileName"));

	if (el->Attribute("T"))
		fillVec(translate,el->Attribute("T"));
	if (el->Attribute("R"))
		fillVec(rotate,el->Attribute("R"));
	if (el->Attribute("S"))
		fillVec(scale,el->Attribute("S"));

/*
	string T(el->Attribute("T"));
	
	fillVec(translate, T);
*/
	if (el->Attribute("shift"))
      stringstream(string(el->Attribute("shift"))) >> shift;

	if (dMapPtr->find(fileName) == dMapPtr->end()){
		unique_ptr<Drawable> drPtr(nullptr);
		char type = el->Attribute("type")[0];
		switch (type){
			case 'r':
				drPtr = unique_ptr<Drawable>(new Rig(initRigFromSVG(fileName, shader)));
				cout << drPtr->getOrigins().front() << endl;
				break;
			case 't':
				drPtr = unique_ptr<Drawable>(new Drawable(initTexQuad(shader, fileName)));
				break;
			case 'p':
				drPtr = unique_ptr<Drawable>(new Drawable(initPolyFromSVG(fileName, shader)));
				break;
			case 's':
				drPtr = unique_ptr<Drawable>(new Drawable(initSpriteSheet(fileName, shader)));
				break;
			default:
				drPtr = unique_ptr<Drawable>(new Drawable(initQuad(shader)));
				break;
		}
		(*dMapPtr)[fileName] = move(drPtr);
	}

	Ligament L = Ligament((*dMapPtr)[fileName].get(), fileName);
	if (shift)
		L.shift();

	L.leftMultMV(glm::translate(translate)*glm::scale(scale));
	QuatVec qv(L.getOrigin(), getRQ(rotate), QV_TRT);
	L.leftMultMV(qv.toMat4());	

	return L;
}
