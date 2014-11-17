#include "Util.h"
#include "Closet.h"
#include <glm/gtx/transform.hpp>

//The constructor, which ensures that the cube and drawables exist by default
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

	//ha ha ha
	drFnMap.clear();
	drFnMap["rig"] = initRigFromSVG; //this one's a problem...
	drFnMap["png"] = initTexQuad;
	drFnMap["svg"] = initPolyFromSVG;
	drFnMap["sprt"] = initSpriteSheet;
	drFnMap["obj"] = initObj;
}

//Create a Skeleton(hierarchy of Ligaments) given an XML Element
Skeleton Closet::createSkeleton(TiXmlElement * skeleton, JShader& shader){
	TiXmlElement * root = skeleton->FirstChildElement("drawable");
   if (root){
		//See Skeleton for more information about the data structure
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

//Recursively add elements to the skeleton in a depth first fashion
int Closet::fill(vector<Ligament>& lVec, unordered_map<string, int>& nameMap, TiXmlElement * el, JShader& shader){
   string name = el->Attribute("name");
	//If the name already exists, a cycle may ensue
	if (nameMap.find(name) != nameMap.end()){
		cout << "Cycle created in Ligament map: " << name << " already exists. \n Segfault Imminent." << endl;
		return 0;
	}

	//Find the newest Ligement's index, insert it into nameMap, construct Ligament
	int curIdx = lVec.size();
	nameMap[name]=curIdx;
	lVec.push_back(createLigament(el, shader));

	//Add all of this Ligament's children
	for (TiXmlElement * i = el->FirstChildElement("drawable"); i; i=i->NextSiblingElement("drawable"))
		lVec[curIdx].addChild(fill(lVec, nameMap, i, shader) - curIdx);

	//return the ligament's index
	return curIdx;
}

//Create a ligament given an XML element
Ligament Closet::createLigament(TiXmlElement * el, JShader& shader){
	int shift(0); //if a rigged ligament's cycle needs to be shifted
	vec3 scale(1), translate;
	vec4 rotate(0,0,0,1);
	
	//Get each ligament's resource (similar to what's done in Soup::createEntity)
	string fileName(el->Attribute("fileName"));
	//Transform info
	if (el->Attribute("T"))
		fillVec(translate,el->Attribute("T"));
	if (el->Attribute("R"))
		fillVec(rotate,el->Attribute("R"));
	if (el->Attribute("S"))
		fillVec(scale,el->Attribute("S"));
	if (el->Attribute("shift"))
      stringstream(string(el->Attribute("shift"))) >> shift;

	string resName(fileName.substr(0,fileName.length()-4));
	string extension(fileName.substr(fileName.length()-3,fileName.length()));
	//If we haven't already created this drawable, create it
	if (dMapPtr->find(resName) == dMapPtr->end()){
		if (drFnMap.find(extension) != drFnMap.end()) //MADNESS
			(*dMapPtr)[resName]=move(unique_ptr<Drawable>( extension == "rig" ?
					new Rig(initRigFromSVG(fileName,shader)) :
					new Drawable(drFnMap[extension](fileName, shader))
				));
		else (*dMapPtr)[resName]=move(unique_ptr<Drawable>(new Drawable(initQuad(shader))));
	}
	
	//Construct the Ligament
	Ligament L = Ligament((*dMapPtr)[resName].get(), resName);
	if (shift) //shift if necessary
		L.shift();

	//Apply local (object) transform
	L.leftMultMV(glm::translate(translate)*glm::scale(scale));
	QuatVec qv(L.getOrigin(), getRQ(rotate), QV_TRT);
	L.leftMultMV(qv.toMat4());	

	return L;
}
