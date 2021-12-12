#pragma once

#ifdef _MSC_VER
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#endif