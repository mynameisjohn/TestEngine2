#include <Hud.h>
#include <glm/gtx/transform.hpp>

/*
TO DO:
	- Make everything less wordy, i.e how the matrices are formed. 
	- Speaking of matrices, add a branch to the shader designed for world space
	  (this will be useful for the menu as well)
*/

mat4 getPlacementMat(placement p, vec2 scale){
	vec3 translate;
//	scale = glm::abs(scale); //commented out for now...
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
	vec4 c(1,.1,.1,1);
	stencil->uploadColor(c);

	//The idea is to get the health (as a percentage)
	//and draw quads up until that value
	int maxIdx((int)(value*(float)N));
	float yScale(dim.y*(value*((float)N) - (float)(maxIdx)));

	int i(0);
	for (; i<N; i++){
		//just draw quads
		float xTrans( (p==TOP_RIGHT || p==BOT_RIGHT) ? 
				-dim.x*(float)(N-1)/(float)N+(float)i*dim.x/(float)N :
				(float)i*dim.x/(float)N);
		//Everything is full up until maxIdx
		if (i < maxIdx){
			MV = pInv*glm::translate(vec3(xTrans,0,0.05))*
															getPlacementMat(p, scale);
			stencil->uploadMV(MV);
			stencil->draw();
		}
		//If we are on the last chunk
		else if (i == maxIdx){ //This grows down right now...
			MV = pInv*glm::translate(vec3(xTrans,-yScale,0.05))*
						getPlacementMat(p, vec2(scale.x,-yScale));
			stencil->uploadMV(MV);
			stencil->draw();
		}
		//Always draw the stencil
		MV = pInv*glm::translate(vec3(xTrans,0,0))*getPlacementMat(p,scale);
      stencil->uploadMV(MV);
      stencil->draw(stencilTexture);
	}
}

void Ind_Chunks::update(Player * playerPtr){
	setValue(max(playerPtr->getHealth()/150.f, 0));
	//cout << value/2 << endl;
}

Ind_Bar::Ind_Bar()
: Indicator(){
}

Ind_Bar::Ind_Bar(vec2 dim, vec4 color, string sT, Drawable * stPtr, placement p)
: Indicator(dim, color, sT, stPtr, p){
}

void Indicator::setValue(float value){
	this->value = value;
}

void Ind_Bar::update(Player * playerPtr){
	setValue(playerPtr->getCharge());
}

void Ind_Bar::draw(mat4 pInv){
	vec2 scale(dim.x, dim.y);
	mat4 MV = pInv*glm::translate(vec3(-(1-value)*dim.x,0,.05))*getPlacementMat(p,vec2(scale.x*value,scale.y));
	stencil->uploadMV(MV);
	stencil->uploadColor(color);
	stencil->draw();
	MV = pInv*glm::translate(vec3(0,0,0))*getPlacementMat(p,scale);;
	stencil->uploadMV(MV);
	stencil->draw(stencilTexture);
}

Hud::Hud()
: slots(0){
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
