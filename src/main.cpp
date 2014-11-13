#include <GL/glew.h>
#include <GL/gl.h>
#include "BaseEngine.h"
#include <SDL2/SDL_image.h>

bool init(BaseEngine& engine);
void close();

SDL_Window * gWindow;
SDL_GLContext gContext;

const std::string vertexShaderSrc = "shaders/simpleVert.glsl";
const std::string fragShaderSrc = "shaders/simpleFrag.glsl";

int main(int argc, char ** argv){
	BaseEngine engine;
	//Initialize Everything
	if (!init(engine)){
	   printf( "Failed to initialize!\n" );
      return EXIT_FAILURE;
   }

	//polling boolean, key event, sub thread
	bool quit = false;
	SDL_Event e;

	//main event loop
	while (!quit){
		while (SDL_PollEvent(&e) != 0){
			if (e.type == SDL_QUIT)
				quit = true;
			engine.handleEvent(e);
		}
		engine.move();
		engine.update();
		engine.render();
		
		SDL_GL_SwapWindow(gWindow);
	}
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
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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
	glClearColor(1.f, 0.2f, 0.2f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE); //maybe put this back later...
   glEnable(GL_MULTISAMPLE_ARB);
	glAlphaFunc(GL_GREATER, 0.5);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
	return true;
}

void close(){
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}
