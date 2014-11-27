#ifndef CLOSET_H
#define CLOSET_H

#include <Geom.h>
#include <Skeleton.h>

#include <unordered_map>
#include <functional>

class Closet{
public:
	Closet(unordered_map<string, unique_ptr<Drawable> > * dMP = nullptr, JShader * shader = nullptr);
	Skeleton createSkeleton(TiXmlElement *, JShader&);
	Ligament createLigament(TiXmlElement * el, JShader& shader);
	uint32_t fill(vector<Ligament>& lVec, unordered_map<string, uint32_t>& nameMap, TiXmlElement * el, JShader& shader);
private:
	unordered_map<string, unique_ptr<Drawable> > * dMapPtr;
	unordered_map<string, function<Drawable(string, JShader&)> > drFnMap;

};

#endif
