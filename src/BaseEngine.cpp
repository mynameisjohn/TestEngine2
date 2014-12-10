/**

TODO
	-- comment code properly
	-- create library/character libraries
	-- projectiles
	-- art
	-- sound
	-- skinning
**/

#include <Level.h>
#include <BaseEngine.h>
#include <Textures.h>

#include <string>
#include <set>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <GL_Includes.h>


BaseEngine::BaseEngine(){
	//NYI
}

BaseEngine::~BaseEngine(){
	glDeleteProgram(shader.getProgramID());
	glBindVertexArray(0);
}

bool BaseEngine::init(string vS, string fS, vec2 screenDim){
	//Load Vertex/Fragment Shader files
   if (!shader.loadVert(vS) ||
       !shader.loadFrag(fS))
      return false;

   //Generate Shader Program
   if (!shader.loadProgram())
      return false;

	cam = Camera(screenDim);

	level = unique_ptr<Level>(new Level(1, hud, shader, &dMap));

	//just testing fonts
	dMap["quad"]->addTex("fontTest", fromTextString("hello"));


	return true;
}

void BaseEngine::update(){
//	pop->update();
	level->update();
	if (!motionHandled){
		int x,y;
		SDL_GetMouseState(&x,&y);
		handleMotion((float)x, (float)y);
	}
	//cout << level->getPlayer()->getCharge() << endl;
	hud.update(level->getPlayer());
}

//figure out a better way to do this
void BaseEngine::move(){
//	level.move();
	cam.push(level->move());//getPlayer());
}
//Try and get SDL out of the picture
void BaseEngine::handleEvent(SDL_Event& e){
	EventRegister * eReg = level->getPlayer()->getRegPtr();

	switch (e.type){
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (!e.key.repeat)
				eReg->handleKey(keyCode(e), e.type==SDL_KEYDOWN);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			switch (e.button.button){
				case SDL_BUTTON_LEFT:
					eReg->toggleLMB();
					break;
				case SDL_BUTTON_RIGHT:
					eReg->toggleRMB();
					break;
			}
			break;
		case SDL_MOUSEMOTION:{
			float x(e.motion.x), y(e.motion.y);
			handleMotion(x,y);
		}
		default:
			break;
	}
}

void BaseEngine::render(){
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   bindShader();

	//send projection matrix to device
	cam.updateProj(shader.getProjHandle());
	//draw everything
	level->draw();

	hud.draw(glm::inverse(cam.getProjMat()));

	unBindShader();
	motionHandled = false;
}

int keyCode(SDL_Event& e){
   return (int)e.key.keysym.sym;
}

void BaseEngine::handleMotion(float x, float y){
	EventRegister * eReg = level->getPlayer()->getRegPtr();
	const vec2 m1(0,1), m2(1,0), m3(-1,-1), m4(1,1);
	mat4 projMat = cam.getProjMat();

	//map mouse position to screen coordinates
	vec2 sp = remap(vec2(x,y)/cam.getScreenDim(), m1, m2, m3, m4);//fix this

	//get player position in screen coordinates
	vec4 playerPos = projMat * vec4(level->getPlayer()->center(), 1);
	playerPos /= playerPos.w;

	//use player screen z to get mouse world coordinates
	vec4 worldMouse = glm::inverse(projMat) * vec4(sp, playerPos.z, 1); 
	worldMouse /= worldMouse.w;

	//send it to the event register
	eReg->handleMotion(vec2(worldMouse));
	motionHandled = true;
}

Drawable * BaseEngine::getDrawablePtr(string name){
	return dMap[name].get();
}

mat4 BaseEngine::getProjMat(){
	return cam.getProjMat();
}

void BaseEngine::bindShader(){
	shader.bind();
}

void BaseEngine::clearKeyState(){
	level->getPlayer()->getRegPtr()->clearKeyState();
}

void BaseEngine::unBindShader(){
	shader.unbind();
}

void BaseEngine::updateProjMat(){
	cam.updateProj(shader.getProjHandle());
}

vec2 BaseEngine::getScreenDim(){
	return cam.getScreenDim();
}
