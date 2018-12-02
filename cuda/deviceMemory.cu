#include <cstdio>
#include <cmath>
constexpr const int N=64;


__global__ void VecAdd(float* A,float* B,float* C)
{
  int i=blockDim.x*blockIdx.x+threadIdx.x;
  if(i<N) C[i]=A[i]+B[i];
}

__global__ void PitchAdd(float* devPtr, size_t pitch,int width, int height)
{
  for(int i=0;i<height;++i)
  {
    auto row=(float*)((char*)devPtr+i*pitch);
    for(int j=0;j<width;++j)
    {
      auto element=sin(row[j]);
    }
  }
}

__global__ void D3Add(cudaPitchedPtr devPitchedPtr, int width, int height, int depth)
{
  auto devPtr=(char*)devPitchedPtr.ptr;
  size_t pitch=devPitchedPtr.pitch;
  size_t slicePitch=pitch*height;

  for(int i=0;i<depth;++i)
  {
    char* slice=devPtr+i*slicePitch;
    for(int j=0;j<height;++j)
    {
      auto row=(float*)(slice+j*pitch);
      for(int k=0;k<width;++k)
      {
	auto element=row[k];
      }
    }
  }
}

int main()
{
  size_t  size= sizeof(float)*N; 
  auto A= new float[N];
  auto B= new float[N];
  auto C= new float[N];
 
  for(int i=0;i<N;++i)
  {
    A[i]=B[i]=i;
  }
  float* d_A,*d_B,*d_C;
  cudaMalloc(&d_A,size);
  cudaMalloc(&d_B,size);
  cudaMalloc(&d_C,size);
  cudaMemcpy(d_A,A,size,cudaMemcpyHostToDevice);
  cudaMemcpy(d_B,B,size,cudaMemcpyHostToDevice);

  int threadPerBlock=64;
  int blockPerGrid=(N+threadPerBlock-1) / threadPerBlock ;

  VecAdd<<<blockPerGrid,threadPerBlock>>>(d_A,d_B,d_C);
  cudaMemcpy(C,d_C,size,cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);

  int width=64,height=64;
  float* devPtr;
  size_t pitch;
  cudaMallocPitch(&devPtr,&pitch,width*sizeof(float),height);
  printf("pitch=%d\n",pitch);
  PitchAdd<<<100,512>>>(devPtr,pitch,width,height);

  cudaFree(devPtr);

  

  int depth=64;
  cudaExtent extent=make_cudaExtent(width*sizeof(float),height,depth);
  cudaPitchedPtr devPitchedPtr;
  cudaMalloc3D(&devPitchedPtr,extent);
  D3Add<<<100,512>>>(devPitchedPtr,width,height,depth);

  return 0;
}
