#include <stdio.h>
#include <string.h>
#include <sse2neon.h>

void mesa__memcpy(void *restrict dst, void *restrict src, size_t len)
{
   char *restrict d = dst;
   char *restrict s = src;

   /* If dst and src are not co-aligned, fallback to memcpy(). */
   if (((uintptr_t)d & 15) != ((uintptr_t)s & 15)) {
      printf("not used");
      memcpy(d, s, len);
      return;
   }

   /* memcpy() the misaligned header. At the end of this if block, <d> and <s>
    * are aligned to a 16-byte boundary or <len> == 0.
    */
   //omp_set_dynamic(0);     // Explicitly disable dynamic teams
   //omp_set_num_threads(2);
   //#pragma omp parallel for default(shared) shared(d,s)
   #pragma omp parallel for reduction (+:d)
   for(int i=len;i>=64;i=i-64){
      __m128i *dst_cacheline = (__m128i *)d;
      __m128i *src_cacheline = (__m128i *)s;

      __m128i temp1 = _mm_stream_load_si128(src_cacheline + 0);
      __m128i temp2 = _mm_stream_load_si128(src_cacheline + 1);
      __m128i temp3 = _mm_stream_load_si128(src_cacheline + 2);
      __m128i temp4 = _mm_stream_load_si128(src_cacheline + 3);

      _mm_store_si128(dst_cacheline + 0, temp1);
      _mm_store_si128(dst_cacheline + 1, temp2);
      _mm_store_si128(dst_cacheline + 2, temp3);
      _mm_store_si128(dst_cacheline + 3, temp4);

      d += 64;
      s += 64;
      //len -= 64;

   }
   /* memcpy() the tail. */
   if (len) {
      memcpy(d, s, len);
   }
}
