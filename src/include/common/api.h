#pragma once

// Helpers
#if defined _WIN32 || defined __CYGWIN__
#define RYU_HELPER_DLL_IMPORT __declspec(dllimport)
#define RYU_HELPER_DLL_EXPORT __declspec(dllexport)
#define RYU_HELPER_DLL_LOCAL
#define RYU_HELPER_DEPRECATED __declspec(deprecated)
#else
#define RYU_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#define RYU_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#define RYU_HELPER_DLL_LOCAL __attribute__((visibility("hidden")))
#define RYU_HELPER_DEPRECATED __attribute__((__deprecated__))
#endif

#ifdef RYU_STATIC_DEFINE
#define RYU_API
#else
#ifndef RYU_API
#ifdef RYU_EXPORTS
/* We are building this library */
#define RYU_API RYU_HELPER_DLL_EXPORT
#else
/* We are using this library */
#define RYU_API RYU_HELPER_DLL_IMPORT
#endif
#endif
#endif

#ifndef RYU_DEPRECATED
#define RYU_DEPRECATED RYU_HELPER_DEPRECATED
#endif

#ifndef RYU_DEPRECATED_EXPORT
#define RYU_DEPRECATED_EXPORT RYU_API RYU_DEPRECATED
#endif
