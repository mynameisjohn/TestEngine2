#ifndef POSE_H
#define POSE_H

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <vector>
#include <glm/glm.hpp>
//#include <glm/gtx/dual_quaternion.hpp>

#include "QuatVec.h"

//using glm::fdualquat;
//using glm::fquat;

class Pose{
public:
	Pose();
	Pose(std::vector<QuatVec> in, float delT=0.1f);
	Pose operator*(const float& s);
	Pose operator+(const Pose& other);
	Pose blend(const Pose& other, float x);
	void setDT(float dt);
	int getNumJoints();
	float getDT();
	float maxDiff(const Pose& other);
	std::vector<glm::mat4> getMats();
	std::vector<QuatVec> joints;
	float T, dt;
private:
};

#endif
