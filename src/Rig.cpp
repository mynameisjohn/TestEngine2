#include "Rig.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Rig::Rig(JShader * shader, unordered_map<string, Cycle> cmap, vector<vec4> j)
: Drawable(shader, 1), numPoses(0), cMap(move(cmap)){//, origins(move(j))
	origins = j;
	mMode = cMap.begin()->second.getNumJoints();

//	cout << cMap.begin()->first << "\t" << cMap.begin()->second.getNumPoses() << "\t" << cMap.begin()->second.getNumJoints() << endl;
}

void Rig::setCycles(vector<Cycle> cycleVec){
	uint8_t i=0;
	//ensure a consisten number of poses across cycles
	uint8_t nP = cycleVec[i].getNumPoses();
	for (i=1; i<cycleVec.size(); i++){
		if (nP != cycleVec[i].getNumPoses()){
			cout << "Inconsistent number of poses. Segfault iminent." << endl;
			return;
		}
	}
	
	numPoses = (unsigned int)nP;
	cycles=cycleVec;
	mMode = cycles[0].getNumJoints();
}

void Rig::addCycle(Cycle c){
	if (cycles.size() < 3)
		cycles.push_back(c);
}

void Rig::addCycle(vector<Cycle> cycleVec){
	for (uint32_t i=0;i<cycleVec.size();i++)
		if (cycles.size() < 3)
			cycles.push_back(cycleVec[i]);
}

string Rig::getFirstCycle(){
	return cMap.begin()->first;
}

float Rig::getShift(){
	return (float)(cMap.begin()->second.getNumPoses())/2.f;
//	return (float)(cycles[0].getNumPoses())/2.f;
}

Pose Rig::getCurrentPose(string from, string to, vec2& u){//unsigned int from, unsigned int to, vec2& u){
//	cout << from << "\t" << to << endl;
	unordered_map<string, Cycle>::iterator f(cMap.find(from)), t(cMap.find(to));
	if (!(f == cMap.end() || t == cMap.end())){
		if (f == t)
			return f->second.getCurrentPose(u.y);//cycles[from].getCurrentPose(u.y);
		else
			return f->second.blend(t->second, u.x).getCurrentPose(u.y);//cycles[from].blend(cycles[to], u.x).getCurrentPose(u.y);
	}
	else 
		return cMap.begin()->second.getCurrentPose(u.y);
}

void Rig::uploadRigMats(vector<mat4>& R){
	glUniformMatrix4fv(mShader->getRigMatHandle(), (R.size() > 3 ? 3 : R.size()), GL_FALSE, (GLfloat *)R.data());
}

//really no different from the Drawable draw
void Rig::draw(string currentTex, bool rigged){
	glUniform1i(mShader->getModeHandle(), rigged ? mMode : 0);
	if (currentTex.length() && texMap.size()){
		unordered_map<string, GLuint>::iterator it;
      if ((it=texMap.find(currentTex)) != texMap.end())
         glBindTexture(GL_TEXTURE_2D,it->second);
      else
         glBindTexture(GL_TEXTURE_2D,texMap.begin()->second);
   }

   glBindVertexArray(mVAO); //Bind my VAO
   glDrawElements(GL_TRIANGLE_STRIP, mElementCount, GL_UNSIGNED_INT, NULL);
}

mat4 Rig::draw(mat4 parentMV, vec2& u, unsigned int from, unsigned int to, string currentTex){//, vec4 color){
/*	vector<mat4> rigData;
   if (from == to)
      rigData = cycles[from].getCurrentPose(u.y).getMats();
   else
      rigData = cycles[from].blend(cycles[to], u.x).getCurrentPose(u.y).getMats();
	//Find inherited transform
//   mat4 transform = parentMV;// * MV;
	unordered_map<string, GLuint>::iterator it;
	
	glUniformMatrix4fv(mShader->getRigMatHandle(), numPoses, GL_FALSE, (GLfloat *)rigData.data());
   glUniform1i(mShader->getModeHandle(), mMode);

	if (texMap.size()){
		if (currentTex.length() && ((it=texMap.find(currentTex)) != texMap.end()))
			glBindTexture(GL_TEXTURE_2D,it->second);
		else
			glBindTexture(GL_TEXTURE_2D,texMap.begin()->second);
	}
   
   glBindVertexArray(mVAO); //Bind my VAO
   glDrawElements(GL_TRIANGLE_STRIP, mElementCount, GL_UNSIGNED_INT, NULL);

	return rigData.front();
*/
//	return transform * rigData.back();

	return mat4();
}

void Rig::draw(string currentTex){//, vec4 color){
	unordered_map<string, GLuint>::iterator it;

   glUniform1i(mShader->getModeHandle(), 0);//mMode);

	if (currentTex.length() && texMap.size()){
		if ((it=texMap.find(currentTex)) != texMap.end())
			glBindTexture(GL_TEXTURE_2D,it->second);
		else
			glBindTexture(GL_TEXTURE_2D,texMap.begin()->second);
	}

   glBindVertexArray(mVAO); //Bind my VAO
   glDrawElements(GL_TRIANGLE_STRIP, mElementCount, GL_UNSIGNED_INT, NULL);

	return;
}
