#include <Soup.h>
#include <Textures.h>
#include <Closet.h>
#include <Projectile.h>

#include <glm/gtx/transform.hpp> 

//The reason these exist outside of the Soup namespace is because I didn't
//want Level to inherit the tinyxml library
Entity loadEntity(TiXmlElement * el, unordered_map<string, unique_ptr<Drawable> > * dMapPtr, JShader& shader);
Collider getCollider(TiXmlElement * collider);

//Retun a unique_ptr to a population given the Level File (XML)
unique_ptr<Population> Soup::createPopulation(string levelFile, JShader& shader){
	unique_ptr<Population> pop(nullptr);
	vector<Obstacle> obsVec;
	vector<Seeker> seekVec;
	bool playerExists(false);
	Player player;

	//Load up XML File, Create Handle/Pointer to Level Element
	TiXmlDocument doc(levelFile.c_str());
   if (!doc.LoadFile()){
      printError(levelFile);
      return pop;
   }
	TiXmlHandle h(&doc);
   TiXmlNode * level;
   level = h.FirstChild("Level").ToNode();
	TiXmlNode * n(nullptr);

	while ((n = level->IterateChildren(n))){
		string type(n->Value());
		char mode(type[type.length()-1]);

		//load entities code (NYI)
		if (mode == 's'){
			cout << "multiple entities" << endl;
		}
		else{//Load Individual Entities
			TiXmlElement * el = n->ToElement();
			if (!el)
				break;
			//Create Base Entity, then create specific type
			Entity e(loadEntity(el, dMapPtr, shader));
			if (type=="Obstacle")
				obsVec.emplace_back(e);
			if (type=="Seeker"){
				seekVec.emplace_back(e);
				float h;
				fillIn(h, el->Attribute("health"));
				seekVec.back().setHealth(h);
			}
			if (!playerExists && type == "Player"){
				player = Player(e);
				float h;
				fillIn(h, el->Attribute("health"));
				player.setHealth(h);
				//Make Projectile for the player (needs to be generalized)
				if (el->FirstChildElement("Projectile")){
					TiXmlElement * projEl = el->FirstChildElement("Projectile");
					player.setProjectile(Projectile(loadEntity(projEl, dMapPtr, shader)));
				}
				playerExists = true;
			}
		}
	}
	//Ensure a player was created
	if (!playerExists){
		cout << "No player in population for level " << levelFile << ". Problems inbound." << endl;
		printError(levelFile);
	}

	pop = unique_ptr<Population>(new Population(player, obsVec, seekVec));
	return move(pop);
}

