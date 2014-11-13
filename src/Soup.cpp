#include "Soup.h"
#include "Textures.h"
#include "Closet.h"
#include <glm/gtx/transform.hpp> 

#include "Projectile.h"

Entity loadEntity(TiXmlElement * el, unordered_map<string, unique_ptr<Drawable> > * dMapPtr, JShader& shader);
Collider getCollider(TiXmlElement * collider);

unique_ptr<Population> Soup::createPopulation(string levelFile, JShader& shader){
	unique_ptr<Population> pop(nullptr);
	vector<Obstacle> obsVec;
	vector<Seeker> seekVec;

	TiXmlDocument doc(levelFile.c_str());

   if (!doc.LoadFile()){
      printError(levelFile);
      return pop;
   }

	TiXmlHandle h(&doc);
   TiXmlNode * level;
   level = h.FirstChild("Level").ToNode();

	bool playerExists(false);
	Player player;
	TiXmlNode * n(nullptr);
	while ((n = level->IterateChildren(n))){
		string type(n->Value());
		char mode(type[type.length()-1]);

		if (mode == 's'){
			//load entities code
			cout << "multiple entities" << endl;
		}
		else{
			TiXmlElement * el = n->ToElement();
			if (!el)
				break;
			Entity e(loadEntity(el, dMapPtr, shader));
			if (type=="Obstacle")
				obsVec.emplace_back(e);
			if (type=="Seeker"){
				seekVec.emplace_back(e);
				float h;
				stringstream(el->Attribute("health")) >> h;
				cout << h << endl;
				seekVec.back().setHealth(h);
			}
			if (type == "Player"){
				player = Player(e);
				float h;
				stringstream(el->Attribute("health")) >> h;
				cout << h << endl;
				player.setHealth(h);
				if (el->FirstChildElement("Projectile")){
					TiXmlElement * projEl = el->FirstChildElement("Projectile");
					player.setProjectile(Projectile(loadEntity(projEl, dMapPtr, shader)));
				}
				playerExists = true;
			}
		}
	}
	if (!playerExists){
		cout << "No player in population for level " << levelFile << ". Problems inbound." << endl;
		printError(levelFile);
	}

	pop = unique_ptr<Population>(new Population(player, obsVec, seekVec));
	return pop;
}

