#pragma once

#include "DME/Core/PlatformDetection.h"

#include <memory>

#ifdef DME_DEBUG
	#if defined(DME_PLATFORM_WINDOWS)
		#define DME_DEBUGBREAK() __debugbreak()
	#elif defined(DME_PLATFORM_LINUX)
		#include <signal.h>
		#define DME_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn`t support debugbreak yet!"
	#endif
	#define DME_ENABLE_ASSERTS
#else
	#define DME_DEBUGBREAK() 
#endif

#define DME_EXPAND_MACRO(x) x
#define DME_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define DME_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace DME
{
	template <typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}


#include "DME/Core/Log.h"
#include "DME/Core/Assert.h"