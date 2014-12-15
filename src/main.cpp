
#include <GL_Includes.h>

#include <SDL_image.h>
#include <Menu.h>
#include <Textures.h>
#include <BaseEngine.h>

#include <glm/gtx/transform.hpp>

bool init(BaseEngine& engine);
void close();

SDL_Window * gWindow;
SDL_GLContext gContext;

//Apple demands OpenGL 3.3, and needs more modern shaders
#ifdef __APPLE__
const std::string vertexShaderSrc = "shaders/mac_vertShader.glsl";
const std::string fragShaderSrc = "shaders/mac_fragShader.glsl";
const int glMajor(3), glMinor(3);
#else
const std::string vertexShaderSrc = "shaders/vertShader.glsl";
const std::string fragShaderSrc = "shaders/fragShader.glsl";
const int glMajor(3), glMinor(0);
#endif

int main(int argc, char ** argv){

	GameState currentState;
	BaseEngine engine;
	SDL_Event * evt(nullptr);

	if (!init(engine)){
		close();
		return EXIT_FAILURE;
	}

	evt = new SDL_Event();
	HandleEvent:
	while (SDL_PollEvent(evt)){
		if (evt->type == SDL_QUIT)
			goto Quit;
		GameState evtState = engine.handleEvent(evt);
		if (evtState == QUIT_GAME)
			goto Quit;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentState = engine.iterate();
	SDL_GL_SwapWindow(gWindow);

	if (currentState != QUIT_GAME)
		goto HandleEvent;


	Quit:
	if (evt) delete evt;
   close();
	cout << "goodbye" << endl;
   return EXIT_SUCCESS;
	/*

	const int GAME_RESUME(1), GAME_QUIT(2);

	SDL_Event * evt(new SDL_Event());//malloc(sizeof(SDL_Event))
	//GameState GS(GAME_RESUME);
	int GS(GAME_RESUME);

	while (GS != GAME_QUIT){
		//Clear OpenGL Buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//If there's an event, handle it
		if (SDL_PollEvent(evt)){
			if (evt->type == SDL_QUIT)
				break;
//			engine.handleEvent(evt);
		}
		//Iterate Engine
		GS = engine.iterate();
		//Swap window Buffers
		SDL_GL_SwapWindow(gWindow);
	}		

	
	if (evt)
		delete evt;	
	close();

	return EXIT_SUCCESS;
*/
/*
			
		}

		close();

		return EXIT_SUCCESS;



		bool inPlay(GAME_RESUME);





		handleevent:
		while (inPlay != GAME_QUIT)
			while (SDL_PollEvent(&e){
				if (e.type == SDL_QUIT)
					goto iterate;
			}
			inplay = engine.iterate(e);
			if (inPlay == GAME_QUIT)
				goto draw;
			while 
			if (engine.iterate() == GAME_QUIT)
				break;
		}

		

		//Clear OpenGL Buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Depending on state
		switch(s){
			//Let the engine do its thing
			case STATE_GAME:
				engine.move();
				engine.update();
				engine.render();
				break;
			case STATE_MENU:
				//Let the menu do its thing (MOVE SHADER INTO MENU ASAP)
				menu.update();
				engine.bindShader();
				menu.render(glm::inverse(engine.getProjMat()));
				engine.unBindShader();
			case STATE_QUIT:
			default:
				break;
		}

		//Swap window buffers
		SDL_GL_SwapWindow(gWindow);
	}
	//After loop, close up shop
	close();

	return EXIT_SUCCESS;*/
}

bool init(BaseEngine& engine){
   //Init SDL Video
   if (SDL_Init( SDL_INIT_VIDEO ) < 0){
      printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
      return false;
   }

	//Init SDL image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)){
		printf("SDL_Image could not initialize! SDL_Image Error: %s\n", IMG_GetError());
      return false;
	}

   //Init SDL+OpenGL Context
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Make local
   //Create Window
	vec2 screenDim(engine.getScreenDim());
   gWindow = SDL_CreateWindow("3D Test",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              screenDim.x, screenDim.y,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
   if (gWindow == NULL){
      printf( "Window could not be created! SDL Error: %s\n", SDL_GetError());
      return false;
   }

   //Create Context from Window
   gContext = SDL_GL_CreateContext(gWindow);
   if( gContext == NULL ){
      printf("OpenGL context was not created! SDL Error: %s\n", SDL_GetError());
      return false;
   }

   //Initialize GLEW
   glewExperimental = GL_TRUE;
   GLenum glewError = glewInit();
   if( glewError != GLEW_OK ){
      printf( "Error initializing GLEW! %s\n", glewGetErrorString(glewError));
      return false;
   }

	//Use Vsync
   if( SDL_GL_SetSwapInterval(1) < 0 ){
      printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
   }

	//Initialize Shaders and Engine
   if(!engine.init(vertexShaderSrc, fragShaderSrc)){
      printf( "Unable to initialize OpenGL!\n" );
      return false;
   }

	//OpenGL settings
	glClearColor(1,.4,.4,1);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE); //maybe put this back later...
   glEnable(GL_MULTISAMPLE_ARB);

	//These are legacy calls valid in OpenGL 3.0 only
	#ifndef __APPLE__
	glAlphaFunc(GL_GREATER, 0.9);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	#endif

	//For debugging
	glLineWidth(8.f);

	return true;
}

void close(){
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}
