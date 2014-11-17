#ifndef GLM_BOX
#define GLM_BOX

#include <Util.h>

class glmBox{
public:
	glmBox();
	glmBox(vec3 dim);
	glmBox(vec3 pos, vec3 dim);
	void scale(vec3 s);
	float left();
	float right();
	float bottom();
	float top();
	float near();
	float far();
	vec3 getPos();
	vec3 getDim();
	vec3 center();
protected:
	vec3 mPos;
	vec3 mDim;
};

#endif
