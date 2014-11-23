#ifndef TEXTURES_H
#define TEXTURES_H
#include <string>
#include <vector>

uint32_t fromImage(std::string fileName);
uint32_t outlineTexture(bool invert = false);
std::vector<uint32_t> fromFileList(std::vector<std::string> files);

#endif
