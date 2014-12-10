#ifndef HUD_H
#define HUD_H

#include <Drawable.h>
#include <Player.h>

#include <vector>

enum placement{
	TOP_LEFT  = 1,
	TOP_RIGHT = 2,
	BOT_LEFT  = 4,
	BOT_RIGHT = 8
};

class Indicator{
protected:
	float value;
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
	void setValue(float value);
	placement getPlacement();
};

class Ind_Chunks : public Indicator{
	int N, n, m;
public:
	Ind_Chunks();
	Ind_Chunks(vec2 dim, vec4 color, string sT, Drawable * drPtr, placement p,int N, int n, int m);
	void draw(mat4 pInv);
	void update(Player * playerPtr);
	bool isEmpty();
};

class Ind_Bar : public Indicator{
public:
	Ind_Bar();
	Ind_Bar(vec2 dim, vec4 color, string sT, Drawable * drPtr, placement p);
	void draw(mat4 pInv);
	void update(Player * playerPtr);
};

class Hud{
public:
	Hud();
	void addIndicator(unique_ptr<Indicator> indPtR);
	void update(Player * playerPtr);
	void draw(mat4 pInv);
	void setValue(float value);
private:
	uint8_t slots;
	std::vector<Drawable *> drawables;
	vector<unique_ptr<Indicator> > indicators;
};

#endif
