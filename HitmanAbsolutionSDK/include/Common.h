#pragma once

#pragma warning(disable: 4251)

#ifdef EXPORTS
#define HitmanAbsolutionSDK_API __declspec(dllexport)
#else
#define HitmanAbsolutionSDK_API __declspec(dllimport)
#endif

#ifndef CONCAT_IMPL
#define CONCAT_IMPL(x, y) x##y
#endif

#ifndef MACRO_CONCAT
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#endif

#ifndef PAD
#define PAD(SIZE) unsigned char MACRO_CONCAT(_pad, __COUNTER__)[SIZE];
#endif
