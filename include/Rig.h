#ifndef RIG_H
#define RIG_H

#include <Drawable.h>
#include <Cycle.h>

class Rig : public Drawable{
public:
	Rig(JShader * shader = nullptr, unordered_map<string, Cycle> cmap=unordered_map<string, Cycle>(), vector<vec4> j = vector<vec4>());
	void setCycles(vector<Cycle> cycleVec);
	void addCycle(Cycle c);
	void addCycle(vector<Cycle> cycleVec);
	void draw(string currentTex="");
	void draw(string currentTex, bool rigged);
	float getShift();
	Pose getCurrentPose(string from, string to, vec2& u);
	void uploadRigMats(vector<mat4>& R);
	string getFirstCycle();
private:
	unsigned int numPoses;
	unordered_map<string, Cycle> cMap;
	vector<Cycle> cycles;
};

#endif
