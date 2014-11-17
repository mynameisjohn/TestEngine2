#ifndef J_SHADER_H
#define J_SHADER_H

#include <ShaderProgram.h>

class JShader : public ShaderProgram{
	public:
		JShader();                         //Default Constructor
		bool loadProgram();                //Load Program
		bool loadVert(std::string vertStr);//Load Vert Shader from string
		bool loadFrag(std::string fragStr);//Load Frag Shader from string
		int getPosHandle();              //get vertex position handle
		int getTexCoordHandle();
		int getMVHandle();
		int getProjHandle();
		int getColorHandle();
		int getModeHandle();
		int getWeightHandle();
		int getRigMatHandle();
	private:
		int a_PosHandle;  //Handle to Vertex Position
		int a_TexCoordHandle; //Handle to Texture Coordinates
		int a_WeightHandle; //Handle to Rig Weights
		int u_MVHandle;   //Handle to MVP matrix
		int u_ProjHandle; //Handle to Projection Matrix
		int u_RigMatHandle;   //Handle to MVP matrix
		int u_ColorHandle;//Handle to Frag Color (needed?)
		int u_ModeHandle;//Handle to Frag Color (needed?)
		uint32_t mVS, mFS;   //References to vertex and fragment shaders
};

#endif
