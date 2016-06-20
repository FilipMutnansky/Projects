// bit-array.h 
// Riesenie IJC-DU1, priklad a), 19.3.2014 
// Autor: Filip Mutnansky, FIT
// Prelozene: gcc 4.6.4 
// Popis: Definicia makier pre bitove polia

#ifndef BITARRAY   // zabezpecenie proti viacnasobnej definicii
#define BITARRAY

#include "error.h"

#define BitArray(jmeno_pole,velikost) unsigned long jmeno_pole[(((8*sizeof(long)-1) + velikost)/(sizeof(long)*8)) + 1] = {velikost}

#define CALL_ERROR(name,i) (FatalError("Index %ld mimo rozsah 0..%ld", (long)i, (long)BitArraySize(name)))

#ifndef USE_INLINE   // bez tokenu USE_INLINE

#define BitArraySize(jmeno_pole) jmeno_pole[0] 

#define DU1_GET_BIT(p,i)  (((p)[i/(sizeof(p[0])*8)] & (1LU << (i%(8*(sizeof((p)[0])))))))

#define DU1_SET_BIT(p,i,b) ((p[i/(8*sizeof(p[0]))]) |= (((b) ? 1LU:0LU) << (i%(8*sizeof(p[0])))))

#define SetBit(name,i,vyraz) ((i > BitArraySize(name)) ? ((CALL_ERROR(name,i)),0LU): ((name[(i/(sizeof(long)*8)) + 1]) |= (((vyraz) ? 1LU:0LU) << (i%(8*sizeof(long))))))

#define GetBit(name,i) (((i) > BitArraySize(name)) ? ((CALL_ERROR(name,i)),0LU): (((name[(i/(sizeof(long)*8)) + 1]) & (1LU << ((i%(8*sizeof(long))))))))

#endif

typedef unsigned long BitArray_t[];   // typedef pre vlozenie do inline funkcii

#ifdef USE_INLINE   // ak je pouzity token USE_INLINE

static inline unsigned long BitArraySize(BitArray_t name)
{
	return name[0];	
}

static inline void SetBit(BitArray_t name, unsigned long i, unsigned long vyraz)
{
	((i > BitArraySize(name)) ? ((CALL_ERROR(name,i)),0LU): ((name[(i/(sizeof(long)*8)) + 1]) |= (((vyraz) ? 1LU:0LU) << (i%(8*sizeof(long))))));
	return;
}

static inline unsigned long GetBit(BitArray_t name, unsigned long i)
{
	return (((i) > BitArraySize(name)) ? ((CALL_ERROR(name,i)),0LU): (((name[(i/(sizeof(long)*8)) + 1]) & (1LU << ((i%(8*sizeof(long))))))));
}

#endif

#endif
