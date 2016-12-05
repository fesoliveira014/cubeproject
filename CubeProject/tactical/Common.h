#ifndef _COMMON_H_
#define _COMMON_H_

#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <map>
#include <sstream>
#include <tuple>
#include <stdexcept>
#include <fstream>
#include <ctime>
#include <unordered_map>
#include <cmath>
#include <random>
#include <thread>
#include <mutex>


#define GLEW_STATIC
#include "glew\glew.h"

#include "SFML\Window.hpp"
#include "SFML\System.hpp"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\euler_angles.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtx\hash.hpp"
#include "glm\gtc\noise.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\string_cast.hpp"
#include "glm\gtx\common.hpp" 

#include "Types.h"

#define RADIANS(x) x*glm::pi<float>()/180.0f
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

#define TACTICAL_MS_PER_UPDATE 10.0
#define NUM_THREADS 8

extern std::mutex globalMutex;

#endif