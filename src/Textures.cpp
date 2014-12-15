#include <SDL_image.h>
#include <SDL_ttf.h>
#include <GL_Includes.h>

#include <Textures.h>
#include <Util.h>

uint32_t initTexture(void * PXA, int w, int h){
	uint32_t tex;

	//Generate the device texture and bind it
   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

   //Upload host texture to device
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, PXA);

	//Does this really help?
	glGenerateMipmap(GL_TEXTURE_2D);

   // Set filtering   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//This necessary?
	float aniso=0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

   //Unbind, Delete the host texture, return handle to device texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;	
}

vector<uint32_t> fromFileList(vector<string> files){
	vector<uint32_t> ret;
	for (uint32_t i=0;i<files.size();i++)
		ret.push_back(fromImage(files[i]));
	return ret;
}

uint32_t fromSDLSurface(SDL_Surface * s){
	if (!s){
		cout << "Invalid SDL Surface" << endl;
		return 0;//This is bad
	}
	GLuint tex = initTexture(s->pixels, s->w, s->h);

	//Unbind, Delete the host texture, return handle to device texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return (uint32_t)tex;
}

uint32_t fromImage(string fileName){
	GLuint tex(0);
	SDL_Surface * s = IMG_Load(fileName.c_str());
	tex = fromSDLSurface(s);
	if (!tex){
		cout << "Failed to load texture " << fileName << endl;
		return 0;//This is bad
	}

	SDL_FreeSurface(s);
	
   return (uint32_t)tex;
}

uint32_t outlineTexture(bool invert){
	const uint32_t white(0xFFFFFFFF), black(0xFF000000), DIM(100), th(8);
	vector<uint32_t> PXA(DIM*DIM);

	for (uint32_t y=0; y<DIM; y++){
      for (uint32_t x=0; x<DIM; x++){
         if (x<th || x>DIM-th || y<th || y>DIM-th)
            PXA[y*DIM+x] = (invert ? white : black);
         else
            PXA[y*DIM+x] = (invert ? black : white);
      }
   }
	
	return initTexture(PXA.data(), DIM, DIM);
}

uint32_t fromTextString(string text, uint32_t fontSize, vec3 textColor, vec3 bgColor, vec2 border){
	uint32_t tex(0);
	uint32_t backgroundColor(getRGBAInt(bgColor)), foregroundColor(getRGBAInt(textColor));
	SDL_Color textColor_str;
	textColor_str.r = 0xFF & (foregroundColor >> 24);
	textColor_str.g = 0xFF & (foregroundColor >> 16);
	textColor_str.b = 0xFF & (foregroundColor >> 8);
	textColor_str.a = 0xFF;

	uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

	const string fontName("FreeMonoBold.ttf");
	TTF_Font * font = TTF_OpenFont((FONT_DIR+fontName).c_str(), fontSize);

	SDL_Surface * s = TTF_RenderText_Blended(font,text.c_str(), textColor_str);
	int bX = (int)(border.x*s->w);
	int bY = (int)(border.y*s->h);
	SDL_Surface * bg = SDL_CreateRGBSurface(0, s->w+2*bX, s->h+2*bY,32, rmask, gmask, bmask, amask);

	for (int i=0; i<bg->w * bg->h; i++)
		((uint32_t *)bg->pixels)[i] = backgroundColor;

	SDL_Rect blitRect = {bX, bY, bg->w-bX, bg->h-bY};
	if (SDL_BlitSurface(s,NULL,bg,&blitRect) < 0){
		cout << "unable to create font surface " << SDL_GetError() << endl;
		return 0;
	}

	tex = fromSDLSurface(bg);

	SDL_FreeSurface(s);
	SDL_FreeSurface(bg);
	TTF_CloseFont(font);

	return tex;
}
