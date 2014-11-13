#include "BoundBox.h"
#include <stdio.h>

BoundBox::BoundBox()
: glmBox(){}

BoundBox::BoundBox(vec3 dim)
:glmBox(dim){}

BoundBox::BoundBox(vec3 pos, vec3 dim)
: glmBox(pos, dim){}

void BoundBox::translate(vec3 trans){
	mPos += trans;
}

void BoundBox::moveTo(vec3 pos){
	mPos = pos;
}

bool BoundBox::collidesWith(BoundBox& other){
	return collidesX(other) && 
			 collidesY(other) && 
			 collidesZ(other);
}

bool BoundBox::collidesX(BoundBox& other){
	return !(right() < other.left() || left() > other.right());
}

bool BoundBox::collidesY(BoundBox& other){
	return !(top() < other.bottom() || bottom() > other.top());
}

bool BoundBox::collidesZ(BoundBox& other){
	return !(far() > other.near() || near() < other.far());
}
