#include "fastHash.h"
#include <stdint.h>

uint32_t rokkit(const char * data, int len) {
  uint32_t hash, tmp;
  int rem;

    if (len <= 0 || data == 0) return 0;
    hash = len;
    rem = len & 3;
    len >>= 2;

    /* Main loop */
    while (len > 0) {
        hash  += *((uint16_t*)data);
        tmp    = (*((uint16_t*)(data+2)) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*2;
        hash  += hash >> 11;
		len--;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += *((uint16_t*)data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[2]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += *((uint16_t*)data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
