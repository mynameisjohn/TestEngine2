#ifndef TEXTURES_H
#define TEXTURES_H
#include <string>
#include <vector>

int * outline(int DIM);
int * invertedOutline(int DIM);
unsigned int fromImage(std::string fileName);
unsigned int outlineTexture();
unsigned int invertedOutlineTexture();
std::vector<unsigned int> fromFileList(std::vector<std::string> files);

#endif
