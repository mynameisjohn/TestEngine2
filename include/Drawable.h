#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "JShader.h"
#include "Util.h"

#include <vector>
#include <unordered_map>

class Drawable{
	public:
		Drawable(JShader * shader = nullptr, int mode=-1, vec4 o = vec4());
		void translate(GLfloat x, GLfloat y, GLfloat z);
		void setVAO(GLuint VAO);
		void setMode(int mode);
		void setOrigin(vec4 o);
		void setOrigins(vector<vec4> vO);
		void setNElements(int n);
		bool isVisible();
		void setVisibility(bool);
		void addTex(string s, GLuint tex);
		void addTex(vector<pair<string, GLuint> > texVec);
		void uploadMV(mat4& MV);
		void uploadColor(vec4& c);
		int getNumElems();
		GLuint getVAO();
		GLuint getTex();
		virtual void draw(string currentTex="");
		vec4 getOrigin(mat4 MV = mat4());
		vector<vec4> getOrigins(mat4 MV = mat4());
	protected:
		JShader * mShader;
		int mMode;
		GLuint mVAO;//,mTex;
		unordered_map<string, GLuint> texMap;
		int mElementCount;
		vector<vec4> origins;
};

#endif
