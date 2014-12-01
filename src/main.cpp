
#include <GL_Includes.h>

#include <SDL_image.h>

#include <Textures.h>
#include <BaseEngine.h>

#include <glm/gtx/transform.hpp>

bool init(BaseEngine& engine);
void close();

SDL_Window * gWindow;
SDL_GLContext gContext;

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

	//polling boolean, key event, sub thread
	bool quit = false;
	SDL_Event e;

	//main event loop
	while (!quit){
		while (SDL_PollEvent(&e) != 0){
			if (e.type == SDL_QUIT)
				quit = true;
			else if (e.key.keysym.sym == SDLK_ESCAPE && e.type == SDL_KEYDOWN){
				GLuint tex(0), fbo(0), downSample(4);
				tex = initTexture(NULL, SCREEN_DIM.x/downSample, SCREEN_DIM.y/downSample);				

				glGenFramebuffers(1, &fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

				glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
				glBlitFramebuffer(0, 0, SCREEN_DIM.x, SCREEN_DIM.y, 0, SCREEN_DIM.y / downSample, SCREEN_DIM.x / downSample, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);


				//Put this somewhere else
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status == GL_FRAMEBUFFER_COMPLETE){
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					bool Quit(false);
					SDL_Event E;
					Drawable * quadPtr = engine.getDrawablePtr("quad");
					quadPtr->addTex("screen", tex);
					vec4 color(1);
					mat4 mv(glm::inverse(engine.getProjMat())*glm::translate(vec3(-1, -1, 0))*glm::scale(vec3(2, 2, 1)));
			

					while (!Quit){
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						while (SDL_PollEvent(&E) != 0){
							if (E.key.keysym.sym == SDLK_ESCAPE && E.type == SDL_KEYDOWN)
								Quit = true;
						}
						engine.bindShader();
						quadPtr->uploadMV(mv);
						quadPtr->uploadColor(color);
						quadPtr->draw("screen");
						engine.unBindShader();
						SDL_GL_SwapWindow(gWindow);
					}
				}
				else
					cout << "Bad" << endl;
				glDeleteTextures(1, &tex);
				glDeleteFramebuffers(1, &fbo);
				

			}
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
