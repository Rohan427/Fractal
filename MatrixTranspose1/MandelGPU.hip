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

int MandelGPU::mandelGPU()
{
  hipDeviceProp_t devProp;
  HIP_CHECK (hipGetDeviceProperties(&devProp, 0));
  std::cout << "Device name " << std::string (devProp.name) << std::endl;
  const uint32_t WIDTH = 64;
  const uint32_t NUM = WIDTH * WIDTH;
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
  const uint32_t GRID_X = uint32_t (WIDTH / THREADS_PER_BLOCK_X);
  const uint32_t GRID_Y = uint32_t (WIDTH / THREADS_PER_BLOCK_Y);
  dim3 dimGrid (GRID_X, GRID_Y);
  dim3 dimBlock (THREADS_PER_BLOCK_X, THREADS_PER_BLOCK_Y, THREADS_PER_BLOCK_Z);

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
