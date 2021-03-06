// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <random>
#include <mutex>
#include <array>
#include <thread>


#define GLM_FORCE_RADIANS
#ifdef _WIN64
#   define GLM_FORCE_SSE4
#endif
#include <glm/glm.hpp>
#include <glm/gtx/simd_vec4.hpp>
#include <glm/gtx/simd_mat4.hpp>


#include <Windows.h>
#include <ppl.h>

#include <d3d11.h>
#include <wrl/client.h>


#ifdef max
#undef max
#undef min
#endif // max

template<class IntType>
inline IntType ceildiv(IntType a, IntType b)
{
	return a / b + (a%b == 0 ? 0 : 1);
}

//#undef OutputLog
#define OutputLog
#ifdef OutputLog
	#include <fstream>
	#define Log(msg) \
				{std::ofstream log_file("log.txt", std::ios_base::out | std::ios_base::app); \
				log_file << msg; \
				log_file << std::endl;}
#else
	#define Log(msg) 
#endif