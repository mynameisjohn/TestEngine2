#ifndef MAIN_H
#define MAIN_H

#include <BaseEngine.h>

//Apple demands OpenGL 3.3, and needs more modern shaders
#ifdef __APPLE__
const std::string vertexShaderSrc = "shaders/mac_vertShader.glsl";
const std::string fragShaderSrc = "shaders/mac_fragShader.glsl";
const int glMajor(3), glMinor(3);
#else
const std::string vertexShaderSrc = "shaders/vertShader.glsl";
const std::string fragShaderSrc = "shaders/fragShader.glsl";
const int glMajor(3), glMinor(0);
#endif

bool init(BaseEngine& engine);
void close();

#endif
