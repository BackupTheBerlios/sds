// This may look like C code, but it is really -*- C++ -*-
#ifndef _SHAREDDATA_PUB_h
#define _SHAREDDATA_PUB_h

#include "SharedData.h"

#define SIZE_SZ                (sizeof(INTERNAL_SIZE_T))


#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGNMENT       (2 * SIZE_SZ)
#endif

#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)

#define fastbin_index(sz)        ((((unsigned int)(sz)) >> 3) - 2)

#define MAX_FAST_SIZE     80

#define NFASTBINS  (fastbin_index(request2size(MAX_FAST_SIZE))+1)

/*
  -----------------------  Chunk representations -----------------------
*/

struct malloc_chunk {

  INTERNAL_SIZE_T      prev_size;  /* Size of previous chunk (if free).  */
  INTERNAL_SIZE_T      size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;
};


typedef struct malloc_chunk* mchunkptr;

#define chunk2mem(p)   ((Void_t*)((char*)(p) + 2*SIZE_SZ))
#define mem2chunk(mem) ((mchunkptr)((char*)(mem) - 2*SIZE_SZ))

#define MIN_CHUNK_SIZE        (sizeof(struct malloc_chunk))

#define MINSIZE   ((MIN_CHUNK_SIZE+MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)

#define aligned_OK(m)  (((unsigned long)((m)) & (MALLOC_ALIGN_MASK)) == 0)

#define IS_NEGATIVE(x) \
  ((unsigned long)x >= \
   (unsigned long)((((INTERNAL_SIZE_T)(1)) << ((SIZE_SZ)*8 - 1))))


#define request2size(req)                                        \
  (((req) + SIZE_SZ + MALLOC_ALIGN_MASK < MINSIZE)  ?            \
   MINSIZE :                                                     \
   ((req) + SIZE_SZ + MALLOC_ALIGN_MASK) & ~MALLOC_ALIGN_MASK)

#define checked_request2size(req, sz)                                    \
   if (IS_NEGATIVE(req)) {                                               \
     MALLOC_FAILURE_ACTION;                                              \
     return 0;                                                           \
   }                                                                     \
   (sz) = request2size(req);



#define NBINS        128

#define BINMAPSHIFT      5
#define BITSPERMAP       (1U << BINMAPSHIFT)
#define BINMAPSIZE       (NBINS / BITSPERMAP)

struct mfastbin {
  mchunkptr hd;
  mchunkptr tl;
};

struct DataSet;

struct malloc_state {
	int size; // size of sd-file; has to be the first element
	void *mmantop; // addess of the next return value of morecore
	DataSet *sds;
	int     sdsize;
	int   logstate;
	int   nid;
	unsigned int   state;
	INTERNAL_SIZE_T  max_fast;   /* low bit used as consolidation flag */
	INTERNAL_SIZE_T  fastbytes;
	mchunkptr        top;
	mchunkptr        last_remainder;
	struct mfastbin  fastbins[NFASTBINS];
	mchunkptr        bins[NBINS * 2];
	unsigned int     binmap[BINMAPSIZE];
	unsigned long    trim_threshold;
	INTERNAL_SIZE_T  top_pad;
	unsigned int     pagesize;    /* Cache malloc_getpagesize */
	char*            sbrk_base;   /* first address returned by sbrk, 
					 or NONCONTIGUOUS_REGIONS */
	INTERNAL_SIZE_T  sbrked_mem;
	INTERNAL_SIZE_T  max_sbrked_mem;
	INTERNAL_SIZE_T  max_total_mem;
};
typedef malloc_state *mstate;

#define ADDR 0x50000000
#define INITIAL_SIZE 0x100000
#define IDENTIFIER "sdf-size="
#define SD_FILE "SDF"
#define DEFAULT_SDSIZE 10;


#endif  // _SHAREDDATA_PUB_h
