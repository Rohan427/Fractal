#pragma once

#include "Constants.h"
#include "msvc_defines.h"
#include <hip/hip_runtime.h>
#include <iostream>
#include <string>

#define HIP_CHECK(status)                                                                                     \
    if (status != hipSuccess) {                                                                               \
        fprintf(stderr, "error: '%s'(%d) at %s:%d\n", hipGetErrorString(status), status, __FILE__, __LINE__); \
        exit(0);                                                                                              \
    }

class MandelGPU
{
	public:

	int mandelGPU();
};