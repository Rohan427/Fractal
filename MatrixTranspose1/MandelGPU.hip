/*
Copyright (c) 2015-present Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "MandelGPU.h"

// Device (Kernel) function, it must be void
__global__ void matrixTranspose (float* out, float* in, const int width)
{
  int x = blockDim.x * blockIdx.x + threadIdx.x;
  int y = blockDim.y * blockIdx.y + threadIdx.y;
  out[y * width + x] = in[x * width + y];
}

// CPU implementation of matrix transpose
void matrixTransposeCPU (float* output, float* input, const unsigned int width)
{
  for (unsigned int j = 0; j < width; ++j) 
  {
    for (unsigned int i = 0; i < width; ++i)
    {
      output[i * width + j] = input[j * width + i];
    }
  }
}

__global__ void calcPixels (unsigned int ImageWidth, unsigned int ImageHeight, double Re_factor, double Im_factor, double MaxIm, double MinRe, double* data)
{
    int y = blockDim.y * blockIdx.y + threadIdx.y;
    int x = blockDim.x * blockIdx.x + threadIdx.x;

    //int yIndex = 0;
    //int xIndex = 0;


    //if (y < ImageHeight)
    {
        //yIndex = y * ImageWidth * 5;
        //data[y * ImageWidth * 5] = y;
        
        //if (x < ImageWidth)
        {
           // xIndex = (y * ImageWidth * 5) + (x * 5);

            // Set x, , y, and n
            data[(y * ImageWidth * 5) + (x * 5)] = x;
            data[(y * ImageWidth * 5) + (x * 5) + 1] = y;
            data[(y * ImageWidth * 5) + (x * 5) + 2] = 0;

            // Calc c_im
            data[(y * ImageWidth * 5) + (x * 5) + 3] = MaxIm - y * Im_factor;

            // Calc c_re
            data[(y * ImageWidth * 5) + (x * 5) + 4] = MinRe + x * Re_factor;
        }
    }
}

__global__ void testPoint (double max, double* hostBuffer, int numPixels)
{
	int pixel = blockDim.x * blockIdx.x + threadIdx.x;
    Mandel::gpuPixel* data = (Mandel::gpuPixel *)&hostBuffer[pixel * 5];
	int count = 0;
	bool extra = false;
	double Z_re2;
	double Z_im2;
	double zReold = 0;
	double zImold = 0;
	double Z_re = data->c_re;
    double Z_im = data->c_im;
	double n4;

	// TODO: Refactor to remove breaks?
	for (; count < max; ++count)
	{
		Z_re2 = Z_re * Z_re;
		Z_im2 = Z_im * Z_im;

		if (Z_re2 + Z_im2 > 4)
		{ // Here if not in the set
            extra = true;
			break;
		}

		Z_im = 2 * Z_re * Z_im + data->c_im;
		Z_re = Z_re2 - Z_im2 + data->c_re;

        // Check for periodicity
		if (Z_im == zImold && Z_re == zReold)
		{
			data->n = n4 = max;  /* Set to max for the color plotting */
			break;				 /* We are inside the Mandelbrot set, leave the while loop */
		}
	}
    
	if (data->n != max) // periodicity check
	{
		data->n = n4 = count;

        /*
		if (extra)
		{
			for (count = 0; count < 4; ++count)
			{
				Z_re2 = Z_re * Z_re;
				Z_im2 = Z_im * Z_im;

				Z_im = 2 * Z_re * Z_im + data->c_im;
				Z_re = Z_re2 - Z_im2 + data->c_re;

				n4++;
			}
		}
        */
	}// periodicity check
}

int MandelGPU::testPoints (double *hostBuffer, double max, int numPixels)
{
    std::cout << "Begin computing points" << std::endl;

    dim3 dimGrid (2048, 1, 1); // 2048 blocks
    dim3 dimBlock (1024, 1, 1); // 1024 threads per block

    testPoint<<<dimGrid, dimBlock, 0, 0>>> (max, hostBuffer, numPixels);

    HIP_CHECK (hipGetLastError());
    HIP_CHECK (hipDeviceSynchronize());

    std::cout << "Points computed" << std::endl;

    return 0;
}

