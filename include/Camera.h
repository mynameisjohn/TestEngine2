#ifndef CAMERA_H
#define CAMERA_H

//#include "BoundBox.h"
#include <Player.h>

//const vec2 SCREEN_DIM(1200,675);

class Camera{
	public:
		Camera();
		Camera(vec2 sD);
		Camera(float b, mat4 p);
		void translate(vec3 trans);
		void setProjMat(mat4 p);
		void leftMult(mat4 m);
		void rightMult(mat4 m);
		void updateProj(int projHandle);
		void push(Player * player);
		vec2 getScreenDim();
		vec3 push(glm::vec4 player);
		mat4 getProjMat();
	private:
		BoundBox mBB;
		mat4 projMat;
		float buf;
		vec2 screenDim;
};

#endif
