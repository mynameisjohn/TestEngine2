#ifndef CAMERA_H
#define CAMERA_H

//#include "BoundBox.h"
#include <Player.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 675

const vec2 DEFAULT_SCREEN_DIM(1200,675);


class Camera{
	public:
		Camera(vec2 sd = DEFAULT_SCREEN_DIM, mat4 p = mat4(1), vec3 center = vec3());
//		Camera(float b, mat4 p);
		void translate(vec3 trans);
		void setProjMat(mat4 p);
		void leftMult(mat4 m);
		void rightMult(mat4 m);
		void updateProj(int projHandle);
		void setScreenDim(vec2 sd);
		vec2 getScreenDim();
		vec3 push(glm::vec4 player);
//		void push(Player * player);
		mat4 getProjMat();
	private:
		vec2 m_ScreenDim;
//		BoundBox mBB;
		mat4 projMat;
//		float buf;
};

#endif
