#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>
#include <assert.h>
#include "utility.c"

using namespace std;

#define TIME_NOW std::chrono::high_resolution_clock::now()
#define TIME_DIFF(gran, start, end) std::chrono::duration_cast<gran>(end - start).count()

#include "single_thread.h"
#include "multi_thread.h"

//file descriptors for profiler
int l1_fd,l1_total_fd,llc_fd,llc_total_fd,dtlb_fd,dtlb_total_fd,itlb_fd,itlb_total_fd,page_fd,cycles_fd;

static void inline initialize_profiling()
{
    l1_fd = initialize_L1_cache_event(l1_cache,0);
    l1_total_fd = initialize_L1_cache_event(l1_total,1);
    llc_fd = initialize_LLC_event(llc,0);
    llc_total_fd = initialize_LLC_event(llc_total,1);
    page_fd = initialize_PF_event(page_faults);

    ioctl(page_fd, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(l1_fd, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(l1_total_fd, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(llc_fd, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(llc_total_fd, PERF_EVENT_IOC_ENABLE, 0);
    // ioctl(dtlb_fd,PERF_EVENT_IOC_ENABLE,0);
    // ioctl(dtlb_total_fd,PERF_EVENT_IOC_ENABLE,0);


}

static void inline disable_profiling()
{
  // Disables profiling
    ioctl(page_fd, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(l1_fd, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(l1_total_fd, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(llc_fd, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(llc_total_fd, PERF_EVENT_IOC_DISABLE, 0);
    // ioctl(dtlb_fd,PERF_EVENT_IOC_DISABLE,0);
    // ioctl(dtlb_total_fd,PERF_EVENT_IOC_DISABLE,0);

    
}
static void inline print_values()
{
    read(l1_fd, &l1_cache.count, sizeof(long long));
    read(l1_total_fd, &l1_total.count, sizeof(long long));
    printf("No of L1 cache read misses : %lld/%lld\n",l1_cache.count,l1_total.count);

    read(llc_fd, &llc.count, sizeof(long long));
    read(llc_total_fd, &llc_total.count, sizeof(long long));
    printf("No of Last Level cache read misses : %lld/%lld\n",llc.count,llc_total.count);

  
    read(page_fd, &page_faults.count, sizeof(long long));
    printf("No of Page Faults : %lld\n",page_faults.count);

    // read(dtlb_fd, &data_TLB.count, sizeof(long long));
    // read(dtlb_total_fd, &data_TLB_total.count, sizeof(long long));
    // printf("No of Data TLB cache misses : %lld/%lld\n",data_TLB.count,data_TLB_total.count);

}

// Used to cross-check answer. DO NOT MODIFY!
void reference(int N, int *matA, int *matB, int *output)
{
  // enforce N to be power of 2 and greater than 2
  assert( N>=4 and N == ( N &~ (N-1)));
  for(int rowA = 0; rowA < N; rowA +=2) {
    for(int colB = 0; colB < N; colB += 2){
      int sum = 0;
      for(int iter = 0; iter < N; iter++) 
      {
        sum += matA[rowA * N + iter] * matB[iter * N + colB];
        sum += matA[(rowA+1) * N + iter] * matB[iter * N + colB];
        sum += matA[rowA * N + iter] * matB[iter * N + (colB+1)];
        sum += matA[(rowA+1) * N + iter] * matB[iter * N + (colB+1)];
      }

      // compute output indices
      int rowC = rowA>>1;
      int colC = colB>>1;
      int indexC = rowC * (N>>1) + colC;
      output[indexC] = sum;
    }
  }
}

void cache_friendly_ref(int N, int *matA, int *matB, int *output)
{
  // Transpose B 
    int *matBT = new int[N * N];
    for(int i = 0; i < N; ++i)
      for(int j = 0; j < N; ++j)
        matBT[i*N+j] = matB[j*N+i];

  assert( N>=4 and N == ( N &~ (N-1)));
  for(int rowA = 0; rowA < N; rowA +=2) {
    for(int colB = 0; colB < N; colB += 2){
      
      int sum = 0;
      for(int iter = 0; iter < N; iter++) 
      {
        // cout<<rowA * N + iter<<" "<<colB * N + iter<<"\t"<<(rowA+1) * N + iter<<" "<<(colB+1) * N + iter<<"\n";
        sum += matA[rowA * N + iter] * matBT[colB * N + iter];
        sum += matA[(rowA+1) * N + iter] * matBT[colB * N + iter];
        sum += matA[rowA * N + iter] * matBT[(colB+1) * N + iter];
        sum += matA[(rowA+1) * N + iter] * matBT[(colB+1) * N + iter];
      }

      // compute output indices
      int rowC = rowA>>1;
      int colC = colB>>1;
      int indexC = rowC * (N>>1) + colC;
      output[indexC] = sum;
    }
  }
}

int main(int argc, char *argv[])
{
  // Input size of square matrices
  int N;
  string file_name; 
  if (argc < 2) 
    file_name = "data/input_8.in"; 
  else 
    file_name = argv[1]; 
  ifstream input_file; 
  input_file.open(file_name); 
  input_file >> N;
  cout << "\nInput matrix of size " << N << "\n";

  // Input matrix A
  int *matA = new int[N * N];
  for(int i = 0; i < N; ++i)
    for(int j = 0; j < N; ++j)
      input_file >> matA[i * N + j];

  // Input matrix B
  int *matB = new int[N * N];
  for(int i = 0; i < N; ++i)
    for(int j = 0; j < N; ++j)
      input_file >> matB[i * N + j];

  // Untimed, warmup caches and TLB
  int *output_reference = new int[(N>>1)*(N>>1)];
  // reference(N, matA, matB, output_reference);

 
 
  // Execute reference program

  auto begin = TIME_NOW;
  // // Start profiling from this point onwards
  // initialize_profiling();  
  
  // reference(N, matA, matB, output_reference);
  
  // // disable profiling
  // disable_profiling();
  auto end = TIME_NOW;

  // cout << "Reference execution time: " << 
  //   (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";   
  // // Prints the required values
  // print_values();
    
    
// CACHE FRIENDLY 
  // int *output_cache_ref = new int[(N>>1)*(N>>1)];
  //  begin = TIME_NOW;
  
  // initialize_profiling();
  // cache_friendly_ref(N, matA, matB, output_cache_ref);
  //  end = TIME_NOW;

  // // Disables profiling
  //  disable_profiling();
  
  // // Prints the required values
  //   cout << "\nCache Friendly single thread execution time: " << 
  //   (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n"; 
  //   print_values();
   
  // for(int i = 0; i < ((N>>1)*(N>>1)); ++i)
  //   if(output_cache_ref[i] != output_reference[i]) {
  //     cout << "Mismatch at Cache " << i << "\n";
  //     exit(0);
  //   }




  // Execute single thread
  int *output_single = new int[(N>>1)*(N>>1)];
  begin = TIME_NOW;
  // Start profiling from this point onwards
    initialize_profiling();
  singleThread(N, matA, matB, output_single);
  end = TIME_NOW;
  
    // Disables profiling 
    disable_profiling();
    // Prints the required values

    // printf("Execution time (in seconds) : %d\n",(end-begin));
    cout << "\nSingle thread vectorized execution time: " << 
    (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";
    print_values();
    
    // for(int i = 0; i < ((N>>1)*(N>>1)); ++i)
    //   if(output_single[i] != output_reference[i]) {
    //     cout << "Mismatch at " << i << "\n";
    //     exit(0);
    // }
  
  // Execute multi-thread


  int max_threads = 32;
  // for (int threads=4;threads<=max_threads;threads*=2)
  // {
  //   int *output_multi = new int[(N>>1)*(N>>1)];
  //   begin = TIME_NOW;
  //   // Start profiling from this point onwards
  //   initialize_profiling();
    
  //   // CALL THE FUNCTION
  //   multiThread(N, matA, matB, output_multi,threads);
  //   end = TIME_NOW;
    
  //    // Disables profiling 
  //   disable_profiling();
  //   // Prints the required values
  //   cout << "\nMulti-threaded with total : "<<threads<<" threads execution time: " << 
  //     (double)TIME_DIFF(std::chrono::microseconds, begin, end) / 1000.0 << " ms\n";
    
  //   print_values();

  //   for(int i = 0; i < ((N>>1)*(N>>1)); ++i){
  //     if(output_multi[i] != output_reference[i]) {
  //         cout << "Mismatch at thread " << i<<" "<<output_multi[i] << "\n";
  //         exit(0);
  //       }
  //   }
  // }

  cout<<"\nSuccess!! All optimizations ran to completion\n";

  input_file.close(); 
  return 0; 
}
