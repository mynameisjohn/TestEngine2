#include "Camera.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(){
	buf=200.f; 
	mBB = BoundBox(vec3(-1000,-1000,0),vec3(2000,2000,0));
	projMat=glm::perspective((float)M_PI/4.f, SCREEN_DIM.x/SCREEN_DIM.y, 100.f, 10000.f) *
		glm::rotate((float)M_PI/15.f, vec3(1, 0, 0)) * 
		glm::translate(vec3(0, -(SCREEN_DIM.x*0.667F),0));
}

Camera::Camera(float b, mat4 p){
	buf = b;
	projMat=p;
}

void Camera::translate(vec3 trans){
	mBB.translate(-trans);
	projMat = projMat * glm::translate(trans);
}

//what a strange method...
vec3 Camera::push(vec4 player){
/*
	dX = (player.x-mBB.left());
	//printf("%lf\t",dX);	
	if (dX < buf){//don't sweat the small stuff...
		translate(vec3(player.w, 0, 0));
	}
	else{
		dX = (mBB.right() - player.x);
		//printf("%lf\n",dX);	
		if (dX < buf){
			translate(vec3(-player.w, 0, 0));
		}
	}
*/
/*

	const vec2 buffer = vec2(1)/2.f;
*/
	vec4 playerPos = projMat*vec4(player.x,player.y,player.z,1);
	playerPos /= playerPos.w;
/*
	float dy = -.0005f*playerPos.y;

	vec4 tr(0,dy,0,1);
	tr = glm::inverse(projMat)*tr;
	tr /= tr.w;

	cout << vec3(player) - mBB.center()+vec3(0,500,0) << endl;
	projMat = projMat * glm::translate(vec3(0,vec3(vec3(player) - mBB.center()).y*-.05f,0));
	mBB.translate(-(vec3(0,vec3(vec3(player) - mBB.center()).y*-.05f,0)));
*/


	playerPos.z = 2*(.95-playerPos.z);
	projMat = projMat * glm::translate(vec3(-150,-25,-100)*glm::pow(vec3(playerPos),vec3(3,3,3)));//0,-50.f*playerPos.y,0));
//	cout << tr << endl;
	
//	projMat = glm::translate(tr)*projMat;
/*
	if (ps.x < -buffer.x)
		tr.x = dt*ps.x+buffer.x;
	else if (ps.x > buffer.x)
		tr.x = dt*ps.x-buffer.x;

	cout << playerPos/playerPos.w << endl;
*/
	return vec3(playerPos)/playerPos.w;
/*
	playerPos /= playerPos.w;

	//returns true of the mouse is to the player's right
	return (playerPos.x - sp.x < 0);
*/
}

//this is all messed up
//consider moving to pixel coords
void Camera::push(Player * player){
	const float buffer = 200.f;
	vec2 d;

	vec3 pVel = player->getVel();
	vec3 pPos = player->center();
	
	if (pPos.x - mBB.left() < buffer)
		d.x = pVel.x;
	else if (pPos.x - mBB.right() > buffer)
		d.x = pVel.x;

	if (pPos.y - mBB.bottom() < buffer)
		d.y = pVel.y;
	else if (pPos.y - mBB.top() > buffer)
		d.y = pVel.y;

/*
	vec4 screen = projMat * vec4(pPos,1);
	screen /= screen.w;
	vec2 sp(vec2(screen)/SCREEN_DIM);
	
	cout << sp << endl;
*/

	translate(vec3(d,0));
}

void Camera::setProjMat(mat4 p){
	projMat = p;
}

void Camera::leftMult(mat4 m){
	projMat = m * projMat;
}

void Camera::rightMult(mat4 m){
	projMat = projMat * m;
}

mat4 Camera::getProjMat(){
	return projMat;
}

void Camera::updateProj(GLint projHandle){
	glUniformMatrix4fv(projHandle, 1, GL_FALSE, glm::value_ptr(projMat));
}
