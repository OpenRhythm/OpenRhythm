#include "config.hpp"

#if defined(PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(PLATFORM_LINUX)
	#include <time.h>
#elif defined(PLATFORM_OSX)
	#include <mach/clock.h>
	#include <mach/mach.c>
#endif