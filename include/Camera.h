#ifndef CAMERA_H
#define CAMERA_H

//#include "BoundBox.h"
#include "Player.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 675

const vec2 SCREEN_DIM(1200,675);

using glm::mat4;

class Camera{
	public:
		Camera();
		Camera(float b, mat4 p);
		void translate(vec3 trans);
		void setProjMat(mat4 p);
		void leftMult(mat4 m);
		void rightMult(mat4 m);
		void updateProj(int projHandle);
		vec3 push(glm::vec4 player);
		void push(Player * player);
		mat4 getProjMat();
	private:
		BoundBox mBB;
		mat4 projMat;
		float buf;
};

#endif
