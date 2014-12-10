#include <Drawable.h>

#include <GL_Includes.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

unordered_map<string, uint32_t> Drawable::TextureSettings = 
	unordered_map<string, uint32_t> ();

Drawable::Drawable(JShader * shader, int mode, vec4 origin)
:mShader(shader), mMode(mode){
	origins.clear();
	origins.push_back(origin);
}

void Drawable::setOrigins(vector<vec4> vO){
	origins = vO;
}

void Drawable::setVAO(GLuint VAO){
	mVAO = VAO;
}

void Drawable::setOrigin(vec4 o){
	origins.clear();
	origins.push_back(o);
}

bool Drawable::addTex(string s, GLuint tex){
	if (tex){
		if (texMap.find(s) == texMap.end()){
			texMap[s] = tex;
			if (mMode < 0)
				mMode = 0;
		}
		return true;
	}
	return false;
}

bool Drawable::addTex(vector<pair<string, GLuint> > texVec){
	bool ret(true);
	for (uint32_t i=0;i<texVec.size();i++)
      if (!addTex(texVec[i].first, texVec[i].second))
			ret = false;
	return ret;
}

void Drawable::setNElements(int n){
	mElementCount = n;
}

//I suppose this means mode's getting phased out
void Drawable::draw(string currentTex){//, vec4 color){
	int mode(-1);

	if (currentTex.length()){
		unordered_map<string,GLuint>::iterator it(texMap.find(currentTex));
		if (it != texMap.end()){
			glBindTexture(GL_TEXTURE_2D,it->second);
			mode = 0;
		}
		else
			cout << "Drawable: Texture not found: " << currentTex << endl;
	}
	
	glUniform1i(mShader->getModeHandle(), mode);
	glBindVertexArray(mVAO); //Bind my VAO
	glDrawElements(GL_TRIANGLES, mElementCount, GL_UNSIGNED_INT, NULL);
}

int Drawable::getNumElems(){
	return mElementCount;
}

void Drawable::uploadMV(mat4& MV){
	glUniformMatrix4fv(mShader->getMVHandle(), 1, GL_FALSE, glm::value_ptr(MV));
}

void Drawable::uploadColor(vec4& color){
	glUniform4fv(mShader->getColorHandle(), 1, glm::value_ptr(color));
}

GLuint Drawable::getVAO(){
	return mVAO;
}

vec4 Drawable::getOrigin(mat4 MV){
	return MV*origins.front();
}

vector<vec4> Drawable::getOrigins(mat4 MV){
	vector<vec4> ret;
	for (vector<vec4>::iterator it=origins.begin();it!=origins.end();it++)
		ret.push_back(MV* (*it));
	return ret;
}
