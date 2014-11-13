//This class was blatantly 
//ripped off from LazyFoo.
//sorry. 

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <fstream>

class ShaderProgram
{
	public:
		ShaderProgram();                  //Default Constructor
		virtual ~ShaderProgram();         //Default Destructor
		virtual bool loadProgram() = 0;   //Load Program
		virtual void freeProgram();       //Free Program
		bool bind();                      //Bind Shader for use
		void unbind();                    //Unbind Shader
		GLuint getProgramID();            //Get Program ID

	protected:
		void printProgramLog(GLuint program); //Print Program Log
		void printShaderLog(GLuint shader);   //Print Shader Log
		GLint getAttribHandle(std::string name);
		GLint getUniformHandle(std::string name);
		GLuint loadShaderFromFile(std::string path, GLenum shaderType); //Load Shader from Text File
		GLuint mProgramID; //Program ID
};

#endif
