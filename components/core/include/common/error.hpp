#pragma once

#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#	define __GCC__ __GNUC__
#endif

#if defined(__clang__)
// CLANG ENABLE/DISABLE WARNING DEFINITION
#	define REMUS_DISABLE_WARNINGS()                            \
		_Pragma("clang diagnostic push")                        \
		    _Pragma("clang diagnostic ignored \"-Wall\"")       \
		        _Pragma("clang diagnostic ignored \"-Wextra\"") \
		            _Pragma("clang diagnostic ignored \"-Wtautological-compare\"")

#	define REMUS_ENABLE_WARNINGS() \
		_Pragma("clang diagnostic pop")
#elif defined(__GNUC__) || defined(__GNUG__)
// GCC ENABLE/DISABLE WARNING DEFINITION
#	define REMUS_DISABLE_WARNINGS()                            \
		_Pragma("GCC diagnostic push")                          \
		    _Pragma("GCC diagnostic ignored \"-Wall\"")         \
		        _Pragma("clang diagnostic ignored \"-Wextra\"") \
		            _Pragma("clang diagnostic ignored \"-Wtautological-compare\"")

#	define REMUS_ENABLE_WARNINGS() \
		_Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
// MSVC ENABLE/DISABLE WARNING DEFINITION
#	define REMUS_DISABLE_WARNINGS() \
		__pragma(warning(push, 0))

#	define REMUS_ENABLE_WARNINGS() \
		__pragma(warning(pop))
#endif
