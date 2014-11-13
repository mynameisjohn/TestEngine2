#include "glmRect.h"

glmRect::glmRect(){
	mPos=vec2();
	mDim=vec2(100.f, 100.f);
}

glmRect::glmRect(vec2 dim){
	mPos=vec2();
	mDim=glm::abs(dim);
}

glmRect::glmRect(vec2 pos, vec2 dim){
	mPos=pos;
	mDim=glm::abs(dim);
}

//Note that this doesn't scale about origin (intentionally)
void glmRect::scale(vec2 s){
	mDim *= s;
}

float glmRect::left(){
	return mPos.x;
}

float glmRect::right(){
	return mPos.x+mDim.x;
}

float glmRect::bottom(){
	return mPos.y;
}

float glmRect::top(){
	return mPos.y+mDim.y;
}

vec2 glmRect::getPos(){
	return mPos;
}

vec2 glmRect::getDim(){
	return mDim;
}
