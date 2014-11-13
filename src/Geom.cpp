#include "Geom.h"
#include "Textures.h"

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

#include <tinyxml.h>
#include <sstream>
#include <glm/gtx/transform.hpp>
#include <unordered_set>

const string SVG_DIR  = "res/svg/";
const string IMG_DIR  = "res/img/";
const string RIG_DIR  = "res/rig/";
const string SPRT_DIR = "res/sprt/";

GLuint genVAO(geoInfo gI, JShader& shader){
	GLuint VAO;
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

   GLuint buffers[4];
   glGenBuffers(4, buffers);

   //vertices
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*gI.vertices.size(), gI.vertices.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(shader.getPosHandle());
   glVertexAttribPointer(shader.getPosHandle(), 4, GL_FLOAT, 0, 0, 0);

   //tex coords
   glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*gI.texCoords.size(), gI.texCoords.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(shader.getTexCoordHandle());
   glVertexAttribPointer(shader.getTexCoordHandle(), 2, GL_FLOAT, 0, 0, 0);

   //indices
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[2] );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle)*gI.indices.size(), gI.indices.data(), GL_STATIC_DRAW );
	
	//weights
	if (gI.weights.size()){
		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*gI.weights.size(), gI.weights.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(shader.getWeightHandle());
		glVertexAttribPointer(shader.getWeightHandle(), 3, GL_FLOAT, 0, 0, 0);
	}
   
	glBindVertexArray(0);

	return VAO;
}

Drawable initObj(string fileName, JShader& shader){
	Drawable dr(&shader, 0);
	map<unsigned int, vec3> vMap;
	map<unsigned int, vec2> tMap;
	
	vector<string> faces;
	geoInfo gI;

	unordered_set<string> addedIndices;

	cout << "can we open it?" << endl;
	ifstream objFile(fileName);
	cout << "maybe not?" << endl;
	
	if (objFile.is_open()){
		const string d(" ");
		string line;
		while (getline(objFile, line)){
//			cout << line << endl;
			string type(line.substr(0,line.find(d)));
			if (type == "v"){
				vec3 vert;
				for (int i=0;i<3;i++){
					line = line.substr(line.find(d)+1,line.size());
					string value(line.substr(0,line.find(d)));
					if (!(stringstream(value) >> vert[i])){
						cout << "Error reading OBJ file vertex :" << value << endl;
						return dr;
					}
				}
				vMap[vMap.size()+1] = vert;
			}
			if (type == "vt"){
				vec2 tex;
				for (int i=0;i<2;i++){
					line = line.substr(line.find(d)+1,line.size());
					string value(line.substr(0,line.find(d)));
					if (!(stringstream(value) >> tex[i])){
						cout << "Error reading OBJ file tex coord :" << value << endl;
						return dr;
					}
				}
				tMap[tMap.size()+1] = tex;
			}
			if (type == "f"){
				line = line.substr(2,line.size());
				for (int i=0;i<3;i++){
					faces.push_back(line.substr(0,line.find(d)));
					line = line.substr(line.find(d)+1,line.size());
				}
			}
		}
		vector<unsigned int> indices;
		vector<string>::iterator fIt;
		map<string, int> fuck;
		for (fIt=faces.begin(); fIt!=faces.end(); fIt++){
			unsigned int v, t;
			if ((stringstream(fIt->substr(0,fIt->find("/"))) >> v) && 
					(stringstream(fIt->substr(fIt->find("/")+1,fIt->length())) >> t)){
//0,1)) >> v) && (stringstream(fIt->substr(2,3)) >> t)){
				if (addedIndices.find(*fIt) == addedIndices.end()){
					if (vMap.find(v) != vMap.end()){
						if (tMap.find(t) != tMap.end()){
							gI.vertices.emplace_back(vMap[v],1);
							gI.texCoords.push_back(tMap[t]);
							addedIndices.insert(*fIt);
							fuck[*fIt] = gI.vertices.size()-1;
						}
						else{
						cout << "Invalid texture coordinate from face " << *fIt << ": " << fIt->substr(fIt->find("/")+1,fIt->length()) << endl;
						return dr;
						}
					}
					else{
						cout << "Invalid vertex from face " << *fIt << ": " << fIt->substr(0,fIt->find("/")) << endl;
						return dr;
					}
				}
				indices.push_back(fuck[*fIt]);
//				indices.push_back(v-1);
			}
			else{
				cout << "Invalid OBJ File : unable to process face " << *fIt << endl;
				return dr;
			}
		}
		if (indices.size() % 3){
			cout << "Invalid number of indices (we need triangles)" << endl;
			return dr;
		}
		else{
			for (uint32_t i=0;i<indices.size();i++){
				triangle t;
				t.A = indices[i++];
				t.B = indices[i++];
				t.C = indices[i];
//				cout << t.A << ", " << t.B << ", " << t.C << endl;
				gI.indices.push_back(t);
			}
		}
		dr.setVAO(genVAO(gI, shader));
		dr.setNElements(3*gI.indices.size());
		dr.addTex("outline", outlineTexture());
		cout << gI.indices.size() << endl;
		return dr;
	}
	else{
		cout << "Invalid OBJ file " << fileName;
		return dr;
	}
}

