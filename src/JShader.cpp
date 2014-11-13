#include "JShader.h"
//#include <glm/gtx/transform.hpp>

JShader::JShader(){
	u_MVHandle = -1;
	u_ProjHandle = -1;
	u_RigMatHandle = -1;
	u_ColorHandle = -1;
	u_ModeHandle = -1;
	a_PosHandle = -1;
	a_TexCoordHandle = -1;
	a_WeightHandle = -1;
	mVS = 0;
	mFS = 0;
}

bool JShader::loadProgram(){
	GLint success = GL_TRUE;	
	
	if (!mVS){
		printf("Missing vertex shader\n");
		return false;
	}

	 if (!mFS){
      printf("Missing fragment shader\n");
      return false;
   }
	
	mProgramID = glCreateProgram();
	glAttachShader(mProgramID, mVS);
	glAttachShader(mProgramID, mFS);
	glLinkProgram(mProgramID);

	glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
	if (success != GL_TRUE){
		printf("Error linking shader program %d. \n", mProgramID);
		printProgramLog(mProgramID);
		mProgramID = 0;
		return false;
	}

	a_PosHandle = getAttribHandle("a_Position");
	if (a_PosHandle < 0)
		return false;
	
	a_TexCoordHandle = getAttribHandle("a_TexCoord");
	if (a_TexCoordHandle < 0)
		return false;
	
	u_ColorHandle = getUniformHandle("u_Color");
	if (u_ColorHandle < 0)
		return false;

	u_ProjHandle = getUniformHandle("u_Proj");
	if (u_ProjHandle < 0)
		return false;

	u_MVHandle = getUniformHandle("u_MV");
	if (u_MVHandle < 0)
		return false;

	u_ModeHandle = getUniformHandle("u_Mode");
	if (u_ModeHandle < 0)
		return false;

	u_RigMatHandle = getUniformHandle("u_RigMat");
	if (u_ModeHandle < 0)
		return false;
	
	a_WeightHandle = getAttribHandle("a_W");
	if (u_ModeHandle < 0)
		return false;

/*
	a_PosHandle = glGetAttribLocation(mProgramID, "vPosition");
   if (a_PosHandle == -1){
      printf("%s is not a valid shader program variable.\n","vPosition");
		return false;
	}

	u_ColorHandle = glGetUniformLocation(mProgramID, "fColor");
	if (u_ColorHandle == -1){
      printf("%s is not a valid shader program variable.\n","fColor");
      return false;
   }

	u_ProjHandle = glGetUniformLocation(mProgramID, "P");
   if (u_ProjHandle == -1){
      printf("%s is not a valid shader program variable.\n","P");
      return false;
   }

	u_MVHandle = glGetUniformLocation(mProgramID, "MV");
   if (mMVPHandle == -1)
      printf("%s is not a valid shader program variable.\n","MV");
	
	a_TexCoordHandle = glGetAttribLocation(mProgramID, "a_TexCoord");
   if (a_TexCoordHandle == -1)
      printf("%s is not a valid shader program variable.\n","a_TexCoord");
*/
	return true;
}

bool JShader::loadVert(std::string vertStr){
	mVS = loadShaderFromFile(vertStr,GL_VERTEX_SHADER);
   if (mVS == 0){
      printf("Invalid vertex shader.\n");
		return false;
	}
	return true;
}

bool JShader::loadFrag(std::string fragStr){
   mFS = loadShaderFromFile(fragStr,GL_FRAGMENT_SHADER);
   if (mFS == 0){
      printf("Invalid fragment shader.\n");
		return false;
	}
	return true;
}

GLint JShader::getRigMatHandle(){
	return u_RigMatHandle;
}

GLint JShader::getWeightHandle(){
	return a_WeightHandle;
}

GLint JShader::getPosHandle(){
	return a_PosHandle;
}

GLint JShader::getTexCoordHandle(){
	return a_TexCoordHandle;
}

GLint JShader::getMVHandle(){
	return u_MVHandle;
}

GLint JShader::getProjHandle(){
	return u_ProjHandle;
}

GLint JShader::getColorHandle(){
	return u_ColorHandle;
}

GLint JShader::getModeHandle(){
	return u_ModeHandle;
}

/*
void JShader::updateSampler(){
	glUniform1i(mTexHandle, 0);
}
*/

/*
void JShader::updateColor(GLfloat * color){
	glUniform4f(u_ColorHandle, color[0], color[1], color[2], color[3]);
}

void JShader::updateProj(GLfloat * proj){
   glUniformMatrix4fv(u_ProjHandle, 1, GL_FALSE, proj);
}

//Maybe do this by reference? Idk
void JShader::updateMVP(GLfloat * MVP){
	glUniformMatrix4fv(mMVPHandle, 1, GL_FALSE, MVP);
}

void JShader::updateMV(GLfloat * MV){
	glUniformMatrix4fv(u_MVHandle, 1, GL_FALSE, MV);
}
*/
