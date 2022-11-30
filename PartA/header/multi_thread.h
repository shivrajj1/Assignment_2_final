#include <pthread.h>
int MAX_T;

int mat_size;
int* A;
int* B;
int* C;

void multiply(int thread_id,int N, int *matA, int *matB, int *output)
{   
  assert( N>=4 and N == ( N &~ (N-1)));
  
    // Transpose B 
    

    for(int rowA = thread_id*2; rowA < N; rowA +=2*MAX_T) {
        // cerr<<thread_id<<endl;
      for (int colB = 0; colB < N; colB +=2)
        {
            int sum = 0;
        __m256i mul_x8_1 = _mm256_setzero_si256();
        __m256i mul_x8_2 = _mm256_setzero_si256();
        __m256i mul_x8_3 = _mm256_setzero_si256();
        __m256i mul_x8_4 = _mm256_setzero_si256();  
        __m256i sumx_final = _mm256_setzero_si256();

        for(int iter=0;iter<N;iter+=8)
        {
          // cerr<<"Hello Inside";
          __m256i a1 = _mm256_loadu_si256((__m256i *) &(matA[rowA*N+iter]));
          __m256i b1 = _mm256_loadu_si256((__m256i *) &(matB[colB*N+iter]));
          __m256i a2 = _mm256_loadu_si256((__m256i *) &(matA[(rowA+1)*N+iter]));
          __m256i b2 = _mm256_loadu_si256((__m256i *) &(matB[(colB+1)*N+iter]));

          mul_x8_1 =_mm256_mullo_epi32(a1,b1);
          sumx_final = _mm256_add_epi32(sumx_final,mul_x8_1);
          
          mul_x8_2 = _mm256_mullo_epi32(a1,b2);
          sumx_final = _mm256_add_epi32(sumx_final,mul_x8_2);
          
          mul_x8_3 =_mm256_mullo_epi32(a2,b1);
          sumx_final = _mm256_add_epi32(sumx_final,mul_x8_3);
          
          mul_x8_4 =_mm256_mullo_epi32(a2,b2);
          sumx_final = _mm256_add_epi32(sumx_final,mul_x8_4);
      
        }
        sum = hsum_8x32(sumx_final);
        // compute output indices
        int rowC = rowA>>1;
        int colC = colB>>1;
        int indexC = rowC * (N>>1) + colC;
        // cerr<<rowC<<" "<<colC<<" "<<sum<<" ";
        output[indexC] = sum;
        // cerr<<C[indexC]<<" ";
      }
    //   cerr<<endl;
    }
  
}
void* assign(void* id){
    multiply((long) id,mat_size,A,B,C);
}
void multiThread(int N, int *matA, int *matB, int *output,int max_threads)
{
    MAX_T = max_threads;
    pthread_t threads[MAX_T];
    A= matA;    
    int *matBT = new int[N * N];
    for(int i = 0; i < N; ++i)
      for(int j = 0; j < N; ++j)
        matBT[i*N+j] = matB[j*N+i];
    B= matBT;
    C=output;
    mat_size = N;
    // for(int i = 0; i < ((N>>1)*(N>>1)); ++i){
    //     cout<<C[i]<<" "; 
    // }


    for(int i=0;i<MAX_T;i++)
        pthread_create(&threads[i], NULL,assign, (void*)(i));

    for(int i=0;i<MAX_T;i++)
        pthread_join(threads[i],NULL);
    // for(int i = 0; i < ((N>>1)*(N>>1)); ++i)
    //     cout<<C[i]<<" ";
    // output = C;

}
