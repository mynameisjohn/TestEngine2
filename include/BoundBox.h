#ifndef BOUNDBOX_H
#define BOUNDBOX_H

#include "glmBox.h"

class BoundBox : public glmBox{
public:
	BoundBox();
   BoundBox(vec3 dim);
   BoundBox(vec3 pos, vec3 dim);
	void translate(vec3 trans);
	void moveTo(vec3 pos);
	bool collidesWith(BoundBox& other);
	bool collidesX(BoundBox& other);
	bool collidesY(BoundBox& other);
	bool collidesZ(BoundBox& other);

/*
	I'll re-inline these when the time comes
//inline functions
public:
	inline bool collidesWith(BoundBox& other){
		return collidesX(other) && 
				 collidesY(other) && 
				 collidesZ(other);
	}

	inline bool collidesX(BoundBox& other){
		return !(right() < other.left() || left() > other.right());
	}

	inline bool collidesY(BoundBox& other){
		return !(bottom() < other.top() || top() > other.bottom());
	}

	inline bool collidesZ(BoundBox& other){
		return !(far() < other.near() || near() > other.far());
	}
*/
};


#endif