//Load a generic Entity, given an XML Element containing its pertinents
Entity loadEntity(TiXmlElement * el, unordered_map<string, unique_ptr<Drawable> > * dMapPtr, JShader& shader){
	Skeleton s;
	Collider col;
	BoundBox bb;

	//Initialize the closet
	Closet c(dMapPtr, &shader);

	//Used to debug collider boxes
	bool collider_dbg(false);
	
	//Transforms
	vec3 translate, scale;
	vec4 rotate, color(1);
	fquat rotQuat;

	//Fill the transforms given the element
	fillVec(rotate, el->Attribute("R"));
	fillVec(translate, el->Attribute("T"));
	fillVec(scale, el->Attribute("S"));
	rotQuat = (getRQ(rotate));
	fillVec(color, string(el->Attribute("color")));

	//Get the element's resource (used to create drawable)
	string resource(el->Attribute("resource"));

	/*Handle various resource cases:
		quad   - solid color quad
		cube   - solid color cube
		o_cube - outline texture cube
		PNG    - a textured quad
		XML    - a hierarchical skeleton
		OBJ    - a 3d .obj file
	*/
	if (resource == "cube" || resource == "quad")
		s = Skeleton((*dMapPtr)[resource].get());
	else if (resource == "o_cube"){
		Drawable * drPtr((*dMapPtr)["cube"].get());
		drPtr->addTex("outline", outlineTexture());
		s = Skeleton(drPtr);
		s.getRoot()->setCurTex("outline");
	}
	else{ //Characterize case by file extension
		string extension(resource.substr(resource.length()-3,resource.length()));

		//An XML Resource, aka a Skeleton (hierarchical drawables)
		if (extension == "xml"){
			//get XML file name, load as TiXmlDocument
			string fileName(ENT_DIR+resource);
			TiXmlDocument doc(fileName);
			if (!doc.LoadFile()){//I should use assertions for this
				printError(fileName);
				return Entity();
			}

			//Load skeleton and collider elements
			TiXmlHandle h(&doc);
			TiXmlElement * entity(0), * skeleton(0), * collider(0);
			entity = h.FirstChild("Entity").ToElement();
			skeleton = entity->FirstChildElement("Skeleton");
			collider = entity->FirstChildElement("Collider");
	
			//if they both exist, generate the data and set collider T/S
			if (skeleton && collider){
				col = Collider(getCollider(collider));
				s = Skeleton(c.createSkeleton(skeleton, shader));
				col.translate(translate);
				col.scale(glm::abs(scale));
				//These collider transforms shouldn't be repeated, hence goto
				goto initSkeleton;
			}
			else printError(fileName);
		}

		//A PNG resource, which I take to be an image-textured quad
		else if (extension == "png"){
			//Get the image name (minus extension) and full file path
			string resName(resource.substr(0,resource.length()-4));
			string fileName(IMG_DIR+resource);
			//Get the quad drawable pointer, add the texture
			Drawable * drPtr((*dMapPtr)["quad"].get());
			drPtr->addTex(resName,fromImage(fileName));
			//Create skeleton and return
			s = Skeleton(drPtr);
			s.getRoot()->setCurTex(resName);
		}

		//An OBJ resource, taken to be a .obj file created in some 3d application
		else if (extension == "obj"){
			//Pretty much the standard thing (more goto ?=? less repetition?)
			Drawable * drPtr;
			string fileName(OBJ_DIR+resource);
			string resName(resource.substr(0,resource.length()-4));
			if (dMapPtr->find(resName) == dMapPtr->end())
				(*dMapPtr)[resName] = move(unique_ptr<Drawable>
													(new Drawable(initObj(fileName, shader))));
			drPtr = (*dMapPtr)[resName].get();
			s = Skeleton(drPtr);
			if (!drPtr->addTex(resName, fromImage(IMG_DIR+resName+".png"))){
				drPtr->addTex("outline", outlineTexture());
				resName="outline";
			}	
			s.getRoot()->setCurTex(resName);//"outline");
		}
	}

	//For non XML resources, create the Collider
	bb = BoundBox(scale);
	col = Collider(bb);
	col.addSub({vec2(col.getDim())});
	col.translate(translate);
	//This is necessary to get object scale (skeleton(world) scale is uniform)
	s.leftMultMV(glm::scale(scale/max(glm::abs(scale))));

	//initialize skeleton
	initSkeleton:
	float skelScale = max(glm::abs(scale));
	s.leftMultMV(QuatVec(s.getOrigin(), rotQuat, QV_TRT).toMat4());
	scale /= skelScale;
	s.scale(skelScale);
	s.setColor(color);

	if (el->Attribute("dbg"))
		fillIn(collider_dbg, el->Attribute("dbg"));
// >> collider_dbg;
	
	//Create some cubes and quads to illustrate the Collider's boundaries
	if (collider_dbg){
		//Get Collider Scale (for box), create Ligament
		vec3 cScale(col.getDim()/skelScale);
		cScale.z *= -1;
		mat4 c_MV(glm::translate(vec3(0,0,0.05f))*glm::scale(cScale));
		Ligament L((*dMapPtr)["cube"].get());
		L.leftMultMV(c_MV);
		L.setColor(vec4(.4,.5,.6,.7));
		s.addToRoot("collider",L);

		//Get Sub Rect scales, create Ligaments
		vector<BoundRect> subs(col.getSubs());
		for (uint32_t i=0;i<subs.size();i++){
			Ligament L_sub((*dMapPtr)["quad"].get());
			vec2 dim(subs[i].getDim()/skelScale);
			vec2 pos((subs[i].getPos()-vec2(col.getPos()))/skelScale);
			mat4 r_MV(glm::translate(vec3(pos,0))*glm::scale(vec3(dim,1)));
			L_sub.leftMultMV(r_MV);
			L_sub.setColor(vec4(.6,.5,.4,.7));
			s.addToRoot("box_"+to_string(i),L_sub);
		}
	}

	//return the entity
	return Entity(col, s);
}

//Create the Collider, given an XML Element
Collider getCollider(TiXmlElement * collider){
	string d=",";
	BoundBox bb;
	float soft;
	vector<BoundRect> recVec;

	//Load in the soft collision value
	fillIn(soft, collider->Attribute("soft"));
	
	//Iterate through all sub-rect boxes in element
	for (TiXmlElement * box=collider->FirstChildElement("box"); box; box=box->NextSiblingElement("box")){
		//get scale, translation of each box
		vec3 dim, tr;
		fillVec(dim,box->Attribute("S"));
		fillVec(tr,box->Attribute("T"));
		
		//create BoundBox
		bb = BoundBox(tr,dim);

		//Do the same for rects
		for (TiXmlElement  * rect=box->FirstChildElement("rect"); rect; rect=rect->NextSiblingElement("rect")){
			vec2 rDim, rTr;
			fillVec(rDim,rect->Attribute("S"));
			fillVec(rTr, rect->Attribute("T"));
			recVec.emplace_back(rTr,rDim);
		}
	}

	//create collider, add rects
	Collider c(bb, soft);
	for (vector<BoundRect>::iterator it=recVec.begin(); it!=recVec.end(); it++)
		c.addSub(*it);

	return c;
}
