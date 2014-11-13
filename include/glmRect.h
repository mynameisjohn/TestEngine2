#ifndef GLM_RECT
#define GLM_RECT

#include "Util.h"

class glmRect{
public:
	glmRect();
	glmRect(vec2 dim);
	glmRect(vec2 pos, vec2 dim);
	void scale(vec2 s);
	float left();
	float right();
	float bottom();
	float top();
	vec2 getPos();
	vec2 getDim();
protected:
	vec2 mPos;
	vec2 mDim;
};

#endif
