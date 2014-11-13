#include "XMLReader.h"
#include <exception>
#include <glm/gtx/transform.hpp>

template <typename T>
bool fillIn(TiXmlElement * el, string id, T& v){
	try{
      return stringstream(el->Attribute(id.c_str())) >> v;
   }
   catch (exception& e){
      cout << "XML attribute " << id << " in element " << 
		string(el->Attribute("id")) << " did not fit in variable. " << endl;
		return false;
   }
	return true;
}

bool fillIn(string s, unsigned int v);
bool fillIn(string s, float v);

vector<string> getSprtFileList(string sprtFile){
	vector<string> ret;
	TiXmlDocument doc(sprtFile);
   if (!doc.LoadFile()){
      cout << "Unable to load Sprite file " << sprtFile << "\n";
		return ret;
   }

   TiXmlHandle mHandle(&doc);
	TiXmlElement * sprt, * el = mHandle.FirstChild("spritesheet").ToElement();

	for (sprt=el->FirstChildElement("sprite"); sprt; sprt = sprt->NextSiblingElement("sprite")){
		string fileName = sprt->Attribute("fileName");
		ret.push_back(fileName);
	}

	return ret;
}

void fillIt(TiXmlElement * el, string id, unsigned int &v){
	try{
      stringstream(el->Attribute(id.c_str())) >> v;
   }
   catch (exception& e){/*
      cout << "XML attribute " << id << "in element " << 
		string(el->Attribute("id")) << " did not fit in variable. " << endl;*/
   }
}

vector<triangle> getConvexIndices(uint32_t n){
	vector<triangle> indices;
	for (uint32_t i=0;i<n/2-1;i++){
		indices.push_back({i,n-i-1,n-i-2});//odd
		indices.push_back({i,i+1,n-i-2});//even
	}
	return indices;
}

Rig getRigFromSVG(string svgFile, JShader& shader){
//	Rig ret(&shader);
	vector<Cycle> cycles;
	vector<Pose> poses;
	vector<QuatVec> Joints;
	vec3 T, S;
	vec4 R;
	unsigned int C;

	unordered_map<string,Cycle> cMap;
	vector<vec4> jointPositions;

	TiXmlDocument doc(svgFile);
   if (!doc.LoadFile()){
      cout << "Unable to load SVG file " << svgFile << "\n";
   }

   TiXmlHandle mHandle(&doc);
	TiXmlElement * rig, * cycle, * pose, * transform;

	rig = mHandle.FirstChild("rig").ToElement();//.FirstChild("g").FirstChild("rig").ToElement();
		if (!rig) 
			return Rig();

	if (rig->Attribute("S")){
		string inStr = rig->Attribute("S");
		size_t pos=0;
		const string d=",";
		for (int i=0;i<3;i++){
			pos = inStr.find(d);
			stringstream(inStr.substr(0,pos)) >> S[i];
			inStr.erase(0,pos+d.length());
		}
	}
	else
		S = vec3(1);

	if (!fillIn(rig, "C", C))
		C=1;

	for (cycle = rig->FirstChildElement("cycle"); cycle; cycle=cycle->NextSiblingElement("cycle")){
		for (pose = cycle->FirstChildElement("pose"); pose; pose=pose->NextSiblingElement("pose")){
			for (transform=pose->FirstChildElement("trans"); transform; transform=transform->NextSiblingElement("trans")){
				string inStr = transform->Attribute("T");
				size_t pos=0;
				string d=",";
				for (int i=0;i<3;i++){
					pos = inStr.find(d);
					stringstream(inStr.substr(0,pos)) >> T[i];
					inStr.erase(0,pos+d.length());
				}
				inStr = transform->Attribute("R");
				pos=0;
				for (int i=0;i<4;i++){
					pos = inStr.find(d);//in degrees
					stringstream(inStr.substr(0,pos)) >> R[i];
					inStr.erase(0,pos+d.length());
				}
				Joints.push_back(QuatVec(T,getRQ(R)));//(Joints.size() ? Joints.back() : QuatVec()) * QuatVec(T,getRQ(R)));
			}
			poses.emplace_back(Joints);//, t, dt);
			Joints.clear();
		}
		float dt;
		if (!fillIn(cycle,"dt",dt))
			dt=0.02f;
//		cycles.emplace_back(poses, C, dt);
//		cout << cycle->Attribute("id") << endl;
		cMap.emplace(cycle->Attribute("id"),Cycle(poses,C,dt));
//		cout << cMap.find(cycle->Attribute("id"))->first << endl;
		poses.clear();
	}
//	Rig r(&shader,cMap); // return this one instead
//	ret.setCycles(cycles);
	return Rig(&shader, cMap);
}

