#include <Camera.h>

#include <GL_Includes.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(vec2 sd, mat4 p, vec3 center)
	:	m_ScreenDim(sd),
	   projMat (p)
{}

void Camera::translate(vec3 trans){
	//mBB.translate(-trans);
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

void Camera::setScreenDim(vec2 sd){
	m_ScreenDim = sd;
}

vec2 Camera::getScreenDim(){
	return m_ScreenDim;
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
