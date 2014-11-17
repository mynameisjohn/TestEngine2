#include <Util.h>
#include <Cycle.h>

Cycle::Cycle(){
	poses = vector<Pose>();
}

Cycle::Cycle(vector<Pose> poseVec, unsigned int C)
: poses(poseVec), C(C){
}

Cycle::Cycle(vector<Pose> poseVec, unsigned int C, float dt)
: poses(poseVec), C(C){
//	cout << poses.size() << endl;
	const float m = 0.01f, M=0.1f;
	
	//Centripetal DT calculation
	for (uint16_t i=0;i<poses.size();i++){
		int i2 = (i+1)%poses.size();
		float DT = clamp(dt*sqrt(poses[i].maxDiff(poses[i2])), m, M);
		poses[i].setDT(DT);
//		cout << DT << endl;
	}
	C = (poses.size() > C ? C : poses.size()-1);
}

void Cycle::setPoses(vector<Pose> poseVec){
	if (poseVec.size()==3)
		poses=poseVec;
}

void Cycle::addPose(Pose p){
	if (poses.size() < 3)
		poses.push_back(p);
}

void Cycle::addPose(vector<Pose> poseVec){
	for (uint16_t i=0;i<poseVec.size();i++)
		if (poses.size() < 3)
			poses.push_back(poseVec[i]);
}

int Cycle::getNumPoses(){
	return poses.size();
}

int Cycle::getNumJoints(){
	uint16_t i=0;
	int nJ = poses[i].getNumJoints();
	for (i=1;i<poses.size();i++){
		if (nJ != poses[i].getNumJoints()){
			cout << "Inconsistent number of Joints. Segfault iminent." << endl;
			return 0;
		}
	}
	return nJ;
}

Cycle Cycle::operator*(const float s){

	Cycle ret(*this);
	for (uint16_t i=0;i<ret.poses.size();i++)
		ret.poses[i] = this->poses[i]*s;
	return ret;
}

Cycle Cycle::operator+(const Cycle& other){
	Cycle ret(*this);
	for (uint16_t i=0;i<ret.poses.size();i++)
		ret.poses[i] = this->poses[i]+other.poses[i];
	return ret;
}

Cycle Cycle::blend(const Cycle& other, float a){
	vector<Pose> poseVec(poses.size());
	for (uint16_t i=0;i<poseVec.size();i++)
		poseVec[i] = poses[i].blend(other.poses[i], a);
	return Cycle(poseVec, C);
}

Pose Cycle::blendPoses(unsigned int p1, unsigned int p2, float x){
	return poses[p1].blend(poses[p2],x);
}

//I'm fairly certain this doesn't work for an even C
//I've read that float -> int is rather slow
Pose Cycle::collapsePoses(float t){
	int top, bottom, center;
	bool red(true);

	bottom = -(C/2);
   top = C/2+(C%2!=0);
	center = (int)t;
	vector<Pose> tmp(top-bottom); 
	for (int i=bottom,k=0;i<top;i++,k++){
      int i1=center+i, i2=i1+1;
		float x = lagrangeTime((float)i1, (float)i2, t);
      i1 = wrap(poses.size(),i1);
      i2 = wrap(poses.size(),i2);
		tmp[k]=blendPoses(i1,i2,x);//lagrangeTime((float)i1, (float)i2, t));
	}

	for (uint16_t j=1;j<C;j++){
      for (uint16_t i=bottom+j/2, k=0;i<top-(j+1)/2;i++, k++){
         int t1 = center+i, t2 = t1+red+1;
			float x = lagrangeTime((float)t1, (float)(t2), t);
         t1 = wrap(poses.size(),t1);
         t2 = wrap(poses.size(),t2);
         tmp[k]=tmp[k].blend(tmp[k+1],x);//lagrangeTime((float)t1, (float)t2, t));
      }
      red = !red;
   }
	return tmp[0];
}

//this is slow but works for now... just like everything else
Pose Cycle::getCurrentPose(float& t){
	Pose ret = (poses.size () > 1) ? collapsePoses(t) : poses[0];
	t += ret.getDT();//0.1f;
	return ret;
}
