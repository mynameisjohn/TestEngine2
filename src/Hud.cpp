#include <Hud.h>
#include <glm/gtx/transform.hpp>

mat4 getPlacementMat(placement p, vec2 scale){
	vec3 translate;
	scale = glm::abs(scale);
	switch(p){
		case TOP_LEFT:
			translate = vec3(vec2(-1+scale.x,1)-scale, -.9);
			break;
		case TOP_RIGHT:
			translate = vec3(vec2(1)-scale, 0);
			break;
		case BOT_LEFT:
			translate = vec3(-1,-1,0);
			break;
		case BOT_RIGHT:
			translate = vec3(vec2(1-scale.x,-1),0);
			break;
		default:
			return mat4();
	}
	return glm::translate(translate)*glm::scale(vec3(scale,1));
}

Indicator::Indicator(){
};

Indicator::Indicator(vec2 dim, vec4 color, string sT, Drawable * stPtr, placement p)
: dim(dim), color(color), stencilTexture(sT), stencil(stPtr), p(p){
}

placement Indicator::getPlacement(){
	return p;
}

Ind_Chunks::Ind_Chunks()
: Indicator(), N(1), n(1), m(1){
}

Ind_Chunks::Ind_Chunks(vec2 dim, vec4 color, string sT, Drawable * drPtr, placement p,int N, int n, int m)
: Indicator(dim, color, sT, drPtr, p), N(N), n(n), m(m){
	
}

void Ind_Chunks::draw(mat4 pInv){
	vec2 scale(dim.x/(float)N,dim.y);
	mat4 MV;
	vec4 c(0,0,0,1);
	for (int i=0;i<N;i++){
		c.x += 1-(float)i/(float)N;
		c.y += (float)i/(float)N;
		c.z += (float)(pow((float)i/(float)N,2));
		if (p==TOP_RIGHT || p==BOT_RIGHT)
			MV =  pInv*
					glm::translate(vec3(-dim.x*(float)(N-1)/(float)N+(float)i*dim.x/(float)N,0,0.05))*getPlacementMat(p, scale);
		else
			MV = pInv*glm::translate(vec3((float)i*dim.x/(float)N,0,0.05))*getPlacementMat(p, scale);
	
		stencil->uploadColor(c);
		stencil->uploadMV(MV);
		stencil->draw();
		MV = glm::translate(vec3(0,0,0.05))*MV;
		stencil->uploadMV(MV);
		stencil->draw(stencilTexture);
	}
}

void Ind_Chunks::update(Player * playerPtr){
//NYI
}

Ind_Bar::Ind_Bar()
: Indicator(){
}

Ind_Bar::Ind_Bar(vec2 dim, vec4 color, string sT, Drawable * stPtr, placement p)
: Indicator(dim, color, sT, stPtr, p), value(0.5){
}

void Ind_Bar::setValue(float value){
	this->value = value;
}

void Ind_Bar::update(Player * playerPtr){
	setValue(playerPtr->getCharge());
}

void Ind_Bar::draw(mat4 pInv){
//	static float osc(0);
//	osc += 0.1f;
//	value = 0.5f*sin(osc)+0.5f;
	vec2 scale(dim.x, dim.y);
	mat4 MV = pInv*glm::translate(vec3(-(1-value)*dim.x,0,.05))*getPlacementMat(p,vec2(scale.x*value,scale.y));
	stencil->uploadMV(MV);
	stencil->uploadColor(color);
	stencil->draw();
}

Hud::Hud()
: slots(0){
}

void Hud::push_back(Drawable * drPtr){
}


void Hud::addIndicator(unique_ptr<Indicator> indPtr){
	if (slots & (1 << indPtr->getPlacement()))
		cout << "Warning: attempt to overwrite existing slot occurred" << endl;
	else{
		slots |= (1 << indPtr->getPlacement());
		indicators.push_back(move(indPtr));
	}
}

void Hud::draw(mat4 pInv){
	vector<unique_ptr<Indicator> >::iterator it;
	for (it=indicators.begin(); it!=indicators.end(); it++)
		(*it)->draw(pInv);
}

void Hud::update(Player * playerPtr){
	vector<unique_ptr<Indicator> >::iterator it;
	for (it=indicators.begin(); it!=indicators.end(); it++)
		(*it)->update(playerPtr);
}
