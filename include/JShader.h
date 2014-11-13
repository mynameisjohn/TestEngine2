#ifndef J_SHADER_H
#define J_SHADER_H

#include "ShaderProgram.h"

class JShader : public ShaderProgram{
	public:
		JShader();                         //Default Constructor
		bool loadProgram();                //Load Program
		bool loadVert(std::string vertStr);//Load Vert Shader from string
		bool loadFrag(std::string fragStr);//Load Frag Shader from string
		GLint getPosHandle();              //get vertex position handle
		GLint getTexCoordHandle();
		GLint getMVHandle();
		GLint getProjHandle();
		GLint getColorHandle();
		GLint getModeHandle();
		GLint getWeightHandle();
		GLint getRigMatHandle();
	private:
		GLint a_PosHandle;  //Handle to Vertex Position
		GLint a_TexCoordHandle; //Handle to Texture Coordinates
		GLint a_WeightHandle; //Handle to Rig Weights
		GLint u_MVHandle;   //Handle to MVP matrix
		GLint u_ProjHandle; //Handle to Projection Matrix
		GLint u_RigMatHandle;   //Handle to MVP matrix
		GLint u_ColorHandle;//Handle to Frag Color (needed?)
		GLint u_ModeHandle;//Handle to Frag Color (needed?)
		GLuint mVS, mFS;   //References to vertex and fragment shaders
};

#endif