map<string,string> getSVGPathMap(string svgFile){
	map<string,string> pathMap;
	TiXmlDocument doc(svgFile);
	if (!doc.LoadFile()){
		cout << "Unable to load SVG file " << svgFile << "\n";
	}

	TiXmlHandle mHandle(&doc);
	TiXmlElement * mElement = mHandle.FirstChild("svg").FirstChild("g").FirstChild("path").ToElement();
	if (!mElement){
		cout << "Couldn't find path element in SVG document\n";
	}
	
	while (mElement){
		pathMap[mElement->Attribute("id")] = mElement->Attribute("d");
      mElement = mElement->NextSiblingElement("path");//->ToElement();
   }
	
	mElement = mHandle.FirstChild("svg").FirstChild("g").FirstChild("rig").ToElement();
	
	return pathMap;
}

vector<vec4> getPathPoints(string pathStr){
	vector<vec4> ret;
	vec4 p;
	const string d1=" ", d2=",";
	size_t pos=0;
	bool relative;
	relative = (pathStr[0] == 'm');
	pathStr = pathStr.substr(2,pathStr.length());
	while (pos != string::npos){
		switch (pathStr[0]){//todo: add support for Q and C
         case 'L':
         case 'l':
            pos = pathStr.find(d1);
            pathStr.erase(0,pos+d1.length());
            break;
         case 'z':
         case 'Z':
            pos = string::npos;
            break;
         default:	
	         pos = pathStr.find(d2);
            string ptStr = pathStr.substr(0,pos);
            if (!(stringstream(ptStr) >> p.x))
               cout << "Incorrect path string" << ptStr << endl;
            pathStr.erase(0,pos+d2.length());
            pos = pathStr.find(d1);
            ptStr = pathStr.substr(0,pos);
            if (!(stringstream(ptStr) >> p.y))
               cout << "Incorrect path string" << ptStr << endl;
            pathStr.erase(0,pos+d2.length());
            if (ret.size() && relative)
               p += ret.back();
            ret.push_back(p);
      }
	}

	return ret;
}

geoInfo SVGtoGeometry(string svgFile, bool rigged){
   vector<vec4> vertices;
	vector<vec2> texCoords;
   vector<triangle> indices;
	vector<vec3> weights;
	vec3 offset;
	vector<vec4> origins;

	map<string,string> pathMap = getSVGPathMap(svgFile);

	string outlineStr = pathMap.at("outline"), d1=" ", d2=",";

	vector<vec4> tmp = getPathPoints(outlineStr);

	if (outlineStr.length()<=1){
		cout << "Error loading path string\n";
		return {vertices, texCoords, indices};
	}

	for (uint32_t i=0;i<tmp.size();i++)
		texCoords.push_back(vec2(tmp[i]));	

	//min, Max
	vec2 m=texCoords.front(), M=texCoords.front();
	vector<vec2>::iterator it;
	for (it=texCoords.begin(); it!=texCoords.end(); it++){
		m.x = min(m.x,it->x); m.y = min(m.y,it->y);
		M.x = max(M.x,it->x); M.y = max(M.y,it->y);
	}
	M -= m;
	for (it=texCoords.begin(); it!=texCoords.end(); it++){
		*it -= m;
		vec2 vert = vec2(*it);
		*it /= M;
		vert.y = M.y-vert.y;
		vertices.push_back(vec4(vert/max(M.x,M.y),0,1));
	}

	//Maybe change this to check for concavity...much later
	indices = getConvexIndices(vertices.size());

	if (rigged){
		string jointStr = pathMap.at("joints");
		vector<vec2> BonePoints;
		tmp = getPathPoints(jointStr);

		for (uint32_t i=0;i<tmp.size();i++){
			BonePoints.push_back((vec2(tmp[i])-m)/max(M.x,M.y));
			BonePoints.back().y = 1.f-BonePoints.back().y;
		}

		//This assumes a lot (i.e joints drawn bottom-up)
		for (uint32_t j=0;j<vertices.size();j++){
			vec3 w;//nJoints should be  < 3
			for (uint32_t i=0;i<BonePoints.size();i++){
				int idx = BonePoints.size()-1-i;
				w[idx]=(float)exp(-pow(glm::length(BonePoints[i]-vec2(vertices[j])),2)/.075f);
			}
			w=glm::normalize(w);
			//cout << w << endl;
			weights.push_back(w);
		}

		float len = 0;
		for (uint32_t i=0;i<BonePoints.size();i++){
			if (glm::length(BonePoints[i]) > len){
				len=glm::length(BonePoints[i]);
			}
		}
		origins.resize(BonePoints.size());
		for (uint32_t i=0;i<BonePoints.size();i++)
			origins[BonePoints.size()-1-i] = vec4(BonePoints[i],0,1);
/*
		//I need to get these BonePoints somewhere else
		//put it in offset
		for (int i=0;i<vertices.size();i++)
			//origins.push_back(BonePoints[k]);
			vertices[i] -= vec4(BonePoints[k],0,0);
		offset = vec3(BonePoints[k],0);
*/
	}

	return {vertices, texCoords, indices, weights, offset, origins};
}
