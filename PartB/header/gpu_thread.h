#include "cuda.h"
// Create other necessary functions here

// Fill in this function

__global__ void rmm(int N,int* d_A,int* d_B,int* d_C)
{
	int row = 2*(blockIdx.y * blockDim.y + threadIdx.y);
	int col = 2*(blockIdx.x * blockDim.x + threadIdx.x);
	int sum =0;
	for(int i=0;i<N;i++){

		sum += d_A[row*N+i]*d_B[i*N+col];
		sum += d_A[(row+1)*N+i]*d_B[i*N+col];
		sum += d_A[row*N+i]*d_B[i*N+col+1];
		sum += d_A[(row+1)*N+i]*d_B[i*N+(col+1)];
	}

	int rowC = row>>1;
	int colC = col>>1;
	int indexC = (rowC * (N>>1) + colC);
	//__syncthreads();
	//d_C[indexC] =d_C[indexC]+ sum;
	d_C[indexC] = sum;
	//printf("%d %d %d %d %d\n",row,col,sum,indexC,d_C[indexC]);


}


void gpuThread(int N, int *matA, int *matB, int *output)
{	
	// int sizeb = min(N/2,16);
	int sizeb	 = 32;
	dim3 block(sizeb,sizeb);
	dim3 grid(N/(2*sizeb),N/(2*sizeb));

	int* d_A;
	int* d_B;
	int* d_C;
	int sizeM=N;

	//Allocating space for our matrices in GPU Memory
	cudaMalloc(&d_A,N*N*sizeof(int));
	cudaMalloc(&d_B,N*N*sizeof(int));
	cudaMalloc(&d_C,N/2*N/2*sizeof(int));
	//cudaMalloc(sizeM,sizeof(int));


	//Actually transferring our matrices to GPU memory
	cudaMemcpy(d_A, matA, N*N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, matB, N*N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_C, output, N*N*sizeof(int), cudaMemcpyHostToDevice);
	
//	cudaError err = CudaDeviceSynchronize();
//	if( err != cudaSuccess){
//		cout<<"Memory Allocation Failed";
//		exit(0);
//	}
	rmm<<<grid,block>>>(sizeM,d_A,d_B,d_C);
	cudaDeviceSynchronize();
	// Getting Output from GPU memory
	cudaMemcpy(output, d_C,N/2*N/2*sizeof(int), cudaMemcpyDeviceToHost);

}


