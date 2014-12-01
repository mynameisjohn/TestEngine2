#ifndef TEXTURES_H
#define TEXTURES_H
#include <string>
#include <vector>
#include <SDL.h>

uint32_t initTexture(void * PXA, int w, int h);
uint32_t fromImage(std::string fileName);
uint32_t outlineTexture(bool invert = false);
uint32_t fromSDLSurface(SDL_Surface * s);
std::vector<uint32_t> fromFileList(std::vector<std::string> files);

#endif
