#ifndef GL_INCLUDES
#define GL_INCLUDES

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#ifndef GLEW_STATIC
		#define GLEW_STATIC
	#endif
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#endif
