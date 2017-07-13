
#include "malloc.h"
#include "bitset.h"
#include "string.h"
#include "screen.h"

bitSet *initBitSet(char *bits, int totalBits)
{
    bitSet *ret;

    // must be a multiple of 8
    if (totalBits % 8 != 0) return 0;

    ret = (bitSet *) Malloc_Atomic(sizeof(bitSet));
    ret->size = totalBits;
    ret->bits = (char *) Malloc_Atomic(totalBits/8);
    if (bits) 
	memcpy((void *) ret->bits, (void *) bits, totalBits/8);
    else
	memset(ret->bits, totalBits/8, '\0');

    return ret;
}

int SetBit(bitSet *set, int bitPos)
{
    int element;
    int offset;

    if (bitPos < 0 || bitPos >= set->size) return -1;

    element = bitPos/8;
    offset = bitPos%8;

    set->bits[element] |= (1<<offset);
   
    return 0;
}

int ClearBit(bitSet *set, int bitPos)
{
    int element;
    int offset;

    if (bitPos < 0 || bitPos >= set->size) return -1;

    element = bitPos/8;
    offset = bitPos%8;

    set->bits[element] &= ~(1<<offset);
   
    return 0;
}

int IsBitSet(bitSet *set, int bitPos)
{
    int element;
    int offset;

    if (bitPos < 0 || bitPos >= set->size) return -1;

    element = bitPos/8;
    offset = bitPos%8;

    if (set->bits[element] & (1<<offset)) {
        return 1;
    } else {
	return 0;
    }
}

int FindFirstFreeBit(bitSet *set) 
{
    int i, j;

    for (i=0; i < set->size; i++) {
        if (set->bits[i] != 0xff) break;
    }

    if (i== set->size) return -1;

    for (j=0; j < 8; j++) {
        if (!((1 << j) & set->bits[i])) {
	    break;
	}
    }

    return (i * 8) | j;
}

// This is slow!!
//
int FindFirstNFree(bitSet *set, int runLength)
{
    int i,j;

    for (i=0; i < set->size-runLength; i++) {
        if (!IsBitSet(set, i)) {
	    for (j=1; j < runLength; j++) {
	        if (IsBitSet(set, i+j)) {
		    break;
		}
	    }
	    if (j == runLength) {
	        return i;
	    }
	}
    }
    return -1;
}

char *ReturnBits(bitSet *set)
{
    return set->bits;
}
