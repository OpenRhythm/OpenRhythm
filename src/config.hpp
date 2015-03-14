#pragma once

// Platforms
#define PL_WINDOWS 1
#define PL_LINUX 2
#define PL_OSX 3

#define PLATFORM PL_LINUX

#if PLATFORM==PL_WINDOWS
	#define PLATFORM_WINDOWS
#elif PLATFORM==PL_LINUX
	#define PLATFORM_LINUX
#elif PLATFORM==PL_OSX
	#define PLATFORM_OSX
#endif


// will add version info here later on
