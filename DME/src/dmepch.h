#pragma once

#include "DME/Core/Base.h"
#include "DME/Core/Log.h"
#include "DME/Core/PlatformDetection.h"

#include "DME/Debug/Instrumentator.h"

#ifdef DME_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <array>												
#include <string>												
#include <memory>												
#include <vector>												
#include <sstream>												
#include <utility>												
#include <iostream>												
#include <algorithm>											
#include <functional>											
#include <unordered_map>										
#include <unordered_set>										

#ifdef DME_PLATFORM_WINDOWS										
	#include <Windows.h>										
#endif								
