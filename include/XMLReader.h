#ifndef XML_READER_H
#define XML_READER_H

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

#include <tinyxml.h>
#include <map>

#include <Util.h>
#include <Rig.h>

//phase this out
typedef struct triangle{
   unsigned int A,B,C;
} triangle;

typedef struct geoInfo{
	vector<vec4> vertices;
	vector<vec2> texCoords;
	vector<triangle> indices;
	vector<vec3> weights;
	vec3 offset;
	vector<vec4> origins;
} geoInfo;

vector<triangle> getConvexIndices(uint32_t n);
Rig getRigFromSVG(string svgFile, JShader& shader);
map<string, string> getSVGPathMap(string svgFile);
geoInfo SVGtoGeometry(string svgFile, bool rigged);
vector<string> getSprtFileList(string sprtFile);

#endif
