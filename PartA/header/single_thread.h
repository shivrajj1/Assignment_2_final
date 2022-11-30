// Optimize this function
#include "immintrin.h"


uint32_t hsum_epi32_avx(__m128i x)
{
    __m128i hi64  = _mm_unpackhi_epi64(x, x);           
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32  = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));    
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);       
}

// only needs AVX2
uint32_t hsum_8x32(__m256i v)
{
    __m128i sum128 = _mm_add_epi32( 
                 _mm256_castsi256_si128(v),
                 _mm256_extracti128_si256(v, 1)); 
    return hsum_epi32_avx(sum128);
}
// _m5
void singleThread(int N, int *matA, int *matB, int *output)
{
  assert( N>=4 and N == ( N &~ (N-1)));
  if(true)
  {

    // Transpose B 
    int *matBT = new int[N * N];
    for(int i = 0; i < N; ++i)
      for(int j = 0; j < N; ++j)
        matBT[i*N+j] = matB[j*N+i];

    for(int rowA = 0; rowA < N; rowA +=2) {
      for (int colB = 0; colB < N; colB +=2)
        {int sum = 0;
        __m256i mul_x8_1 = _mm256_setzero_si256();
        __m256i mul_x8_2 = _mm256_setzero_si256();
        __m256i mul_x8_3 = _mm256_setzero_si256();
        __m256i mul_x8_4 = _mm256_setzero_si256();  
        __m256i sumx_final = _mm256_setzero_si256();
        // cerr<<"Hello";
        // 256-bit vector and 32-bit int , so 256/32 = 8 elements in each vector
        for(int iter=0;iter<N;iter+=8)
        {
          __m256i a1 = _mm256_loadu_si256((__m256i *) &(matA[rowA*N+iter]));
          __m256i b1 = _mm256_loadu_si256((__m256i *) &(matBT[colB*N+iter]));
          __m256i a2 = _mm256_loadu_si256((__m256i *) &(matA[(rowA+1)*N+iter]));
          __m256i b2 = _mm256_loadu_si256((__m256i *) &(matBT[(colB+1)*N+iter]));;
          
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
        // cerr<<sum<<" ";

        // compute output indices
        int rowC = rowA>>1;
        int colC = colB>>1;
        int indexC = rowC * (N>>1) + colC;
        output[indexC] = sum;
      }
    }
  }
  else
  {
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
  
}
