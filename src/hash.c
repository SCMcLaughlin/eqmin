
#include "hash.h"

static uint32_t hash_int64_impl(uint32_t lo, uint32_t hi)
{
    lo ^= hi;
    hi = bit_rotate(hi, 14);
    lo -= hi;
    hi = bit_rotate(hi, 5);
    hi ^= lo;
    hi -= bit_rotate(lo, 13);
    return hi;
}

uint32_t hash_int64(int64_t key)
{
    key++;
    return hash_int64_impl((uint32_t)(key & 0x00000000ffffffLL), (uint32_t)((key & 0xffffffff00000000LL) >> 32LL));
}

uint32_t hash_cstr(const char* key, uint32_t len)
{
    uint32_t h = len;
    uint32_t step = (len >> 5) + 1;
    uint32_t i;
    
    for (i = len; i >= step; i -= step)
    {
        h = h ^ ((h << 5) + (h >> 2) + (key[i - 1]));
    }
    
    return h;
}

#define NHASH 16

uint32_t hash_data(const void* data, uint32_t len)
{
    const byte* ptr = (const byte*)data;
    uint16_t a = 0;
    uint16_t b = 0;
    uint16_t c;
    uint32_t i;
    
    for (i = 0; i < len; i++)
    {
        c = (uint16_t)ptr[i];
        a += c;
        b += (NHASH - i) * c;
    }
    
    return (USHRT_MAX * (uint32_t)b) + a;
}

#undef NHASH
