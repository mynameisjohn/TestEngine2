/**

TODO
	-- Have hud drawn to a separate buffer, just so menu bg doesn't have it
	-- comment code properly
	-- art
	-- sound
**/

#include <BaseEngine.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <GL_Includes.h>
#include <Textures.h>

//Menu downsample
const int DS(4);

BaseEngine::BaseEngine()
//	:	m_Status(QUIT_GAME)
{}

BaseEngine::~BaseEngine(){
	glDeleteProgram(shader.getProgramID());
	glBindVertexArray(0);
}

GameState BaseEngine::iterate(){
	mat4 I(1);
	shader.bind();
	switch(m_Status){
		case RESUME_GAME:
			move();
			update();
			draw();
			break;
		case RESUME_MENU:
			glUniformMatrix4fv(shader.getProjHandle(), 1, GL_FALSE, glm::value_ptr(I));
			menu.update();
			cout << volume << endl;
			menu.draw();
			break;
		case QUIT_GAME:
		default:
			m_Status = QUIT_GAME;
	}
	shader.unbind();

	return m_Status;
}


bool BaseEngine::init(string vertShaderSrc, string fragShaderSrc){
	//Load Vertex/Fragment Shader files
   if (!shader.loadVert(vertShaderSrc) ||
       !shader.loadFrag(fragShaderSrc))
      return false;

   //Generate Shader Program
   if (!shader.loadProgram())
      return false;

	//Load first level
	level = unique_ptr<Level>(new Level(1, hud, shader, &dMap));

	//Create the menu frame
	vec2 menuDim(1.5);
	menu = Menu(BoundRect(-0.5f*menuDim, menuDim),0.15f, dMap["quad"].get()); 

	float aspect = DEFAULT_SCREEN_DIM.x / DEFAULT_SCREEN_DIM.y;
	mat4 proj = 
		glm::perspective((float)M_PI/4.f, aspect, 100.f, 10000.f) *
		glm::rotate((float)M_PI/15.f, vec3(1, 0, 0));
	
// * 		glm::translate(vec3(0, DEFAULT_SCREEN_DIM.x*1.667F,0));
	cam = Camera(DEFAULT_SCREEN_DIM, proj, vec3());//level->getPlayer()->getPos());

	vec2 screenDim(cam.getScreenDim());
	//Generate blank texture, add it to drawable
   *(menu.getTex()) = initTexture(NULL, screenDim.x/DS, screenDim.y/DS);
   dMap["quad"].get()->addTex("screen", *(menu.getTex()));

   //Generate Frame buffer and attach texture
   glGenFramebuffers(1, menu.getFBO());
   glBindFramebuffer(GL_FRAMEBUFFER, *(menu.getFBO()));
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *(menu.getTex()), 0);   

   //Rebind back buffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (!isFBOComplete())
		return false;

	//Add some panes to the menu (will be in XML soon enough
	if ((menu.addPane("General") &&
		  menu.addPane("Video") &&
		  menu.addPane("Audio") &&
		  menu.addPane("Controls")) == false)
		return false;

	//What a pile
	const float minVol(0), maxVol(100), defVol(75);
	volume = defVol;
	Slider sl("slider",BoundRect(vec2(0.3),vec2(0.3,0.15)),dMap["quad"].get(),defVol,minVol,maxVol,&volume);
	Switch sw("switch",BoundRect(vec2(-0.3),vec2(0.3,0.15)),dMap["quad"].get(),
		{{"zero", 0},{"one", 1}}, nullptr, "one");
	
	menu["General"]->addControl(sl);
	menu["General"]->addControl(sw);

	m_Status = RESUME_GAME;

	return isFBOComplete();
}

bool BaseEngine::grabScreen(){
   //Bind Read and Draw buffers for Blit
   glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *(menu.getFBO()));
   //Blit with downsampling for fun
	vec2 screenDim(cam.getScreenDim());
   glBlitFramebuffer(0, 0, screenDim.x, screenDim.y,
                     0, screenDim.y/DS, screenDim.x/DS, 0
                     ,GL_COLOR_BUFFER_BIT, GL_NEAREST);
   //Return back buffer into the default slot
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   return isFBOComplete();
}

vec2 BaseEngine::getScreenDim(){
	return cam.getScreenDim();
}

void BaseEngine::update(){
	level->update();
	if (!motionHandled){
		int x,y;
		SDL_GetMouseState(&x,&y);
		handleMotion((float)x, (float)y);
	}
	hud.update(level->getPlayer());
}

//figure out a better way to do this
void BaseEngine::move(){
	cam.push(level->move());
}

GameState BaseEngine::handleEvent(SDL_Event * e){
	EventRegister * eReg = level->getPlayer()->getRegPtr();

	switch (e->type){
		case SDL_KEYUP:
			if (e->key.keysym.sym == SDLK_ESCAPE && m_Status != RESUME_MENU){
				grabScreen();
				m_Status = RESUME_MENU;
				return m_Status;
			}
		case SDL_KEYDOWN:
			if (e->key.repeat == false)
				eReg->handleKey((int)(e->key.keysym.sym), e->type==SDL_KEYDOWN);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (e->button.button == SDL_BUTTON_LEFT)
				eReg->toggleLMB();
			if (e->button.button == SDL_BUTTON_RIGHT)
				eReg->toggleRMB();
			break;
		case SDL_MOUSEMOTION:{
			float x(e->motion.x), y(e->motion.y);
			handleMotion(x,y);
		}
		default:
			break;
	}

	if (m_Status == RESUME_MENU){
		//This may be done twice...but it's not really that expensive
		const vec2 m1(0,1), m2(1,0), m3(-1,-1), m4(1,1);
		int x, y;
		bool lmbDown = (bool)((SDL_GetMouseState(&x, &y)) & SDL_BUTTON(SDL_BUTTON_LEFT));
		vec2 sp = remap(vec2(x,y)/getScreenDim(), m1, m2, m3, m4);
		m_Status = menu.handleEvent(sp, lmbDown).gs;
	}

	return m_Status;
}

void BaseEngine::draw(){
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//   bindShader();

	//send projection matrix to device
	cam.updateProj(shader.getProjHandle());
	//draw everything
	level->draw();

	hud.draw(glm::inverse(cam.getProjMat()));

//	unBindShader();
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
	vec2 sp = remap(vec2(x,y)/getScreenDim(), m1, m2, m3, m4);//fix this

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