int MandelGPU::mandelGPU (unsigned int ImageWidth, unsigned int ImageHeight, double Re_factor, double Im_factor, double MaxIm, double MinRe, double* hostBuffer)
{
    std::cout << "Initialize GPU buffer..." << std::endl;
    
    dim3 dimGrid (64, 32, 1); // 2048 blocks
    dim3 dimBlock (32, 32, 1); // 1024 threads per block

    std::cout << "Parameters" << std::endl;
    printf ("hostBuffer: %p\n", (void *)hostBuffer);
    std::cout << "Re_factor: " << Re_factor << std::endl;
    std::cout << "Im_factor: " << Im_factor << std::endl;
    std::cout << "MaxIm: " << MaxIm << std::endl;
    std::cout << "MinRe: " << MinRe << std::endl;

    calcPixels<<<dimGrid, dimBlock, 0, 0>>> (ImageWidth, ImageHeight, Re_factor, Im_factor, MaxIm, MinRe, hostBuffer);

    HIP_CHECK (hipGetLastError());

    HIP_CHECK (hipDeviceSynchronize());

    std::cout << "GPU buffer init complete" << std::endl;

    return 0;
}

int MandelGPU::mandelGPU2()
{
  hipDeviceProp_t devProp;
  HIP_CHECK (hipGetDeviceProperties(&devProp, 0));
  std::cout << "Device name " << std::string (devProp.name) << std::endl;
  const uint32_t WIDTH = 64;
  const uint32_t NUM = WIDTH * WIDTH; // 4096
  float* Matrix = (float*)malloc (NUM * sizeof (float));
  float* TransposeMatrix = (float*)malloc (NUM * sizeof (float));
  float* cpuTransposeMatrix = (float*)malloc (NUM * sizeof (float));

  // Initialize the input data
  for (uint32_t i = 0; i < NUM; ++i) 
  {
    Matrix[i] = (float)i * 10.987654f;
  }

  // MatrixTranspose computation on CPU
  matrixTransposeCPU (cpuTransposeMatrix, Matrix, WIDTH);

  std::cout << "CPU transform complete" << std::endl;

  float* gpuMatrix = 0;
  float* gpuTransposeMatrix = 0;

  // Allocate the memory on device side
  HIP_CHECK (hipMalloc ((void**)&gpuMatrix, NUM * sizeof (float)));
  HIP_CHECK (hipMalloc ((void**)&gpuTransposeMatrix, NUM * sizeof (float)));

  // Memory transfer from host to device
  HIP_CHECK (hipMemcpy (gpuMatrix, Matrix, NUM * sizeof (float), hipMemcpyHostToDevice));

  const uint32_t THREADS_PER_BLOCK_X = 4;
  const uint32_t THREADS_PER_BLOCK_Y = 4;
  const uint32_t THREADS_PER_BLOCK_Z = 1;
  const uint32_t GRID_X = uint32_t (WIDTH / THREADS_PER_BLOCK_X); // 256
  const uint32_t GRID_Y = uint32_t (WIDTH / THREADS_PER_BLOCK_Y); // 256
  dim3 dimGrid (GRID_X, GRID_Y); // 256, 256 (65,536)
  dim3 dimBlock (THREADS_PER_BLOCK_X, THREADS_PER_BLOCK_Y, THREADS_PER_BLOCK_Z); // 4, 4, 1 (16)

  // Lauching kernel from host
  matrixTranspose<<<dimGrid, dimBlock, 0, 0>>> (gpuTransposeMatrix, gpuMatrix, WIDTH);

  // Memory transfer from device to host
  HIP_CHECK (hipMemcpy (TransposeMatrix, gpuTransposeMatrix, NUM * sizeof(float), hipMemcpyDeviceToHost));

//  for (uint32_t i = 0; i < NUM; ++i)
//  {
//    printf ("Matrix[%d]: %.6f  |  TransposeMatrix[%d]: %.6f  |  cpuTransposeMatrix[%d]: %.6f\n", i, Matrix[i], i, TransposeMatrix[i], i, cpuTransposeMatrix[i]);
//  }

  int errors = 0;
  const float eps = 1.0E-6;
  printf ("The margin of error = %.6f\n", eps);

  // Verify the results
  for (uint32_t i = 0; i < NUM; ++i) 
  {
    if (std::abs (TransposeMatrix[i] - cpuTransposeMatrix[i]) > eps)
      errors++;
  }

  if (errors != 0)
  {
    printf ("FAILED: %d errors\n", errors);
  }
  else
  {
    printf ("PASSED!\n");
  }

  // Free the resources on device side
  HIP_CHECK (hipFree(gpuMatrix));
  HIP_CHECK (hipFree(gpuTransposeMatrix));

  // Free the resources on host side
  free (Matrix);
  free (TransposeMatrix);
  free (cpuTransposeMatrix);
  return errors;
}
