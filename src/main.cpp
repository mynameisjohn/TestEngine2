
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
	Menu menu(engine.getDrawablePtr("quad"),SCREEN_DIM.x, SCREEN_DIM.y, 4);

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
					menu.grabScreen(SCREEN_DIM.x, SCREEN_DIM.y);
				}//Otherwise, if we're in menu
				else if (s == STATE_MENU){
					//Depending on what the event is
					switch(menu.handleEvent(e)){
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

   //Init SDL+OpenGL Context
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

   //Create Window
   gWindow = SDL_CreateWindow("3D Test",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_DIM.x, SCREEN_DIM.y, //SCREEN_WIDTH, SCREEN_HEIGHT,
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
