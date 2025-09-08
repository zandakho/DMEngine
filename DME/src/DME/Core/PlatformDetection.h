#pragma once

// Basic platform detection for DME
#if defined(_WIN32) || defined(_WIN64)
    #define DME_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define DME_PLATFORM_IOS
    #else
        #define DME_PLATFORM_MACOS
    #endif
#elif defined(__linux__)
    #define DME_PLATFORM_LINUX
#elif defined(__ANDROID__)
    #define DME_PLATFORM_ANDROID
#else
    #warning "Unknown platform: building with generic defaults"
#endif

// Build config
#if !defined(NDEBUG)
    #define DME_DEBUG
#else
    #define DME_RELEASE
#endif

// Helpers for exports
#if defined(DME_PLATFORM_WINDOWS)
    #ifdef DME_DYNAMIC_LINK
        #ifdef DME_BUILD_DLL
            #define DME_API __declspec(dllexport)
        #else
            #define DME_API __declspec(dllimport)
        #endif
    #else
        #define DME_API
    #endif
#else
    #define DME_API
#endif