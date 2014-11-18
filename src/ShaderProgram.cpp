//also ripped off of lazy foo

#include <ShaderProgram.h>

#include <GL_Includes.h>

ShaderProgram::ShaderProgram(){
	mProgramID = (uint32_t)NULL;
}

ShaderProgram::~ShaderProgram(){
	freeProgram();
}

void ShaderProgram::freeProgram(){
	glDeleteProgram(mProgramID);
}

bool ShaderProgram::bind(){
	glUseProgram(mProgramID);
	uint32_t err = glGetError();
	if (err != GL_NO_ERROR){
		printf( "Error binding shader! %s\n", gluErrorString(err));
		printProgramLog(mProgramID);
		return false;
	}

	return true;
}

void ShaderProgram::unbind(){
	glUseProgram((uint32_t)NULL);
}

uint32_t ShaderProgram::getProgramID(){
	return mProgramID;
}

void ShaderProgram::printProgramLog(uint32_t program){
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

void ShaderProgram::printShaderLog(uint32_t shader){
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

uint32_t ShaderProgram::loadShaderFromFile(std::string path, uint32_t shaderType){
	uint32_t shaderID=0;
	std::string shaderString;
	std::ifstream sourceFile(path.c_str());

	if (sourceFile){
		shaderString.assign((std::istreambuf_iterator<char>(sourceFile)),
									std::istreambuf_iterator<char>());
		shaderID = glCreateShader(shaderType);
		const GLchar * shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);
		glCompileShader(shaderID);
		int shaderCompiled = GL_FALSE;
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

int ShaderProgram::getAttribHandle(std::string name){
	int handle = glGetAttribLocation(mProgramID, name.c_str());
	if (handle==-1){
		printf("%s is not a valid shader program variable.\n",name.c_str());
		return -1;
	}
	return handle;
}

int ShaderProgram::getUniformHandle(std::string name){
	int handle = glGetUniformLocation(mProgramID, name.c_str());
	if (handle==-1){
		printf("%s is not a valid shader program variable.\n",name.c_str());
		return -1;
	}
	return handle;
}
