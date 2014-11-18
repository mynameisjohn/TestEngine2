#include <Geom.h>
#include <Textures.h>

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

#include <GL_Includes.h>

#include <tinyxml.h>
#include <sstream>
#include <glm/gtx/transform.hpp>
#include <unordered_set>

//Create a Vertex Array Object given some geometry info
uint32_t genVAO(geoInfo gI, JShader& shader){
	uint32_t VAO;
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

   uint32_t buffers[4];
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

//Init a drawable from a .obj file
Drawable initObj(string fileName, JShader& shader){
	Drawable dr(&shader, -1);
	map<unsigned int, vec3> vMap;
	map<unsigned int, vec2> tMap;
	
	vector<string> faces;
	geoInfo gI;

	map<string, unsigned int> addedIndices;

	ifstream objFile(fileName);
	
	if (objFile.is_open()){
		const string d(" ");
		string line;
		while (getline(objFile, line)){
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
		vector<string>::iterator sIt;
		vector<unsigned int> indices;

		for (sIt=faces.begin(); sIt!=faces.end(); sIt++){
			unsigned int v, t;
			stringstream(sIt->substr(0,sIt->find("/"))) >> v;
			stringstream(sIt->substr(sIt->find("/")+1,sIt->length())) >> t;
			if (addedIndices.find(*sIt) == addedIndices.end()){
				vec3 vert(vMap[v]);
				vec2 tex(tMap[t]);
				addedIndices[*sIt] = gI.vertices.size();
				gI.vertices.emplace_back(vert,1);
				gI.texCoords.push_back(tex);
			}
			unsigned int idx(addedIndices[*sIt]);
			indices.push_back(idx);
		}
		for (uint32_t i=0;i<indices.size();i++){
			triangle t;
			t.A = indices[i++];
			t.B = indices[i++];
			t.C = indices[i];
			gI.indices.push_back(t);
		}
		dr.setVAO(genVAO(gI,shader));
		dr.addTex("outline",outlineTexture());
		dr.setNElements(indices.size());
		return dr;
	}

	return initCube(shader);
}

//Initialize a rig from an SVG file and a rig file
Rig initRigFromSVG(string fileName, JShader& shader){
	string resName(fileName.substr(0,fileName.length()-4));
	string rigName = RIG_DIR+fileName;
	Rig r(getRigFromSVG(rigName, shader));
	geoInfo gI = SVGtoGeometry(SVG_DIR+resName+".svg", true);
	r.setOrigins(gI.origins);
   uint32_t VAO = genVAO(gI, shader);
	r.setVAO(VAO);
	vector<string> imageFiles = getSprtFileList(SPR_DIR+resName+".sprt");
	for (uint32_t i=0;i<imageFiles.size();i++){
		string imRes(imageFiles[i].substr(0,imageFiles[i].length()-4));
		r.addTex(imRes, fromImage(IMG_DIR+imageFiles[i]));
	}
   r.setNElements(3*gI.indices.size());

	return r;
}

//assumes "x.svg" as input, as well as some "x.png" texture
Drawable initPolyFromSVG(string fileName, JShader& shader){
	Drawable dr(&shader, 0);

	geoInfo gI = SVGtoGeometry(SVG_DIR+fileName,0);
	gI.weights = vector<vec3>();
	uint32_t VAO = genVAO(gI, shader);	
	string imageName = fileName.substr(0,fileName.length()-3)+"png";
	string imRes(imageName.substr(0,imageName.length()-4));
   dr.setVAO(VAO);
   dr.addTex(imRes, fromImage(IMG_DIR+imageName));
   dr.setNElements(3*gI.indices.size());

   return dr;
}

//Creates a textured quad with multiple textures to choose from
Drawable initSpriteSheet(string fileName, JShader& shader){
	Drawable dr = initQuad(shader);
	vector<string> files = getSprtFileList(SPR_DIR+fileName);
	for (uint32_t i=0;i<files.size();i++){
		string imRes(files[i].substr(0,files[i].length()-4));
		dr.addTex(imRes, fromImage(IMG_DIR+files[i]));
	}
	
	return dr;
}

//Initialize a Textured Quad
Drawable initTexQuad(string fileName, JShader& shader){
	Drawable dr = initQuad(shader);
	if(fileName == "outline")
		dr.addTex("outline", outlineTexture());
	else{
		string resName(fileName.substr(0,fileName.length()-4));
		dr.addTex(resName, fromImage(IMG_DIR+fileName));
	}

	return dr;
}

//Initialize a quad
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
	uint32_t VAO = genVAO({vertices, texCoords, indices, weights}, shader);
	
	dr.setVAO(VAO);
	dr.setNElements(3*indices.size());

	dr.setOrigin(vec4(0.5,0.5,0,1));

   return dr;
}

//I don't even want to touch this
Drawable initCube(JShader& shader){
	Drawable dr(&shader);

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

   uint32_t tmpVAO;
   glGenVertexArrays(1, &tmpVAO);
   glBindVertexArray(tmpVAO);

   uint32_t buffers[3];
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
	dr.setNElements(faceCount*3);
	dr.setOrigin(vec4(0.5,0.5,0.5,1));

   return dr;
}
