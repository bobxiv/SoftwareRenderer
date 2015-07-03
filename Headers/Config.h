#pragma once

#ifndef CONFIG_H
#define CONFIG_H

//--------------------------------------------------------------------------
// Inlining macro
//--------------------------------------------------------------------------

// Uncomment or define FORCE_INLINE to use __forceinline
// #define FORCE_INLINE

#if defined(DEBUG)
#define INLINE inline
#else
	#if defined(FORCE_INLINE)
	#define INLINE __forceinline
	#else
	#define INLINE inline
	#endif
#endif

//--------------------------------------------------------------------------
// Primitives types macro
//--------------------------------------------------------------------------

#include "PrimitiveTypes.h"

//--------------------------------------------------------------------------
// Vector instructions usage
//--------------------------------------------------------------------------

//Tells if SIMD instructions should be used
#define SIMD_EXTENSION

#ifdef SIMD_EXTENSION
	//#include "sse_config.h"

	//#include "Vector4SIMD.inl"
	#include "Matrix4x4SIMD.inl"
#else
	#include "Vector4.h"
	#include "Matrix4x4.h"
	//#include "Vector_Expression.h"
	//#include "Vector_Expression_Mod.h"
#endif

#include "HelpMath.h"

#endif // end CONFIG_H