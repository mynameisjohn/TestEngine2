#ifndef COLLIDER_H
#define COLLIDER_H

#include <vector>
#include <unordered_map>

#include <BoundBox.h>
#include <BoundRect.h>
#include <ColBuf.h>

class Collider{
	public:
		Collider();
		Collider(BoundBox bb, float soft=300);
		~Collider();
		void setBB(BoundBox);
		void addSub(BoundRect sub);
		void setWalls(vec3 min, vec3 max);
		void clearSub();
		void translate(vec3 trans);
		void moveTo(vec3 pos);
		void scale(vec3 s);
		void print();
	   char collidesWith(Collider&);
		bool collidesX(Collider&);
      bool collidesY(Collider&);
		bool collidesZ(Collider&);
		bool overlapsWith(BoundRect rect);
		bool overlapsWith(Collider&);
		bool softCol(Collider& other);
		float toLeft(Collider& c);
		float toRight(Collider& c);
		float toBottom(Collider& c);
		float toTop(Collider& c);
		float toNear(Collider& c);
		float toFar(Collider& c);
		vec3 move(vec3 vel);
		vec3 getPos();
		vec3 center();
		vec3 getDim();
		vector<BoundRect> getSubs();
	private:
		BoundBox mBB;
		std::vector<BoundRect> mSubs;
		float soft;//		vec2 soft;
		vec3 W_min, W_max;
		std::unordered_map<Collider *, ColBuf> cBufMap;
	public:
		/* 
			I'll re-inline these when the time comes
		inline vec3 getPos(){
			return mBB.getPos();
		}
		inline int toLeft(Collider& c){
			return c.mBB.left()-mBB.right(); 
		}
		inline int toRight(Collider& c){
         return c.mBB.right()-mBB.left(); 
      }
		inline int toBottom(Collider& c){
         return c.mBB.bottom()-mBB.top(); 
      }
		inline int toTop(Collider& c){
         return c.mBB.top()-mBB.bottom(); 
      }
		inline int toNear(Collider& c){
			return c.mBB.near() - mBB.far();
		}
		inline int toFar(Collider& c){
         return c.mBB.near()-mBB.far(); 
      }
		inline vec3 center(){
			return mBB.center();
		}
		*/
};

#endif
