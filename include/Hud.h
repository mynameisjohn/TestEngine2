#ifndef HUD_H
#define HUD_H

#include <Drawable.h>
#include <Player.h>

#include <vector>

enum placement{
	TOP_LEFT,
	TOP_RIGHT,
	BOT_LEFT,
	BOT_RIGHT
};

class Indicator{
protected:
	vec2 dim;
	vec4 color;
	string stencilTexture;
	Drawable * stencil;
	placement p;
public:
	Indicator();
	Indicator(vec2 dim, vec4 color, string sT, Drawable * drPtr, placement p);
	virtual void draw(mat4 pInv) = 0;
	virtual void update(Player * playerPtr) = 0;
	placement getPlacement();
};

class Ind_Chunks : public Indicator{
	int N, n, m;
public:
	Ind_Chunks();
	Ind_Chunks(vec2 dim, vec4 color, string sT, Drawable * drPtr, placement p,int N, int n, int m);
	void draw(mat4 pInv);
	void operator++();
	void operator--();
	void update(Player * playerPtr);
	bool isEmpty();
};

class Ind_Bar : public Indicator{
	float value;
public:
	Ind_Bar();
	Ind_Bar(vec2 dim, vec4 color, string sT, Drawable * drPtr, placement p);
	void setValue(float value);
	void draw(mat4 pInv);
	void update(Player * playerPtr);
};

class Hud{
public:
	Hud();
	void addIndicator(unique_ptr<Indicator> indPtR);
	void push_back(Drawable * drPtr);
	void update(Player * playerPtr);
	void draw(mat4 pInv);
private:
	uint8_t slots;
	std::vector<Drawable *> drawables;
	vector<unique_ptr<Indicator> > indicators;
};

#endif
