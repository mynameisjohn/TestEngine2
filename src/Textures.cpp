#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Textures.h"
#include "Util.h"

vector<unsigned int> fromFileList(vector<string> files){
	vector<unsigned int> ret;
	for (uint32_t i=0;i<files.size();i++)
		ret.push_back(fromImage(files[i]));
	return ret;
}

unsigned int fromImage(string fileName){
	GLuint tex(0);
	SDL_Surface * s = IMG_Load(fileName.c_str());
	if (!s){
		cout << "Failed to load texture " << fileName << endl;
		return tex;//This is bad
	}

	//Generate the device texture and bind it
   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

	//figure out mipmapping with filtering asap

   //Upload host texture to device
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

   // Set filtering
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Unbind, Delete the host texture, return handle to device texture
	glBindTexture(GL_TEXTURE_2D, 0);
   SDL_FreeSurface(s);
	
   return (unsigned int)tex;
}

int * outline(int DIM, int th){
	int * PXA = new int[DIM*DIM];
	int white = 0xFFFFFFFF;
	int black = 0xFF000000;

   for (int y=0; y<DIM; y++){
      for (int x=0; x<DIM; x++){
         if (x<th || x>DIM-th || y<th || y>DIM-th)
            PXA[y*DIM+x] = black;
         else
            PXA[y*DIM+x] = white;
      }
   }
	
	return PXA;
}

int * invertedOutline(int DIM, int th){
	int * PXA = new int[DIM*DIM];
	int white = 0xFFFFFFFF;
	int black = 0xFF000000;

   for (int y=0; y<DIM; y++){
      for (int x=0; x<DIM; x++){
         if (x<th || x>DIM-th || y<th || y>DIM-th)
            PXA[y*DIM+x] = white;
         else
            PXA[y*DIM+x] = black;
      }
   }
	
	return PXA;
}

unsigned int outlineTexture(){
	GLuint tex; //Device texture handle
   int DIM=100; //Texture dimension
	int th=8; //Outline thickness
   int * PXA; //Host texture storage(MUST BE FREED)

   //Create the outline texture on host
	PXA = outline(DIM, th);

   //Generate the device texture and bind it
   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

   //Upload host texture to device
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DIM, DIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, PXA);

   // Set filtering
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Delete the host texture, return handle to device texture
   delete PXA;
   return (unsigned int)tex;
}

unsigned int invertedOutlineTexture(){
	GLuint tex; //Device texture handle
   int DIM=100; //Texture dimension
	int th=8; //Outline thickness
   int * PXA; //Host texture storage(MUST BE FREED)

   //Create the outline texture on host
	PXA = invertedOutline(DIM, th);

   //Generate the device texture and bind it
   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

   //Upload host texture to device
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DIM, DIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, PXA);

   // Set filtering
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Delete the host texture, return handle to device texture
   delete PXA;
   return (unsigned int)tex;
}
