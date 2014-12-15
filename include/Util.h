#ifndef UTIL_H
#define UTIL_H

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef M_PI
#define M_PI 3.14159f
#endif

#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <vector>
#include <sstream>

using glm::fquat;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

using namespace std;

const string IMG_DIR("res/img/");
const string OBJ_DIR("res/obj/");
const string RIG_DIR("res/rig/");
const string SVG_DIR("res/svg/");
const string SPR_DIR("res/sprt/");
const string ENT_DIR("res/entities/");
const string FONT_DIR("res/fonts/");

int wrap(int dim, int num);
float min(float v1, float v2);
float max(float v1, float v2);
float clamp(float v, float m, float M);
float degToRad(float d);
float radToDeg(float r);
float lagrangeTime(float t0, float t1, float t);
float lerp(float x, float y, float a);

float min(vec2 v);
float max(vec2 v);
float min(vec3 v);
float max(vec3 v);
float min(vec4 v);
float max(vec4 v);

uint32_t getRGBAInt(vec4 c);
uint32_t getRGBAInt(vec3 c);

void printError(string name);

template <typename T>
bool fillIn(T& v, string s){
	return (bool)(stringstream(s) >> v);
}

template bool fillIn<uint32_t>(uint32_t&, string);
template bool fillIn<float>(float&, string);
template bool fillIn<bool>(bool&, string);

void fillVec(vec2& v, string s);
void fillVec(vec3& v, string s);
void fillVec(vec4& v, string s);

vec2 normalizeVec(vector<vec2>& v, bool shape=false);
vec3 normalizeVec(vector<vec3>& v, bool shape=false);
vec4 normalizeVec(vector<vec4>& v, bool shape=false);

mat4 getAlignMat(vec3 x, vec3 y, vec3 z);
mat4 getAlignMat(vec3 u, char o);

fquat getQuatFromVec2(vec2 r);

template <typename T>
T remap(T v, T m1, T m2, T m3, T m4){
	return (((v-m1)/(m2-m1))*(m4-m3)+m3);
}

template float remap<float>(float, float, float, float, float);
template vec2 remap<vec2>(vec2,vec2,vec2,vec2,vec2);
template vec3 remap<vec3>(vec3,vec3,vec3,vec3,vec3);
template vec4 remap<vec4>(vec4,vec4,vec4,vec4,vec4);

template <typename T>
T centroid(vector<T>& v){
	T a(0);
	typename vector<T>::iterator it;
	for (it=v.begin(); it!=v.end(); it++){
		a = a + *it;
	}
	a = a/(float)(v.size());

	return a;
}
template float centroid<float>(vector<float>&);
template vec2 centroid<vec2>(vector<vec2>&);
template vec3 centroid<vec3>(vector<vec3>&);
template vec4 centroid<vec4>(vector<vec4>&);

fquat getRQ(vec4 rot);

vec3 getLagrangeInterpolants(float x);

ostream& operator<<(ostream& os, const vec2& vec);
ostream& operator<<(ostream& os, const vec3& vec);
ostream& operator<<(ostream& os, const vec4& vec);
ostream& operator<<(ostream& os, const mat4& mat);
ostream& operator<<(ostream& os, const fquat& quat);

#endif
