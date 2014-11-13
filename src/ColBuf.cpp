#include "ColBuf.h"

ColBuf::ColBuf(int dim) : CircBuf<int>(dim){
	collisions=0;
	x=1; y=2; z=4;
}

void ColBuf::addDimCol(unsigned int eig, bool col){
	//if this dimension has collided
	if (col){
		//if it's not already in the bit vector
      if (~collisions & eig){
			//add it to the bit vector and circular buffer
         add(eig);
         collisions |= eig;
      }
   }//otherwise, remove the dimension from the bit vector
	else collisions &= ~eig;
}

char ColBuf::handleCollision(bool colX, bool colY, bool colZ){
   addDimCol(x, colX);
   addDimCol(y, colY);
   addDimCol(z, colZ);
   
	if (collisionDetected()){
		int l = last();
		if (l==x) return 'x';
		if (l==y) return 'y';
		if (l==z) return 'z';
	}
   return (char)0;
}

int ColBuf::handleIt(bool colX, bool colY, bool colZ){
	addDimCol(x, colX);
	addDimCol(y, colY);
	addDimCol(z, colZ);

	if (collisionDetected())
		return last();
	return 0;
}
