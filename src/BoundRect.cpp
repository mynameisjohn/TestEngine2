#include "BoundRect.h"

BoundRect::BoundRect()
: glmRect() {}

BoundRect::BoundRect(vec2 dim)
: glmRect(dim) {}

BoundRect::BoundRect(vec2 pos, vec2 dim)
: glmRect(pos, dim) {}

void BoundRect::translate(vec2 trans){
	mPos += trans;
}

void BoundRect::moveTo(vec2 pos){
	mPos = pos;
}

bool BoundRect::collidesWith(BoundRect other){
	return collidesX(other) && collidesY(other);
}

bool BoundRect::collidesX(BoundRect other){
	return !(right() < other.left() || left() > other.right());
}

bool BoundRect::collidesY(BoundRect other){
	return !(top() < other.bottom() || bottom() > other.top());
}
