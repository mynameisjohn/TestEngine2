//This class was blatantly 
//ripped off from LazyFoo.
//sorry. 

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <fstream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
typedef unsigned __int32 uint32_t;
#endif

class ShaderProgram
{
	public:
		ShaderProgram();                  //Default Constructor
		virtual ~ShaderProgram();         //Default Destructor
		virtual bool loadProgram() = 0;   //Load Program
		virtual void freeProgram();       //Free Program
		bool bind();                      //Bind Shader for use
		void unbind();                    //Unbind Shader
		uint32_t getProgramID();            //Get Program ID

	protected:
		void printProgramLog(uint32_t program); //Print Program Log
		void printShaderLog(uint32_t shader);   //Print Shader Log
		int getAttribHandle(std::string name);
		int getUniformHandle(std::string name);
		uint32_t loadShaderFromFile(std::string path, uint32_t shaderType); //Load Shader from Text File
		uint32_t mProgramID; //Program ID
};

#endif
