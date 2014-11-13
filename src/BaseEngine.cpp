/**
TODO
	-- comment code properly
	-- create library/character libraries
	-- projectiles
	-- art
	-- sound
	-- skinning
**/

#include "Level.h"
#include "BaseEngine.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

BaseEngine::BaseEngine(){
	//NYI
}

BaseEngine::~BaseEngine(){
	glDeleteProgram(shader.getProgramID());
	glBindVertexArray(0);
}

bool BaseEngine::init(string vertShaderSrc, string fragShaderSrc){
	//Load Vertex/Fragment Shader files
   if (!shader.loadVert(vertShaderSrc) ||
       !shader.loadFrag(fragShaderSrc))
      return false;

   //Generate Shader Program
   if (!shader.loadProgram())
      return false;
	
//	pop = std::move(initLevel(shader, 1));

	level = Level(1, shader);

	return true;
}

void BaseEngine::update(){
//	pop->update();
	level.update();
}

//figure out a better way to do this
void BaseEngine::move(){
//	level.move();
	cam.push(level.move());//getPlayer());
}
//Try and get SDL out of the picture
void BaseEngine::handleEvent(SDL_Event& e){
	//EventRegister * eReg = pop->getPlayer()->getRegPtr();
	EventRegister * eReg = level.getPlayer()->getRegPtr();

	switch (e.type){
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (!e.key.repeat)
				eReg->handleKey(keyCode(e));
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
			const vec2 screenDim(SCREEN_WIDTH, SCREEN_HEIGHT);
			const vec2 m1(0,1), m2(1,0), m3(-1,-1), m4(1,1);
			mat4 projMat = cam.getProjMat();

			//map mouse position to screen coordinates
			vec2 sp = remap(vec2(e.motion.x,e.motion.y)/screenDim, m1, m2, m3, m4);//fix this

			//get player position in screen coordinates
			vec4 playerPos = projMat * vec4(level.getPlayer()->center(), 1);
			playerPos /= playerPos.w;

			//use player screen z to get mouse world coordinates
			vec4 worldMouse = glm::inverse(projMat) * vec4(sp, playerPos.z, 1); 
			worldMouse /= worldMouse.w;
	
			//send it to the event register
			eReg->handleMotion(vec2(worldMouse));
		}
		default:
			break;
	}
}

void BaseEngine::render(){
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.bind();

	//send projection matrix to device
	cam.updateProj(shader.getProjHandle());
	//draw everything
	//pop->draw();
	level.draw();

	shader.unbind();
}

int keyCode(SDL_Event& e){
   return (int)e.key.keysym.sym;
}
