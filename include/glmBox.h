#ifndef GLM_BOX
#define GLM_BOX

#include "Util.h"

/*
#ifndef GLM_FORCE_RADIANS 
#define GLM_FORCE_RADIANS 
#endif

#include <glm/glm.hpp>
#include <iostream>

using glm::vec3;

#ifndef VEC_PRINT
#define VEC_PRINT
std::ostream& operator<<(std::ostream& os, const glm::vec3& vec);
#endif
*/
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
/*
	I'll re-inline these when the time comes
//inline functions 
public: 
   inline float left(){ 
      return mPos.x; 
   } 
 
   inline float right(){ 
      return mPos.x+mDim.x;
   }

   inline float bottom(){
      return mPos.y+mDim.y;
   }

   inline float top(){
      return mPos.y;
   }

   inline float near(){
      return mPos.z;
   }

   inline float far(){
      return mPos.z+mDim.z;
   }

	inline vec3 getPos(){
		return mPos;
	}

	inline vec3 getDim(){
		return mDim;
	}
	inline vec3 center(){
		return mPos + mDim/2.f;
	}
*/
};

#endif