Entity loadEntity(TiXmlElement * el, unordered_map<string, unique_ptr<Drawable> > * dMapPtr, JShader& shader){
	Skeleton s;
	Collider col;

	Closet c(dMapPtr, &shader);
	bool collider_dbg(false);
	vec3 translate, scale;
	vec4 rotate, color(1);
	fquat rotQuat;
	float S;

	fillVec(rotate, el->Attribute("R"));
	fillVec(translate, el->Attribute("T"));
	fillVec(scale, el->Attribute("S"));
	rotQuat = (getRQ(rotate));
	fillVec(color, string(el->Attribute("color")));
	mat4 MV = glm::mat4_cast(rotQuat);
/*
	cout << translate << "\n" <<
	rotate << "\n" <<
	scale << "\n" <<
	color << "\n" << endl;
*/
	//We've already added these three, hopefully
	string dType(el->Attribute("dType")), name;
	if (dType == "cube" || dType == "quad" || dType == "texQuad"){
		Drawable * drPtr = (*dMapPtr)[dType].get();
		if (dType == "texQuad"){
			if (el->Attribute("resource")){
				name = string(el->Attribute("resource"));
				if (name == "outline")
					drPtr->addTex(name, outlineTexture());
				else
					drPtr->addTex(name, fromImage("res/img/"+name));
			}
		}
		s = Skeleton(drPtr);
		s.setColor(color);
		if ((el->Attribute("resource") && dType == "texQuad"))
			s.getRoot()->setCurTex(name);

		BoundBox bb = BoundBox(scale);
		col = Collider(bb);
		col.addSub({vec2(col.getDim())});

		S = max(glm::abs(scale));
		s.scale(S);
		scale /= S;
		Ligament * l(s.getRoot());
		MV = QuatVec(l->getOrigin(),rotQuat,QV_TRT).toMat4()*glm::scale(scale);
		l->leftMultMV(MV);
		s.setColor(color);

		col.translate(translate);
	}
	else if (dType == "obj"){
		name = string(el->Attribute("resource"));
		if (dMapPtr->find(name) == dMapPtr->end())
			(*dMapPtr)[name] = move(unique_ptr<Drawable>(new Drawable(initObj(name, shader))));
		Drawable * drPtr((*dMapPtr)[name].get());
		s = Skeleton(drPtr);
      s.setColor(color);
		BoundBox bb = BoundBox(scale);
      col = Collider(bb);
      col.addSub({vec2(col.getDim())});

      S = max(glm::abs(scale));
      s.scale(S);
      scale /= S;
      Ligament * l(s.getRoot());
      MV = QuatVec(l->getOrigin(),rotQuat,QV_TRT).toMat4()*glm::scale(scale);
      l->leftMultMV(MV);
      s.setColor(color);

      col.translate(translate);
	}
	else{
		//otherwise let's hope we got a skeleton
		name = string(el->Attribute("resource"));
		string fileName(string("res/entities/")+name);
		TiXmlDocument doc(fileName);
		if (!doc.LoadFile()){
			printError(fileName);
			return Entity();
		}

		TiXmlHandle h(&doc);
		TiXmlElement * entity(0), * skeleton(0), * collider(0);
		entity = h.FirstChild("Entity").ToElement();
		skeleton = entity->FirstChildElement("Skeleton");
		collider = entity->FirstChildElement("Collider");

		if (skeleton && collider){
			col = Collider(getCollider(collider));
			col.scale(glm::abs(scale));
			col.translate(translate);

			s = Skeleton(c.createSkeleton(skeleton, shader));
			S = max(glm::abs(scale));
			s.scale(S);
			s.leftMultMV(glm::translate(vec3(0,0,-scale.z/S)));
			s.setColor(color);
		}
	}
	//in the future it'll be best to make a separate entity for collider boxes, tied to this one somehow
	if (el->Attribute("dbg")){
		if (!(stringstream(el->Attribute("dbg")) >> collider_dbg))
			collider_dbg = false;

		//strange, I know, but the hierarchy proved too annoying to deal with for collider boxes
		if (collider_dbg){
			vec3 cScale(col.getDim()/S);
			cScale.z *= -1;
			mat4 c_MV(glm::translate(vec3(0,0,0.05f))*glm::scale(cScale));

			vector<BoundRect> subs(col.getSubs());
			for (uint32_t i=0;i<subs.size();i++){
				Ligament L_sub((*dMapPtr)["quad"].get());
				vec2 dim(subs[i].getDim()/S);
				vec2 pos((subs[i].getPos()-vec2(col.getPos()))/S);
				mat4 r_MV(glm::translate(vec3(pos,0))*glm::scale(vec3(dim,1)));
				L_sub.leftMultMV(r_MV);
				L_sub.setColor(vec4(.6,.5,.4,.7));
				s.addToRoot("box_"+to_string(i),L_sub);
			}

			Ligament L((*dMapPtr)["cube"].get());
			L.leftMultMV(c_MV);
			L.setColor(vec4(.4,.5,.6,.7));
			s.addToRoot("collider",L);
		}
	}

	return Entity(col,move(s));
}


Collider getCollider(TiXmlElement * collider){
	string d=",";
	BoundBox bb;
	float soft;
	vector<BoundRect> recVec;

	stringstream(collider->Attribute("soft")) >> soft;
	cout << soft << endl;
	for (TiXmlElement * box=collider->FirstChildElement("box"); box; box=box->NextSiblingElement("box")){
		vec3 dim, tr;
		string in = box->Attribute("S");
		size_t pos=0;
		for (int i=0;i<3;i++){
			pos=in.find(d);
			stringstream(in.substr(0,pos)) >> dim[i];
			in.erase(0,pos+d.length());
		}
		fillVec(tr,box->Attribute("T"));
		cout << tr << endl;
		bb = BoundBox(tr,dim);
		for (TiXmlElement  * rect=box->FirstChildElement("rect"); rect; rect=rect->NextSiblingElement("rect")){
			vec2 rDim, rTr;
			fillVec(rDim,rect->Attribute("S"));
			fillVec(rTr, rect->Attribute("T"));
			recVec.emplace_back(rTr,rDim);
		}
	}

	Collider c(bb, soft);
	for (vector<BoundRect>::iterator it=recVec.begin(); it!=recVec.end(); it++)
		c.addSub(*it);

	return c;
}
