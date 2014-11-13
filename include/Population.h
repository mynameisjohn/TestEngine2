#ifndef POPULATION_H
#define POPULATION_H

#include "Player.h"
#include "Obstacle.h"
#include "Seeker.h"

typedef vector<unique_ptr<Obstacle> > ObsPtrVec;
typedef vector<unique_ptr<ActiveEnt> > AePtrVec;
typedef vector<unique_ptr<Seeker> > SeekPtrVec;

using glm::vec4;

class Population{
	public:
		Population();
		Population(Player p, vector<Obstacle>, vector<Seeker>);
		Population(unique_ptr<Player> p);
		~Population();
		void setPlayer(unique_ptr<Player> p);
		void update();//EventInfo);
		void draw();//int, int);
		void addObs(unique_ptr<Obstacle> obs);
		void addActiveEnt(unique_ptr<ActiveEnt> aE);
		void addSeeker(unique_ptr<Seeker> seek);
		void predispose();
		vec4 move();
		vec3 getPlayerCenter();
		Player * getPlayer();
	private:
//		Player * player;
		unique_ptr<Player> player;
		ObsPtrVec obsVec;
		AePtrVec aeVec;
		SeekPtrVec seekVec;
};

#endif