//Bug with the way offset is being handled
Rig initRigFromSVG(string fileName, JShader& shader){
	string rigName = RIG_DIR+fileName+".rig";
	Rig r(getRigFromSVG(rigName, shader));
	geoInfo gI = SVGtoGeometry("res/svg/"+fileName+".svg",1);
	r.setOrigins(gI.origins);
   GLuint VAO = genVAO(gI, shader);
	r.setVAO(VAO);
	vector<string> imageFiles = getSprtFileList("res/sprt/"+fileName+".sprt");
	for (uint32_t i=0;i<imageFiles.size();i++)
      r.addTex(imageFiles[i].substr(0,imageFiles[i].length()-4),fromImage("res/img/"+imageFiles[i]));
   r.setNElements(3*gI.indices.size());

	return r;
}

//assumes "x.svg" as input, as well as some "x.png" texture
Drawable initPolyFromSVG(string fileName, JShader& shader){
	Drawable dr(&shader, 0);

	geoInfo gI = SVGtoGeometry("res/svg/"+fileName,0);
	gI.weights = vector<vec3>();
	GLuint VAO = genVAO(gI, shader);	
	string imageName = fileName.substr(0,fileName.length()-4)+".png";
   dr.setVAO(VAO);
   dr.addTex(imageName.substr(0,imageName.length()-4),fromImage("res/img/"+imageName));//outlineTexture());
   dr.setNElements(3*gI.indices.size());

   return dr;
}

Drawable initSpriteSheet(string fileName, JShader& shader){
	Drawable dr = initQuad(shader);
	vector<string> files = getSprtFileList("res/sprt/"+fileName+".sprt");
	for (uint32_t i=0;i<files.size();i++)
		dr.addTex(files[i].substr(0,files[i].length()-4),fromImage("res/img/"+files[i]));//"res/img/"+imageName+".png"));
	
	return dr;
}

Drawable initTexQuad(JShader& shader, string imageName){
	
	Drawable dr = initQuad(shader);
/*
	if (imageName.length())
		dr.addTex(fromImage("res/img/"+imageName+".png"));
	else
		dr.addTex(outlineTexture());
*/
	if(!imageName.length() || imageName=="outline")
		dr.addTex("outline", outlineTexture());
	else
		dr.addTex(imageName, fromImage("res/img/"+imageName+".png"));

	return dr;
}

Drawable initQuad(JShader& shader){
	Drawable dr(&shader,-1, vec4(0.5,0.5,0,1));

	vector<vec4> vertices={
		{0,0,0,1}, {1,0,0,1},
		{1,1,0,1}, {0,1,0,1}
	};
	//Somewhere along the way these are being flipped (the y coords should be reversed...)
	vector<vec2> texCoords={
		{0,1}, {1,1},
		{1,0}, {0,0}
	};
	vector<triangle> indices=getConvexIndices(vertices.size());
	vector<vec3> weights;
	GLuint VAO = genVAO({vertices, texCoords, indices, weights}, shader);
	
	dr.setVAO(VAO);
	dr.setNElements(3*indices.size());

	//THIS LINE SHIFTS THE QUAD ORIGIN TO (0.5,0.5) (aka the center)
	//RATHER THAN (0,0) (aka the bottom left corner). 
	//There were too many times where I needed the rotation to appear in place
	//dr.leftMultMV(glm::translate(vec3(-.5,-.5,0)));
	dr.setOrigin(vec4(0.5,0.5,0,1));

   return dr;
}

//I don't even want to touch this
Drawable initCube(JShader& shader){
/*
   const int nVert=24, dim=3, nIndices=36;
   const int vStride = nVert*dim*sizeof(GLint);
   const int tStride = nVert*2*sizeof(GLfloat);
   const int iStride = nIndices*sizeof(GLuint);
*/
	Drawable dr(&shader);

//	int verticeCount = 24;
	int faceCount = 12;

	float vertices[] = {
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f
	};
	float texCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};
	unsigned int faceIndex[] = {
		0,1,2,0,2,3,
		4,5,6,4,6,7,
		8,9,10,8,10,11,
		12,13,14,12,14,15,
		16,17,18,16,18,19,
		20,21,22,20,22,23
	};

   GLuint tmpVAO;
   glGenVertexArrays(1, &tmpVAO);
   glBindVertexArray(tmpVAO);

   GLuint buffers[3];
   glGenBuffers(3, buffers);

   //vertices
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(shader.getPosHandle());
   glVertexAttribPointer(shader.getPosHandle(), 4, GL_FLOAT, 0, 0, 0);

   //tex coords
   glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
   glEnableVertexAttribArray(shader.getTexCoordHandle());
   glVertexAttribPointer(shader.getTexCoordHandle(), 2, GL_FLOAT, 0, 0, 0);

   //indices
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[2] );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(faceIndex), faceIndex, GL_STATIC_DRAW );

	//unbind
   glBindVertexArray(0);

	dr.setVAO(tmpVAO);
	//dr.addTex(outlineTexture());
	dr.addTex("outline",outlineTexture());
	dr.setNElements(faceCount*3);
	dr.setOrigin(vec4(0.5,0.5,0.5,1));

   return dr;
}
