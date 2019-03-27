/**
 * Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

/**
 * Vector addition: C = A + B.
 *
 * This sample is a very basic sample that implements element by element
 * vector addition. It is the same as the sample illustrating Chapter 2
 * of the programming guide with some additions like error checking.
 */

#include <stdio.h>

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>

/**
 * CUDA Kernel Device code
 *
 * Computes the vector addition of A and B into C. The 3 vectors have the same
 * number of elements numElements.
 */

__global__ void
verifyCollatz(int64_t maxNumber)
{
	int timesToRunGrid = maxNumber / (blockDim.x * gridDim.x) + 1;

	int64_t number = 0;
	int64_t i = 0;

	for (int64_t gridRunNumber = 0; gridRunNumber < timesToRunGrid; ++gridRunNumber) {
		// odd numbers only
		number = 2 * (blockDim.x * gridDim.x * gridRunNumber + blockDim.x * blockIdx.x + threadIdx.x) + 1;
		i = number;
		if (number > 2 && number < maxNumber) {
			while (i >= number) {
				if (i & 0x1) {
					/* odd case */
					i = i * 3 + 1;
				} else {
					/* even case */
					i = i >> 1;
				}
			}
		}
	}
}

/**
 * Host main routine
 */
int
main()
{
    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;
    int64_t maxNumber = 256ll * 256ll * 256ll * 256ll;


    // Launch the Vector Add CUDA Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = 256;

	// use CUDA builtin heruistics to get max performance
	cudaOccupancyMaxPotentialBlockSize(
        &blocksPerGrid,
        &threadsPerBlock,
        (void*) verifyCollatz,
        0, 0);
	
	printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
	verifyCollatz<<<blocksPerGrid, threadsPerBlock>>>(maxNumber);
	err = cudaGetLastError();

	cudaDeviceSynchronize();
	
	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to launch collatz  kernel (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

    printf("Done\n");
    return 0;
}

