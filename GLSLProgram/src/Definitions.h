#ifndef Definitions_H
#define Definitions_H

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
#define WORLD_COORD_LOCATION	0
#define COLOR_COORD_LOCATION	1
#define NORMAL_COORD_LOCATION	2
#define TEXTURE_COORD_LOCATION	3



#pragma comment (lib,"./lib/glfw3.lib")
#pragma comment (lib,"./lib/glfw3dll.lib")	
#pragma comment (lib,"./lib/glew32.lib")
#pragma comment (lib, "opengl32.lib")



#include "../include/GL/glew.h"
#define GLFW_DLL
#include "../include/GLFW/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtx/quaternion.hpp"



#endif