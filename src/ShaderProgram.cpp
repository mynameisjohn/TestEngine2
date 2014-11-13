//also ripped off of lazy foo

#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(){
	mProgramID = (GLuint)NULL;
}

ShaderProgram::~ShaderProgram(){
	freeProgram();
}

void ShaderProgram::freeProgram(){
	glDeleteProgram(mProgramID);
}

bool ShaderProgram::bind(){
	glUseProgram(mProgramID);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR){
		printf( "Error binding shader! %s\n", gluErrorString(err));
		printProgramLog(mProgramID);
		return false;
	}

	return true;
}

void ShaderProgram::unbind(){
	glUseProgram((GLuint)NULL);
}

GLuint ShaderProgram::getProgramID(){
	return mProgramID;
}

void ShaderProgram::printProgramLog(GLuint program){
	if (glIsProgram(program)){
		int infoLogLength=0, maxLength=0;
		char * infoLog;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		infoLog = new char[maxLength];
		glGetProgramInfoLog(program,maxLength,&infoLogLength,infoLog);

		if (infoLogLength>0)
			printf("%s\n", infoLog);

		delete[] infoLog;
	}
	else
		printf("%d did not reference a program. \n",program);

	return;
}

void ShaderProgram::printShaderLog(GLuint shader){
	if (glIsShader(shader)){
      int infoLogLength=0, maxLength=0;
      char * infoLog;

      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
      infoLog = new char[maxLength];
      glGetShaderInfoLog(shader,maxLength,&infoLogLength,infoLog);

      if (infoLogLength>0)
         printf("%s\n", infoLog);

      delete[] infoLog;
   }
   else
      printf("%d did not reference a shader. \n",shader);

   return;
}

GLuint ShaderProgram::loadShaderFromFile(std::string path, GLenum shaderType){
	GLuint shaderID=0;
	std::string shaderString;
	std::ifstream sourceFile(path.c_str());

	if (sourceFile){
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)),
									std::istreambuf_iterator<char>());
		shaderID = glCreateShader(shaderType);
		const GLchar * shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);
		glCompileShader(shaderID);
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
		if (shaderCompiled != GL_TRUE){
			printf("Unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource);
			printShaderLog(shaderID);
			glDeleteShader(shaderID);
			shaderID = 0;
		}
	}
	else
		printf( "Unable to open file %s\n", path.c_str() );

	return shaderID;
}

GLint ShaderProgram::getAttribHandle(std::string name){
	GLint handle = glGetAttribLocation(mProgramID, name.c_str());
	if (handle==-1){
		printf("%s is not a valid shader program variable.\n",name.c_str());
		return -1;
	}
	return handle;
}

GLint ShaderProgram::getUniformHandle(std::string name){
	GLint handle = glGetUniformLocation(mProgramID, name.c_str());
	if (handle==-1){
		printf("%s is not a valid shader program variable.\n",name.c_str());
		return -1;
	}
	return handle;
}
