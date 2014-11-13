#ifndef CYCLE_H
#define CYCLE_H

#include "Pose.h"

class Cycle{
public:
   Cycle();
	Cycle(std::vector<Pose> poseVec, unsigned int C=1);
	Cycle(std::vector<Pose> poseVec, unsigned int C, float dt);
	void setPoses(std::vector<Pose> poseVec);
	void addPose(Pose p);
	void addPose(std::vector<Pose> poseVec);
	int getNumPoses();
	int getNumJoints();
   Cycle operator*(const float s);
   Cycle operator+(const Cycle& other);
	Cycle blend(const Cycle& other, float a);
   float * getPtr();
	Pose blendPoses(unsigned int p1, unsigned int p2, float x);
   Pose getCurrentPose(float& t);
	Pose collapsePoses(float t);
private:
   std::vector<Pose> poses;
	unsigned int C;
};

#endif
