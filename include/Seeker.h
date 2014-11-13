#ifndef SEEKER_H
#define SEEKER_H

#include "ActiveEnt.h"

typedef struct PotentialInfo{
	float sig;
	float A;
} PotInfo;

class Seeker : public ActiveEnt{
public:
	Seeker();
	Seeker(const Entity& e);
	Seeker(vec3, vec3);
	void update();//vec3);
	void addPotential(Entity * e);
	void predispose(Entity * e, PotInfo pI);
	void takeHit(HitEffect eff);
	bool overlapsWith(Entity * e);
protected:
	unordered_map<Entity *, PotInfo> dispositions;
	bool attacking;
	BoundRect aRect;
};

#endif
