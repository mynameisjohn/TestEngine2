#ifndef TEXTURES_H
#define TEXTURES_H
#include <string>
#include <vector>
#include <SDL.h>

#include <Util.h>

uint32_t initTexture(void * PXA, int w, int h);
uint32_t fromImage(std::string fileName);
uint32_t outlineTexture(bool invert = false);
uint32_t fromSDLSurface(SDL_Surface * s);
uint32_t fromTextString(std::string text, uint32_t fontSize=100, vec3 textColor = vec3(), vec3 bgColor = vec3(1), vec2 border = vec2(0.1f));
std::vector<uint32_t> fromFileList(std::vector<std::string> files);

#endif
