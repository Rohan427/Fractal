#pragma once

#include "Constants.h"
#include "msvc_defines.h"
#include <hip/hip_runtime.h>
#include <iostream>
#include <string>
#include "Mandel.h"

#define HIP_CHECK(status)                                                                                     \
    if (status != hipSuccess) {                                                                               \
        fprintf(stdout, "error: '%s'(%d) at %s:%d\n", hipGetErrorString(status), status, __FILE__, __LINE__); \
        exit(0);                                                                                              \
    }

class MandelGPU
{
	public:

	int mandelGPU2();
    int mandelGPU (unsigned int ImageWidth, unsigned int ImageHeight, double Re_factor, double Im_factor, double MaxIm, double MinRe, double* hostBuffer);
    int testPoints (double *hostBuffer, double max, int numPixels);
};