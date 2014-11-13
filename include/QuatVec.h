#ifndef QUATVEC_H
#define QUATVEC_H

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

const char QV_TR(1), QV_RT(2), QV_TRT(4);

class QuatVec{
public:
	glm::vec3 trans;
	glm::fquat rot;
//	bool RT;
	char mode;

public:
	QuatVec();
	QuatVec(glm::vec3, glm::fquat, char m=QV_TR);
	QuatVec(glm::fquat, glm::vec3, char m=QV_RT);
	QuatVec operator * (const float s);
	QuatVec operator * (const QuatVec& other);
	QuatVec operator + (const QuatVec& other);
	char operator & (const QuatVec& other);
	void operator *= (const float s);
	QuatVec blend(const QuatVec& other, float a);
	QuatVec inverse();
	glm::mat4 toMat4();
};

#endif
