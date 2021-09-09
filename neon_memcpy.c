#include <stdio.h>
#include <string.h>
#include <sse2neon.h>

void mesa__memcpy(void *restrict dst, void *restrict src, size_t len)
{
   char *restrict d = dst;
   char *restrict s = src;
   size_t padding;


   if (((uintptr_t)d & 15) != ((uintptr_t)s & 15)) {
      
      padding = (16 - (((size_t)dst) & 15)) & 15;

      if (padding > 0) {
         __m128i head = _mm_loadu_si128((const __m128i*)s);
         _mm_storeu_si128((__m128i*)d, head);
         dst += padding;
         src += padding;
         len -= padding;
      }


      
      for(int i=len;i>=64;i=i-64){
         __m128i *dst_cacheline = (__m128i *)d;
         __m128i *src_cacheline = (__m128i *)s;


         __m128i m0 = _mm_loadu_si128(((const __m128i*)src_cacheline) + 0);
         __m128i m1 = _mm_loadu_si128(((const __m128i*)src_cacheline) + 1);
         __m128i m2 = _mm_loadu_si128(((const __m128i*)src_cacheline) + 2);
         __m128i m3 = _mm_loadu_si128(((const __m128i*)src_cacheline) + 3);
         _mm_storeu_si128(((__m128i*)dst_cacheline) + 0, m0);
         _mm_storeu_si128(((__m128i*)dst_cacheline) + 1, m1);
         _mm_storeu_si128(((__m128i*)dst_cacheline) + 2, m2);
         _mm_storeu_si128(((__m128i*)dst_cacheline) + 3, m3);

         _mm_prefetch((const char*)(src_cacheline + 64), _MM_HINT_NTA);

         d += 64;
         s += 64;
         //len -= 64;
      }
   }


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
