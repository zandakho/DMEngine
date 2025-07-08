#pragma once

#include "DME/Core/Base.h"
#include "DME/Core/Log.h"
#include <filesystem>

#ifdef DME_ENABLE_ASSERTS

	#define DME_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { DME##type##ERROR(msg, __VA_ARGS__); DME_DEBUGBREAK(); } }
	#define DME_INTERNAL_ASSERT_WITH_MSG(type, check, ...) DME_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define DME_INTERNAL_ASSERT_NO_MSG(type, check) DME_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", DME_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
	
	#define DME_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define DME_INTERNAL_ASSERT_GET_MACRO(...) DME_EXPAND_MACRO( DME_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, DME_INTERNAL_ASSERT_WITH_MSG, DME_INTERNAL_ASSERT_NO_MSG) )
	
	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define DME_ASSERT(...) DME_EXPAND_MACRO( DME_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define DME_CORE_ASSERT(...) DME_EXPAND_MACRO( DME_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define DME_ASSERT(...)
	#define DME_CORE_ASSERT(...)
#endif