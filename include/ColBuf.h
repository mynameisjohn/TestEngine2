#ifndef COLBUF_H
#define COLBUF_H

#include "CircBuf.h"

class ColBuf : public CircBuf<int>{
public:
	ColBuf(int dim = 3);
	void addDimCol(unsigned int, bool);
	char handleCollision(bool, bool, bool);
	int handleIt(bool, bool, bool);
	inline bool collisionDetected();
private:
	int collisions;
	int x, y, z;
};

inline bool ColBuf::collisionDetected(){
	return collisions == (x|y|z);
}

#endif
