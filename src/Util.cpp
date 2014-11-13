#include "Util.h"
#include <sstream>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

void printError(string name){
	cout << "Unable to load file " << name << ". Segfault imminent" <<  endl;
}

void fillVec(vec2& v, string s){
	size_t pos(0);
	const string d(",");
	for (int i=0;i<2;i++){
      pos = s.find(d);
      if (!(stringstream(s.substr(0,pos)) >> v[i]))
         cout << "Invalid vector string " << s << endl;
      s.erase(0,pos+d.length());
   }
}

void fillVec(vec3& v, string s){
	size_t pos(0);
	const string d(",");
	for (int i=0;i<3;i++){
      pos = s.find(d);
      if (!(stringstream(s.substr(0,pos)) >> v[i]))
         cout << "Invalid vector string " << s << endl;
      s.erase(0,pos+d.length());
   }

}void fillVec(vec4& v, string s){
	size_t pos(0);
	const string d(",");
	for (int i=0;i<4;i++){
      pos = s.find(d);
      if (!(stringstream(s.substr(0,pos)) >> v[i]))
         cout << "Invalid vector string " << s << endl;
      s.erase(0,pos+d.length());
   }
}

float min(vec3 v){
	float m(v[0]);
	for (int i=1;i<3;i++)
		if (m > v[i])
			m = v[i];
	return m;
}

float max(vec3 v){
	float M(v[0]);
	for (int i=1;i<3;i++)
		if (M < v[i])
			M = v[i];
	return M;
}

int wrap(int dim, int num){
	return (dim+num)%dim;
}

float min(float v1, float v2){
   return (v1 < v2 ? v1 : v2);
}
float max(float v1, float v2){
   return (v1 > v2 ? v1 : v2);
}
float clamp(float v, float m, float M){
   return min(max(v,m),M);
}

float degToRad(float d){
	return (float)d*M_PI/180.f;
}

float radToDeg(float r){
	return (float)(r*180.f)/((float)M_PI);
}

float lagrangeTime(float t0, float t1, float t){
	return (t-t0)/(t1-t0);
}

float lerp(float x, float y, float a){
	return x*(1.f-a)+a*y;
}

mat4 getAlignMat(vec3 x, vec3 y, vec3 z){
	return glm::transpose(mat4(vec4(glm::normalize(x),0), 
										vec4(glm::normalize(y),0), 
										vec4(glm::normalize(z),0), 
										vec4(vec3(),1)));
}

mat4 getAlignMat(vec3 u, char o){
	switch (o){
		case 'x':{
			vec3 x = u;
			vec3 y = vec3(u.y,-u.x,0);
			vec3 z = glm::cross(x,y);
			return getAlignMat(x,y,z);
		}
		case 'y':{
			vec3 x = vec3(-u.y,u.x,0);
			vec3 y = u;
			vec3 z = glm::cross(x,y);
			return getAlignMat(x,y,z);
		}
		case 'z':{
			vec3 x = vec3(u.y,-u.x,0); //glm::normalize(u);
			vec3 y = glm::cross(u,x); //glm::normalize(vec3(u.y,-u.x,0));
			vec3 z = u;//glm::cross(x,y));
			return getAlignMat(x,y,z);
		}
		default:
			cout << "Incorrect Alignment Specification" << endl;
			return mat4();
	}
}

//This returns a rotation quat that will line something
//up with the given vec2 in x,y (meaning rotation is about z)
fquat getQuatFromVec2(vec2 r){
	r = glm::normalize(r);
	float s(sqrt((1-r.x)/2));
	float c((r.y>0)?sqrt((1+r.x)/2):-sqrt((1+r.x)/2));
	
	return fquat(c,s*vec3(0,0,1));
}

fquat getRQ(vec4 rot){
	vec3 axis = glm::normalize(vec3(rot.y, rot.z, rot.w));
   float angle = 0.5f*degToRad(rot.x);
	
	return fquat(cos(angle),sin(angle)*axis);
}

vec3 getLagrangeInterpolants(float x){
   x = clamp(x,-1.f,1.f);
   return vec3(-0.5f*x*(1.f-x), 1.f-x*x, 0.5f*x*(1.f+x));
}

ostream& operator<<(ostream& os, const vec2& vec){
	os << "{" << vec.x << ", " << vec.y << "}";
   return os;
}

ostream& operator<<(ostream& os, const vec3& vec){
  os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
  return os;
}

ostream& operator<<(ostream& os, const vec4& vec){
  os << "{" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "}";
  return os;
}

ostream& operator<<(ostream& os, const mat4& mat){
  mat4 transMat = glm::transpose(mat);
  os << "{\n" << transMat[0] << ",\n" << transMat[1] << ",\n" << transMat[2] << ",\n" << transMat[3] << ",\n}";
  return os;
}

ostream& operator<<(ostream& os, const fquat& quat){
  os << "{" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << "}";
  return os;
}
