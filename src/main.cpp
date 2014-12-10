#include <main.h>

#include <GL_Includes.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glm/gtx/transform.hpp>

#include <tinyxml.h>

int main(int argc, char ** argv){
	BaseEngine engine;
	//Initialize Everything
	if (!init(engine)){
	   printf( "Failed to initialize!\n" );
      return EXIT_FAILURE;
   }

	//different loop states
	enum state{
		STATE_QUIT,
		STATE_GAME,
		STATE_MENU
	};

	//Game state, main event struct, Menu object
	state s(STATE_GAME);
	SDL_Event e;
	Menu menu(engine.getDrawablePtr("quad"),engine.getScreenDim(), 4);

	//While the state hasn't been set to quit
	while (s != STATE_QUIT){
		//Check for events
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT)
				s = STATE_QUIT;
			else{ //If Escape is pressed for the first time, and we aren't in Menu
				if (e.key.keysym.sym == SDLK_ESCAPE && e.type == SDL_KEYDOWN && 
					 e.key.repeat == 0 && s != STATE_MENU){
					//Set state to Menu, grab currect screen for background
					s = STATE_MENU;
					menu.grabScreen(engine.getScreenDim());
				}//Otherwise, if we're in menu
				else if (s == STATE_MENU){
					//Depending on what the event is
					switch(menu.handleEvent(e,engine.getScreenDim())){
						case MENU_CONTINUE: //Stay in Menu
							break;
						case MENU_RESUME: //Resume game
							s = STATE_GAME;
							break;
						case MENU_QUIT: //Quit
						default:
							s = STATE_QUIT;
							break;
					}
				}
				//Engine just stays aware of the keyboard (MOVE OUT EVENT_REGISTER)
				engine.handleEvent(e);//I don't like this because motion is expensive
			}
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

	return EXIT_SUCCESS;
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

	if (TTF_Init() < 0){
		cout << "SDL_TTF could not initialize! Error: " << TTF_GetError() << endl;
		return false;
	}
	
   //Init SDL+OpenGL Context
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

   //Create Window
   gWindow = SDL_CreateWindow("Engine Test",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_SCREEN_SIZE.x, DEFAULT_SCREEN_SIZE.y,
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
   if(!engine.init(vertexShaderSrc, fragShaderSrc, DEFAULT_SCREEN_SIZE)){
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
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}
