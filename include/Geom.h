#ifndef GEOM_H
#define GEOM_H

#include <map>

#include <Rig.h>
#include <XMLReader.h>

Drawable initQuad(JShader& shader);
Drawable initTexQuad(string fileName, JShader& shader);
Drawable initCube(JShader& shader);
Drawable initPolyFromSVG(string fileName, JShader& shader);
Drawable initSpriteSheet(string fileName, JShader& shader);
Drawable initObj(string fileName, JShader& shader);
Rig initRigFromSVG(string fileName, JShader& shader);
GLuint genVAO(geoInfo gI, JShader& shader);

#endif
