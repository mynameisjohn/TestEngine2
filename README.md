An updated version of the Test Engine. This represents the groundwork for a larger project I hope to start soon. 
It's the beginning of a simple gameengine using C++ and OpenGL. At the moment the following is working:

- 3D Collision (hard and soft)
- Texture Mapping
- OBJ Loading
- Geometry from SVG Paths
- XML based Resource Loading (for levels, drawable hierarchies, etc.)
- Basic Skinning/Rig Algorithm
- Basic Player Based Event Handling
- Basic Non-Player Entities with potential based AI/movement

The next big step is to work on art and music.

INSTRUCTIONS (for linux/unix based systems):

1. Clone this repository. 
2. Install SDL2(with the image/mixer libraries), GLEW, and ensure you have the OpenGL libraries. Also get CMake 2.8 or greater, and copy the files in CMakeModules to the Modules directory of CMake (/usr/share/cmake-2.8/Modules for me)
3. Enter the build directory, and type
	- cmake ..; make
	or use the makeIt shell script, which was designed for my machine that has 8 cores to compile on. 
4. run engineTest from within the build directory
