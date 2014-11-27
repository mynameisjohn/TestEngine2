#include <Camera.h>

#include <GL_Includes.h>

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
	vec4 playerPos = projMat*vec4(player.x,player.y,player.z,1);
	playerPos /= playerPos.w;
	vec3 a(100,25, 150);
	vec3 displace(playerPos);
	displace.z = 0.95-displace.z;
	for (int i=0;i<3;i++)
		if (fabs(displace[i]) < 0.015f)
				displace[i] = 0;

	projMat = projMat * glm::translate(-a*displace);

	return vec3(playerPos)/playerPos.w;
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
