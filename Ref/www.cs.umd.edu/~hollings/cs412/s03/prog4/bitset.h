#ifndef BITSET_H
#define BITSET_H

typedef struct {
    int size;
    unsigned char *bits;
} bitSet;

bitSet *initBitSet(char *bits, int totalBits);
int SetBit(bitSet *set, int bitPos);
int ClearBit(bitSet *set, int bitPos);
int IsBitSet(bitSet *set, int bitPos);
int FindFirstFreeBit(bitSet *set);
int FindFirstNFree(bitSet *set, int runLength);
char *ReturnBits(bitSet *set);

#endif
