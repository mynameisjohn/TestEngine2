#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <JShader.h>
#include <Util.h>

#include <vector>
#include <unordered_map>

class Drawable{
	public:
		Drawable(JShader * shader = nullptr, int mode=-1, vec4 o = vec4());
		void setVAO(uint32_t VAO);
		void setMode(int mode);
		void setOrigin(vec4 o);
		void setOrigins(vector<vec4> vO);
		void setNElements(int n);
		bool isVisible();
		void setVisibility(bool);
		bool addTex(string s, uint32_t tex);
		bool addTex(vector<pair<string, uint32_t> > texVec);
		void uploadMV(mat4& MV);
		void uploadColor(vec4& c);
		bool changeTextureSettings(string setting, uint32_t value);
		int getNumElems();
		uint32_t getVAO();
		uint32_t getTex();
		virtual void draw(string currentTex="");
		vec4 getOrigin(mat4 MV = mat4());
		vector<vec4> getOrigins(mat4 MV = mat4());
	protected:
		JShader * mShader;
		int mMode;
		uint32_t mVAO;//,mTex;
		unordered_map<string, uint32_t> texMap;
		int mElementCount;
		vector<vec4> origins;
	private:
		static unordered_map<string, uint32_t> TextureSettings;
};

#endif
