/* This code originates from 
 *
 *                          malloc-2.7.0pre6.c
 *
 * which is included in the source code of this distribution and where
 * you can find the following version information:
 
 *  A version (aka dlmalloc) of malloc/free/realloc written by Doug
 *  Lea and released to the public domain.  Use this code without
 *  permission or acknowledgement in any way you wish.  Send
 *  questions, comments, complaints, performance data, etc to
 *  dl@cs.oswego.edu

 * VERSION 2.7.0pre6 Thu Nov  2 14:42:01 2000  Doug Lea  (dl at gee)

 * Note: There may be an updated version of this malloc obtainable at
 * ftp://gee.cs.oswego.edu/pub/misc/malloc.c
 *
 * Check before installing!

 * However, this is what happens when RSCH modifies things for use
 * with SharedData
 *
 *  i) stripping win32-code
 *
 *  ii) stipping comments

 * The interested user is refered to the original source code: */

#include "SharedData_pub.h"

#include <unistd.h>
#include <errno.h>

#if DEBUG
#include <assert.h>
#else
#define assert(x) ((void)0)
#endif

/*   #define REALLOC_ZERO_BYTES_FREES */


#define HAVE_MEMCPY

#ifndef USE_MEMCPY
#ifdef HAVE_MEMCPY
#define USE_MEMCPY 1
#else
#define USE_MEMCPY 0
#endif
#endif

extern "C" {
void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
}

#ifndef MALLOC_FAILURE_ACTION
#define MALLOC_FAILURE_ACTION \
   errno = ENOMEM;
#endif

/* SVID2/XPG mallinfo structure */

struct mallinfo {
  int arena;    /* space allocated from system */
  int ordblks;  /* number of free chunks */
  int smblks;   /* number of fastbin blocks */
  int hblks;    /* number of mmapped regions */
  int hblkhd;   /* space in mmapped regions */
  int usmblks;  /* maximum total allocated space */
  int fsmblks;  /* space available in freed fastbin blocks */
  int uordblks; /* total allocated space */
  int fordblks; /* total free space */
  int keepcost; /* top-most, releasable (via malloc_trim) space */
};

/* SVID2/XPG mallopt options */

#define M_MXFAST  1    /* Set maximum fastbin size */
#define M_NLBLKS  2    /* UNUSED in this malloc */
#define M_GRAIN   3    /* UNUSED in this malloc */
#define M_KEEP    4    /* UNUSED in this malloc */


/* Additional mallopt options supported in this malloc */

#ifndef M_TRIM_THRESHOLD
#define M_TRIM_THRESHOLD    -1
#endif

#ifndef M_TOP_PAD
#define M_TOP_PAD           -2
#endif

#ifndef DEFAULT_MXFAST
#define DEFAULT_MXFAST      64
#endif

#ifndef DEFAULT_TRIM_THRESHOLD
#define DEFAULT_TRIM_THRESHOLD (128 * 1024)
#endif

#ifndef DEFAULT_TOP_PAD
#define DEFAULT_TOP_PAD        (0)
#endif


#define MORECORE morecore
#define MORECORE_CONTIGUOUS 1

#define MORECORE_FAILURE (-1)

#ifndef MORECORE_CONTIGUOUS
#define MORECORE_CONTIGUOUS 1
#endif

#ifndef malloc_getpagesize

#ifndef LACKS_UNISTD_H
#  include <unistd.h>
#endif

#  ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
#    ifndef _SC_PAGE_SIZE
#      define _SC_PAGE_SIZE _SC_PAGESIZE
#    endif
#  endif

#  ifdef _SC_PAGE_SIZE
#    define malloc_getpagesize sysconf(_SC_PAGE_SIZE)
#  else
#    if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
       extern size_t getpagesize();
#      define malloc_getpagesize getpagesize()
#    else
#      ifdef WIN32
#        define malloc_getpagesize (4096) 
#      else
#        ifndef LACKS_SYS_PARAM_H
#          include <sys/param.h>
#        endif
#        ifdef EXEC_PAGESIZE
#          define malloc_getpagesize EXEC_PAGESIZE
#        else
#          ifdef NBPG
#            ifndef CLSIZE
#              define malloc_getpagesize NBPG
#            else
#              define malloc_getpagesize (NBPG * CLSIZE)
#            endif
#          else
#            ifdef NBPC
#              define malloc_getpagesize NBPC
#            else
#              ifdef PAGESIZE
#                define malloc_getpagesize PAGESIZE
#              else /* just guess */
#                define malloc_getpagesize (4096) 
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

/* ---------- To make a malloc.h, end cutting here ------------ */

/* ------------- Optional versions of memcopy ---------------- */

#if USE_MEMCPY

#define MALLOC_COPY(dest, src, nbytes)  memcpy(dest, src, nbytes)
#define MALLOC_ZERO(dest, nbytes)       memset(dest, 0,   nbytes)

#else /* !USE_MEMCPY */

/* Use Duff's device for good zeroing/copying performance. */

#define MALLOC_ZERO(charp, nbytes)                                            \
do {                                                                          \
  INTERNAL_SIZE_T* mzp = (INTERNAL_SIZE_T*)(charp);                           \
  long mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T), mcn;                         \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mzp++ = 0;                                             \
    case 7:           *mzp++ = 0;                                             \
    case 6:           *mzp++ = 0;                                             \
    case 5:           *mzp++ = 0;                                             \
    case 4:           *mzp++ = 0;                                             \
    case 3:           *mzp++ = 0;                                             \
    case 2:           *mzp++ = 0;                                             \
    case 1:           *mzp++ = 0; if(mcn <= 0) break; mcn--; }                \
  }                                                                           \
} while(0)

#define MALLOC_COPY(dest,src,nbytes)                                          \
do {                                                                          \
  INTERNAL_SIZE_T* mcsrc = (INTERNAL_SIZE_T*) src;                            \
  INTERNAL_SIZE_T* mcdst = (INTERNAL_SIZE_T*) dest;                           \
  long mctmp = (nbytes)/sizeof(INTERNAL_SIZE_T), mcn;                         \
  if (mctmp < 8) mcn = 0; else { mcn = (mctmp-1)/8; mctmp %= 8; }             \
  switch (mctmp) {                                                            \
    case 0: for(;;) { *mcdst++ = *mcsrc++;                                    \
    case 7:           *mcdst++ = *mcsrc++;                                    \
    case 6:           *mcdst++ = *mcsrc++;                                    \
    case 5:           *mcdst++ = *mcsrc++;                                    \
    case 4:           *mcdst++ = *mcsrc++;                                    \
    case 3:           *mcdst++ = *mcsrc++;                                    \
    case 2:           *mcdst++ = *mcsrc++;                                    \
    case 1:           *mcdst++ = *mcsrc++; if(mcn <= 0) break; mcn--; }       \
  }                                                                           \
} while(0)

#endif

/*
  Physical chunk operations
*/

#define PREV_INUSE 0x1


/* Hi, it's me, RSCH!
 * There is no room for this kind of mmap: */
/* size field is or'ed with IS_MMAPPED if the chunk was obtained with mmap() */
/*
#define IS_MMAPPED 0x2
*/

#define SIZE_BITS (PREV_INUSE)

#define next_chunk(p) ((mchunkptr)( ((char*)(p)) + ((p)->size & ~PREV_INUSE) ))
#define prev_chunk(p) ((mchunkptr)( ((char*)(p)) - ((p)->prev_size) ))

#define chunk_at_offset(p, s)  ((mchunkptr)(((char*)(p)) + (s)))

#define inuse(p)\
((((mchunkptr)(((char*)(p))+((p)->size & ~PREV_INUSE)))->size) & PREV_INUSE)

#define prev_inuse(p)       ((p)->size & PREV_INUSE)

#define set_inuse(p)\
((mchunkptr)(((char*)(p)) + ((p)->size & ~PREV_INUSE)))->size |= PREV_INUSE

#define clear_inuse(p)\
((mchunkptr)(((char*)(p)) + ((p)->size & ~PREV_INUSE)))->size &= ~(PREV_INUSE)

#define inuse_bit_at_offset(p, s)\
 (((mchunkptr)(((char*)(p)) + (s)))->size & PREV_INUSE)

#define set_inuse_bit_at_offset(p, s)\
 (((mchunkptr)(((char*)(p)) + (s)))->size |= PREV_INUSE)

#define clear_inuse_bit_at_offset(p, s)\
 (((mchunkptr)(((char*)(p)) + (s)))->size &= ~(PREV_INUSE))

#define chunksize(p)         ((p)->size & ~(SIZE_BITS))

#define set_head_size(p, s)  ((p)->size = (((p)->size & PREV_INUSE) | (s)))

#define set_head(p, s)       ((p)->size = (s))

#define set_foot(p, s)       (((mchunkptr)((char*)(p) + (s)))->prev_size = (s))




typedef struct malloc_chunk* mbinptr;

#define bin_at(m, i) ((mbinptr)((char*)&((m)->bins[(i)<<1]) - (SIZE_SZ<<1)))

#define next_bin(b)  ((mbinptr)((char*)(b) + (sizeof(mchunkptr)<<1)))

#define first(b)     ((b)->fd)
#define last(b)      ((b)->bk)

#define unlink(P, BK, FD) {                                            \
  FD = P->fd;                                                          \
  BK = P->bk;                                                          \
  FD->bk = BK;                                                         \
  BK->fd = FD;                                                         \
}

#define NSMALLBINS         64
#define SMALLBIN_WIDTH      8
#define MIN_LARGE_SIZE    512

#define in_smallbin_range(sz)  ((sz) <  MIN_LARGE_SIZE)

#define smallbin_index(sz)     (((unsigned)(sz)) >> 3)

#define largebin_index(sz)                                                   \
(((((unsigned long)(sz)) >>  6) <= 32)?  56 + (((unsigned long)(sz)) >>  6): \
 ((((unsigned long)(sz)) >>  9) <= 20)?  91 + (((unsigned long)(sz)) >>  9): \
 ((((unsigned long)(sz)) >> 12) <= 10)? 110 + (((unsigned long)(sz)) >> 12): \
 ((((unsigned long)(sz)) >> 15) <=  4)? 119 + (((unsigned long)(sz)) >> 15): \
 ((((unsigned long)(sz)) >> 18) <=  2)? 124 + (((unsigned long)(sz)) >> 18): \
                                        126)

#define bin_index(sz) \
 ((in_smallbin_range(sz)) ? smallbin_index(sz) : largebin_index(sz))

#define unsorted_chunks(M)          (bin_at(M, 1))

#define initial_top(M)              (unsorted_chunks(M))

#define idx2block(i)     ((i) >> BINMAPSHIFT)
#define idx2bit(i)       ((1U << ((i) & ((1U << BINMAPSHIFT)-1))))

#define mark_bin(m,i)    ((m)->binmap[idx2block(i)] |=  idx2bit(i))
#define unmark_bin(m,i)  ((m)->binmap[idx2block(i)] &= ~(idx2bit(i)))
#define get_binmap(m,i)  ((m)->binmap[idx2block(i)] &   idx2bit(i))

typedef struct mfastbin* mfastbinptr;

#define fastbin_index(sz)        ((((unsigned int)(sz)) >> 3) - 2)

#define MAX_FAST_SIZE     80

#define NFASTBINS  (fastbin_index(request2size(MAX_FAST_SIZE))+1)

#define need_consolidation(M)       (((M)->max_fast & 1U) == 0)
#define set_consolidation_flag(M)   ((M)->max_fast &= ~1U)
#define clear_consolidation_flag(M) ((M)->max_fast |= 1)

#define FASTBIN_CONSOLIDATION_THRESHOLD  4096

#define NONCONTIGUOUS_REGIONS ((char*)(-3))

#define get_malloc_state() (avp)

#include <stdlib.h>
#include <sys/mman.h>

/*
  General Requirements for MORECORE.

  The MORECORE function must have the following properties:

  If MORECORE_CONTIGUOUS is false:

    * MORECORE must allocate in multiples of pagesize. It will
      only be called with arguments that are multiples of pagesize.

    * MORECORE must page-align. That is, MORECORE(0) must
      return an address at a page boundary.

  else (i.e. If MORECORE_CONTIGUOUS is true):

    * Consecutive calls to MORECORE with positive arguments
      return increasing addresses, indicating that space has been
      contiguously extended.

    * MORECORE need not allocate in multiples of pagesize.
      Calls to MORECORE need not have args of multiples of pagesize.

    * MORECORE need not page-align.

  In either case:

    * MORECORE must not allocate memory when given argument zero, but
      instead return one past the end address of memory from previous
      nonzero call. This malloc does NOT call MORECORE(0)
      until at least one call with positive arguments is made, so
      the initial value returned is not important.

    * Even though consecutive calls to MORECORE need not return contiguous
      addresses, it must be OK for malloc'ed chunks to span multiple
      regions in those cases where they do happen to be contiguous.

    * MORECORE need not handle negative arguments -- it may instead
      just return MORECORE_FAILURE when given negative arguments.
      Negative arguments are always multiples of pagesize. However,
      it must not misinterpret negative args as large positive unsigned
      args.

  There is some variation across systems about the type of the
  argument to sbrk/MORECORE. If size_t is unsigned, then it cannot
  actually be size_t, because sbrk supports negative args, so it is
  normally the signed type of the same width as size_t (sometimes
  declared as "intptr_t", and sometimes "ptrdiff_t").  It doesn't much
  matter though. Internally, we use "long" as arguments, which should
  work across all reasonable possibilities.

  Additionally, if MORECORE ever returns failure for a positive
  request, and HAVE_MMAP is true, then mmap is used as a noncontiguous
  system allocator. This is a useful backup strategy for systems with
  holes in address spaces -- in this case sbrk cannot contiguously
  expand the heap, but mmap may be able to map noncontiguous space.
  If you'd like mmap to ALWAYS be used, you can define MORECORE to be
  a function that always returns MORECORE_FAILURE.

  If you are using this malloc with something other than unix sbrk to
  supply memory regions, you probably want to set MORECORE_CONTIGUOUS
  as false.  

  As an example, here is a custom allocator 

  Hi, it's me, RSCH! I modified things to work as member function of a
  class called SharedData. Here  "morecore" is a member
  function as well. It obeys (hopefully) the above rules (for contigous
  allocation)

*/

void fatal(const char *templ, ...);

void *SharedData::morecore(int si)
{
	char h=(char)0;
	char *seg;
	void *ret;

	if(si==0)
		return avp->mmantop;
	else if(si>0) {
		errno=0;
		if(fseeko(fp,si-1,SEEK_END))
			fatal("morecores lseek operation fails");
		fwrite(&h,sizeof(char),1,fp);
		if(avp->mmantop==NULL) {
			avp->mmantop=(void*)(((char*)avp)+
					     sizeof(malloc_state));
			ret=avp->mmantop;
		} else {
			ret=avp->mmantop;
			avp->mmantop=(void*)((char*)avp->mmantop+si);
		}
		
		avp->size+=si;
		
		errno=0;
		seg=(char*)mmap((void*)ADDR,avp->size,
				PROT_READ|PROT_WRITE,MAP_FIXED|MAP_SHARED,
				fileno(fp),0);

		if(seg!=(char*)ADDR)
			fatal("morecores mmap increase operation results in"
			      " %p instead of %p",seg,(void*)ADDR);

		return ret;
	} else {
		avp->size+=si;

		errno=0;
		seg=(char*)mmap((void*)ADDR,avp->size,
				PROT_READ|PROT_WRITE,MAP_FIXED|MAP_SHARED,
				fileno(fp),0);
		if(seg!=(char*)ADDR)
			fatal("morecores mmap decrease operation results in"
			      " %p instead of %p",seg,(void*)ADDR);

		if(ftruncate(fileno(fp),avp->size)==-1)
			fatal("morecores ftruncate operation failed");
		avp->mmantop=(void*)((char*)avp->mmantop+si);
		return avp->mmantop;
	}
}

/*
  Initialize a malloc_state struct.
  This is called only via a check in malloc_consolidate, which needs
  be called on first malloc call anyway.
*/

void SharedData::malloc_init_state(mstate av)
{
  int     i;
  mbinptr bin;

  /* Establish circular links for normal bins */
  for (i = 1; i < NBINS; ++i) { 
    bin = bin_at(av,i);
    bin->fd = bin->bk = bin;
  }

  av->max_fast       = (DEFAULT_MXFAST == 0)? 0: request2size(DEFAULT_MXFAST);
  av->trim_threshold = DEFAULT_TRIM_THRESHOLD;
  av->top_pad        = DEFAULT_TOP_PAD;
  av->top            = initial_top(av);

#if MORECORE_CONTIGUOUS
  av->sbrk_base      = (char*)MORECORE_FAILURE;
#else
  av->sbrk_base      = NONCONTIGUOUS_REGIONS;
#endif

  av->pagesize       = malloc_getpagesize;

  clear_consolidation_flag(av);
}

#if ! DEBUG

#define check_chunk(P)
#define check_free_chunk(P)
#define check_inuse_chunk(P)
#define check_remalloced_chunk(P,N)
#define check_malloced_chunk(P,N)
#define check_malloc_state()

#else
#define check_chunk(P)              do_check_chunk(P)
#define check_free_chunk(P)         do_check_free_chunk(P)
#define check_inuse_chunk(P)        do_check_inuse_chunk(P)
#define check_remalloced_chunk(P,N) do_check_remalloced_chunk(P,N)
#define check_malloced_chunk(P,N)   do_check_malloced_chunk(P,N)
#define check_malloc_state()        do_check_malloc_state()


/*
  Properties of all chunks
*/
void SharedData::do_check_chunk(mchunkptr p)
{
	mstate av = get_malloc_state();
	unsigned long sz = chunksize(p);

	if (av->sbrk_base != NONCONTIGUOUS_REGIONS) {
		assert(((char*)p) >= ((char*)(av->sbrk_base)));
	}

	if (p != av->top) {
		if (av->sbrk_base != NONCONTIGUOUS_REGIONS) {
			assert(((char*)p + sz) <= ((char*)(av->top)));
		}
	}
	else {
		if (av->sbrk_base != NONCONTIGUOUS_REGIONS) {
			assert(((char*)p + sz) <= 
			       ((char*)(av->sbrk_base) + av->sbrked_mem));
		}
		assert((long)(sz) >= (long)(MINSIZE));
		assert(prev_inuse(p));
	}
}

/*
  Properties of free chunks
*/
void SharedData::do_check_free_chunk(mchunkptr p)
{
	mstate av = get_malloc_state();
	
	INTERNAL_SIZE_T sz = p->size & ~PREV_INUSE;
	mchunkptr next = chunk_at_offset(p, sz);

	do_check_chunk(p);

	assert(!inuse(p));

	if ((unsigned long)sz >= (unsigned long)MINSIZE) {
		assert((sz & MALLOC_ALIGN_MASK) == 0);
		assert(aligned_OK(chunk2mem(p)));
		assert(next->prev_size == sz);
		assert(prev_inuse(p));
		assert (next == av->top || inuse(next));

		assert(p->fd->bk == p);
		assert(p->bk->fd == p);
	} else 
		assert(sz == SIZE_SZ);
}

/*
  Properties of inuse chunks
*/
void SharedData::do_check_inuse_chunk(mchunkptr p)
{
	mstate av = get_malloc_state();
	mchunkptr next;
	do_check_chunk(p);
	
	assert(inuse(p));

	next = next_chunk(p);
	if (!prev_inuse(p))  {
		mchunkptr prv = prev_chunk(p);
		assert(next_chunk(prv) == p);
		do_check_free_chunk(prv);
	}

	if (next == av->top) {
		assert(prev_inuse(next));
		assert(chunksize(next) >= MINSIZE);
	} else if (!inuse(next))
		do_check_free_chunk(next);

}

/*
  Properties of chunks recycled from fastbins
*/
void SharedData::do_check_remalloced_chunk(mchunkptr p, INTERNAL_SIZE_T s)
{

	INTERNAL_SIZE_T sz = p->size & ~PREV_INUSE;
	
	do_check_inuse_chunk(p);

	assert((sz & MALLOC_ALIGN_MASK) == 0);
	assert((long)sz - (long)MINSIZE >= 0);
	assert((long)sz - (long)s >= 0);
	assert((long)sz - (long)(s + MINSIZE) < 0);

	assert(aligned_OK(chunk2mem(p)));
}

/*
  Properties of nonrecycled chunks at the point they are malloced
*/
void SharedData::do_check_malloced_chunk(mchunkptr p, INTERNAL_SIZE_T s)
{
	do_check_remalloced_chunk(p, s);

	assert(prev_inuse(p));
}


/*
  Properties of malloc_state.

  This may be useful for debugging malloc, as well as detecting user
  programmer errors that somehow write into malloc_state.
*/
void SharedData::do_check_malloc_state(void)
{
	mstate av = get_malloc_state();
	int i;
	mchunkptr p;
	mchunkptr q;
	mbinptr b;
	mfastbinptr f;
	unsigned int biton;
	int empty;
	unsigned int idx;
	INTERNAL_SIZE_T size;
	unsigned long total = 0;
	int max_fast_bin;
	
	assert(sizeof(INTERNAL_SIZE_T) <= sizeof(char*));
	assert((MALLOC_ALIGNMENT & (MALLOC_ALIGNMENT-1)) == 0);
	if (av->top == 0 || av->top == initial_top(av))
		return;
	assert((av->pagesize & (av->pagesize-1)) == 0);
	assert((av->max_fast & ~1) <= request2size(MAX_FAST_SIZE));
	max_fast_bin = fastbin_index(av->max_fast);
	for (i = 0; (unsigned)i < NFASTBINS; ++i) {
		f = &(av->fastbins[i]);
		p = f->hd;
		if (i > max_fast_bin)
			assert(p == 0);
		assert((p == 0) == (f->tl == 0));
		while (p != 0) {
			do_check_inuse_chunk(p);
			total += chunksize(p);
			assert(fastbin_index(chunksize(p)) == (unsigned)i);
			if (p->fd == 0)
				assert(p == f->tl);
			p = p->fd;
		}
	}

	assert (total == av->fastbytes);
	for (i = 1; i < NBINS; ++i) {
		b = bin_at(av,i);
		
		if (i >= 2) {
			biton = get_binmap(av,i);
			empty = last(b) == b;
			if (!biton)
				assert(empty);
			else if (!empty)
				assert(biton);
		}
		
		for (p = last(b); p != b; p = p->bk) {
			do_check_free_chunk(p);
			size = chunksize(p);
			total += size;
			if (i >= 2) {
				idx = bin_index(size);
				assert(idx == (unsigned)i);
				assert(p->bk == b || 
				       chunksize(p->bk) >= chunksize(p));
			}
			for (q = next_chunk(p);
			     q != av->top && 
				     inuse(q) && 
				     (long)(chunksize(q)) >= (long)MINSIZE;
			     q = next_chunk(q))
				do_check_inuse_chunk(q);

    }
  }

  check_chunk(av->top);
  assert(total <= (unsigned long)(av->max_total_mem));
  assert((unsigned long)(av->sbrked_mem) <=
         (unsigned long)(av->max_sbrked_mem));
}

#endif




/* ----------- Routines dealing with system allocation -------------- */

/* This routine is much simpler (after RSCHS' changes) than the original 
 * version it copys the content of the shared memory segment to a shared
 * memory
*/
Void_t* SharedData::sysmalloc(INTERNAL_SIZE_T nb)
{
	mstate av = get_malloc_state(); /* av MUST be initialized on entry */
	
	long            size;           /* arg to first MORECORE call */
	long            correction;     /* arg to 2nd call */
	
	INTERNAL_SIZE_T front_misalign; /* unusable bytes at front of 
					 * new space */
	INTERNAL_SIZE_T end_misalign;   /* partial page left at end 
					 * of new space */
	
	char*           brk;            /* return value from MORECORE */
	char*           aligned_brk;    /* aligned offset into brk */
	char*           snd_brk;        /* 2nd return val */
	
	mchunkptr       p;              /* the allocated/returned chunk */
	mchunkptr       remainder;      /* remainder from allocation */
	long            remainder_size; /* its size */
	
	unsigned long   sum;            /* for updating stats */
	
	mchunkptr       old_top;        /* incoming value of av->top */
	INTERNAL_SIZE_T old_size;       /* its size */
	char*           old_end;        /* its end address */

	size_t          pagemask = av->pagesize - 1;
	
	/* record incoming configuration of top */

	old_top  = av->top;
	old_size = chunksize(old_top);
	old_end  = (char*)(chunk_at_offset(old_top, old_size));
	
	brk = snd_brk = (char*)(MORECORE_FAILURE); 
	
	/* 
	   If not the first time through, we require old_size to be
	   at least MINSIZE and to have prev_inuse set.
	*/
	
	assert(old_top == initial_top(av) || 
	       ((unsigned long) (old_size) >= (unsigned long)(MINSIZE) &&
		prev_inuse(old_top)));
	

	/* Request enough space for nb + pad + overhead */

	size = nb + av->top_pad + MINSIZE;

	/*
	  If contiguous, we can subtract out existing space that we hope to
	  combine with new space. We add it back later in only if
	  we don't actually get contiguous space.
	*/

	if (av->sbrk_base != NONCONTIGUOUS_REGIONS)
		size -= old_size;

	/*
	  Round to a multiple of page size.
	  If MORECORE is not contiguous, this ensures that we only call it
	  with whole-page arguments.  And if MORECORE is contiguous and
	  this is not first time through, this preserves page-alignment of
	  previous calls. Otherwise, we re-correct anyway to page-align below.
	*/

	size = (size + pagemask) & ~pagemask;
	
	/*
	  Don't try to call MORECORE if argument is so big as to appear
	  negative. 
	*/

	if (size > 0) 
		brk = (char*)(MORECORE(size));


	if (brk != (char*)(MORECORE_FAILURE)) {
		av->sbrked_mem += size;

		/*
		  If MORECORE extends previous space, we can likewise 
		  extend top size.
		*/
    
		if (brk == old_end && snd_brk == (char*)(MORECORE_FAILURE)) {
			set_head(old_top, (size + old_size) | PREV_INUSE);
		}
    
		/*
		  Otherwise, make adjustments guided by the special values of 
		  av->sbrk_base (MORECORE_FAILURE or NONCONTIGUOUS_REGIONS):
		  
		  * If the first time through or noncontiguous, we need to 
		  call sbrk just to find out where the end of memory lies.

		  * We need to ensure that all returned chunks from malloc 
		  will meet MALLOC_ALIGNMENT
		  
		  * If there was an intervening foreign sbrk, we need to 
		  adjust sbrk request size to account for fact that we will
		  not be able to combine new space with existing space in
		  old_top.
		  
		  * Almost all systems internally allocate whole pages at a
		  time, in which case we might as well use the whole last
		  page of request. So we allocate enough more memory to hit
		  a page boundary now, which in turn causes future contiguous
		  calls to page-align.

		*/
    
		else {
			front_misalign = 0;
			end_misalign = 0;
			correction = 0;
			aligned_brk = brk;
			
			/* handle contiguous cases */
			if (av->sbrk_base != NONCONTIGUOUS_REGIONS) { 
				
				/* Guarantee alignment of first new 
				   chunk made from this space */
				
				front_misalign = (INTERNAL_SIZE_T)
					chunk2mem(brk) & MALLOC_ALIGN_MASK;
				if (front_misalign > 0) {

					/*
					  Skip over some bytes to
					  arrive at an aligned
					  position.  We don't need to
					  specially mark these wasted
					  front bytes.  They will
					  never be accessed anyway
					  because prev_inuse of
					  av->top (and any chunk
					  created from its start) is
					  always true after
					  initialization. 
					*/

					correction = MALLOC_ALIGNMENT
						- front_misalign;
					aligned_brk += correction;
				}
				
				/*
				  If this isn't adjacent to a previous
				  sbrk, then we will not be able to
				  merge with old_top space, so must
				  add to 2nd request. 
				*/
        
				correction += old_size;
        
				/* Pad out to hit a page boundary */

				end_misalign = (INTERNAL_SIZE_T)(brk + 
								 size + 
								 correction);
				correction += ((end_misalign + pagemask)
					       & ~pagemask) - end_misalign;
        
				assert(correction >= 0);
        
				snd_brk = (char*)(MORECORE(correction));
        
				/*
				  If can't allocate correction, try to
				  at least find out current brk.  It
				  might be enough to proceed without
				  failing.
				  
				  Note that if second sbrk did NOT
				  fail, we assume that space is
				  contiguous with first sbrk. This is
				  a safe assumption unless program is
				  multithreaded but doesn't use locks
				  and a foreign sbrk occurred between
				  our first and second calls.  */
        
				if (snd_brk == (char*)(MORECORE_FAILURE)) {
					correction = 0;
					snd_brk = (char*)(MORECORE(0));
				}
			}
      
			/* handle non-contiguous cases */
			else { 
				
				/* MORECORE must correctly align etc */
				assert(((unsigned long)chunk2mem(brk) 
					& MALLOC_ALIGN_MASK) == 0);
        
				/* Find out current end of memory */
				if (snd_brk == (char*)(MORECORE_FAILURE)) {
					snd_brk = (char*)(MORECORE(0));
				}
        
				/* This must lie on a page boundary */
				if (snd_brk != (char*)(MORECORE_FAILURE)) {
					assert(((INTERNAL_SIZE_T)(snd_brk) 
						& pagemask) == 0);
				}
			}
      
			if (snd_brk != (char*)(MORECORE_FAILURE)) {
       
				av->top = (mchunkptr)aligned_brk;
				set_head(av->top, 
					 (snd_brk - aligned_brk + 
					  correction) | PREV_INUSE);
        
				av->sbrked_mem += correction;
        
				/* If first time through and
                                   contiguous, record base */
				if (old_top == initial_top(av)) {
					if (av->sbrk_base == (char*)
					    (MORECORE_FAILURE)) 
						av->sbrk_base = brk;
				}
        
				/*
				  Otherwise, we either have a gap or a
				  non-contiguous region.  Insert a
				  double fencepost at old_top to
				  prevent consolidation with space we
				  don't own. These fenceposts are
				  artificial chunks that are marked as
				  inuse and are in any case too small
				  to use.  We need two to make sizes
				  and alignments work out.  */

				else {
          
					/* 
					   Shrink old_top to insert
					   fenceposts, keeping size a
					   multiple of
					   MALLOC_ALIGNMENT.  */
					old_size = (old_size - 3*SIZE_SZ) 
						& ~MALLOC_ALIGN_MASK;
					set_head(old_top, 
						 old_size | PREV_INUSE);
          
					/*
					  Note that the following
					  assignments overwrite
					  old_top when old_size was
					  previously MINSIZE.  This is
					  intentional. We need the
					  fencepost, even if old_top
					  otherwise gets lost.  */
					chunk_at_offset(old_top, 
							old_size)
						->size = SIZE_SZ|PREV_INUSE;

					chunk_at_offset(old_top, 
							old_size+SIZE_SZ)
						->size = SIZE_SZ|PREV_INUSE;
          
					/* If possible, release the rest. */
					if (old_size >= MINSIZE) 
						free(chunk2mem(old_top));
					
				}
			}
		}
    
		/* Update statistics */
		
		sum = av->sbrked_mem;
		if (sum > (unsigned long)(av->max_sbrked_mem))
			av->max_sbrked_mem = sum;
		
		if (sum > (unsigned long)(av->max_total_mem))
			av->max_total_mem = sum;
		
		check_malloc_state();
		
		/* finally, do the allocation */
		
		p = av->top;
		size = chunksize(p);
		remainder_size = (long)size - (long)nb;
		
		/* check that one of the above allocation paths succeeded */
		if (remainder_size >= (long)MINSIZE) {
			remainder = chunk_at_offset(p, nb);
			av->top = remainder;
			set_head(p, nb | PREV_INUSE);
			set_head(remainder, remainder_size | PREV_INUSE);
			
			check_malloced_chunk(p, nb);
			return chunk2mem(p);
		}
	}

	/* catch all failure paths */
	MALLOC_FAILURE_ACTION;
	return 0;
}

int SharedData::mtrim(size_t pad)
{
	mstate av = get_malloc_state();
	
	long  top_size;        /* Amount of top-most memory */
	long  extra;           /* Amount to release */
	long  released;        /* Amount actually released */
	char* current_brk;     /* address returned by pre-check sbrk call */
	char* new_brk;         /* address returned by post-check sbrk call */
	size_t pagesz;
	
	/* Ensure initialization/consolidation */
	if (need_consolidation(av) || av->fastbytes != 0)
		malloc_consolidate();

	/* Don't bother trying if sbrk doesn't provide contiguous regions */
	if (av->sbrk_base == NONCONTIGUOUS_REGIONS) return 0;
	
	pagesz = av->pagesize;
	top_size = chunksize(av->top);
	
	/* Release in pagesize units, keeping at least one page */
	extra = ((top_size - pad - MINSIZE + (pagesz-1)) 
		 / pagesz - 1) * pagesz;

	if (extra > 0) {
		
		/*
		  Only proceed if end of memory is where we last set it.
		  This avoids problems if there were foreign sbrk calls.
		*/
		current_brk = (char*)(MORECORE(0));
		if (current_brk == (char*)(av->top) + top_size) {
			
			/*
			  Attempt to release memory. We ignore return
			  value, and instead call again to find out
			  where new end of memory is.  This avoids
			  problems if first call releases less than we
			  asked, of if failure somehow altered brk
			  value. (We could still encounter problems if
			  it altered brk in some very bad way, but the
			  only thing we can do is adjust anyway, which
			  will cause some downstream failure.)  */

			MORECORE(-extra);
			new_brk = (char*)(MORECORE(0));

			if (new_brk != (char*)MORECORE_FAILURE) {
				released = (long)(current_brk - new_brk);
				if (released != 0) {
					/* Adjust top. */
					av->sbrked_mem -= released;
					set_head(av->top, 
						 (top_size - released) 
						 | PREV_INUSE);
					check_malloc_state();
					return 1;
				}
			}
		}
	}
	
	return 0;
}

/* ----------------------- Main public routines ----------------------- */

Void_t* SharedData::xmalloc(size_t bytes)
{
	errno=0;
	register Void_t *val=malloc(bytes);

	if(val==NULL)
		fatal("SharedData::malloc cannot get the"
		      " requested %d bytes\n",
		      bytes);
	return val;
}

Void_t* SharedData::malloc(size_t bytes)
{
  mstate av = get_malloc_state();

  INTERNAL_SIZE_T nb;               /* normalized request size */
  unsigned int    idx = 0;          /* associated bin index */
  mbinptr         bin;              /* associated bin */
  mfastbinptr     fb;               /* associated fastbin */

  mchunkptr       victim;           /* inspected/selected chunk */
  INTERNAL_SIZE_T size;             /* its size */
  int             victim_index;     /* its bin index */

  mchunkptr       remainder;        /* remainder from a split */
  long            remainder_size;   /* its size */

  unsigned int    block;            /* bit map traverser */
  unsigned int    bit;              /* bit map traverser */
  unsigned int    map;              /* current word of binmap */

  mchunkptr       fwd;              /* misc temp for linking */
  mchunkptr       bck;              /* misc temp for linking */


  /*
    Check request for legality and convert to internal form, nb.
    This rejects negative arguments when size_t is treated as
    signed. It also rejects arguments that are so large that the size
    appears negative when aligned and padded.  The converted form
    adds SIZE_T bytes overhead plus possibly more to obtain necessary
    alignment and/or to obtain a size of at least MINSIZE, the
    smallest allocatable size.
  */

  checked_request2size(bytes, nb);

  /*
    If the size qualifies as a fastbin, check corresponding bin.
    (Note that this code is safe to execute even if av not yet initialized
    so saves some time on this fast path.)
  */

  if (nb <= av->max_fast) {
    fb = &(av->fastbins[(fastbin_index(nb))]);
    if ( (victim = fb->hd) != 0) {
      fwd = victim->fd;
      av->fastbytes -= nb;
      fb->hd = fwd;
      if (fwd == 0) fb->tl = 0;

      check_remalloced_chunk(victim, nb);
      return chunk2mem(victim);
    }
  }

  /*
    Before continuing, consolidate fastbins if it looks like their
    existence could degrade fits. This is done if:

    * The current request is >= MIN_LARGE_SIZE (512 bytes)

    * A large chunk was freed since last consolidation

    * av has not yet been intitialized.

    * There was at some point space in fastbins, but a request could
      not be satisfied without splitting space in much larger bins or
      top.

    The rules try to adapt to different (sometimes very transient)
    modes of use. The main idea is to kill off fastbins as soon as
    they look problematic -- early enough so that it is still cheap
    (especially wrt locality) and effective to do so, but to still
    give them a chance to improve performance when in a mode where
    many small chunks are being continually allocated and freed. (512
    is used as a cutoff because it is unusually big for a small chunk,
    but unusually small for a big one.)

  */

  if (need_consolidation(av) ||
      (!in_smallbin_range(nb) && av->fastbytes != 0))
    malloc_consolidate();

  /*
    If a small request, check regular bin.  Since these "smallbins"
    hold one size each, no searching is necessary.
    
    (If a large request, we need to wait until unsorted chunks are
    processed to find best fit. But for small ones, fits are exact
    anyway, so we can check now, which is faster.)

  */

  if (in_smallbin_range(nb)) {
    idx = smallbin_index(nb);
    bin = bin_at(av,idx);
    if ( (victim = last(bin)) != bin) {
      bck = victim->bk;
      set_inuse_bit_at_offset(victim, nb);
      bin->bk = bck;
      bck->fd = bin;

      check_malloced_chunk(victim, nb);
      return chunk2mem(victim);
    }
  }

  /*
    Scan through recently freed or remaindered chunks, placing
    unusable ones in bins.  Note that this step is the only place in
    any routine where chunks are placed in bins.
    
    For small requests, use chunk if an exact fit or if the only chunk
    left is the one that was last used to satisfy a previous small
    request.  This helps preserve locality and improve speed in the
    absence of exact fits for small requests, and is often the best
    fitting chunk anyway.
    
    For large requests, ignore even exact fits for now,
    since a better (less recently used) choice may be in nb's bin. 
 */

  while ( (victim = unsorted_chunks(av)->bk) != unsorted_chunks(av)) {
    bck = victim->bk;
    size = chunksize(victim);

    /* Try to use last remainder if it is the only chunk left in bin. */

    if (in_smallbin_range(nb) &&
        bck == unsorted_chunks(av) &&
        victim == av->last_remainder &&
        (remainder_size = (long)size - (long)nb) >= (long)MINSIZE) {

      remainder = chunk_at_offset(victim, nb);

      /* reattach remainder */
      unsorted_chunks(av)->bk = unsorted_chunks(av)->fd = remainder;
      av->last_remainder = remainder;
      remainder->bk = remainder->fd = unsorted_chunks(av);

      set_head(victim, nb | PREV_INUSE);
      set_head(remainder, remainder_size | PREV_INUSE);
      set_foot(remainder, remainder_size);

      check_malloced_chunk(victim, nb);
      return chunk2mem(victim);
    }

    unsorted_chunks(av)->bk = bck;
    bck->fd = unsorted_chunks(av);

    /* place chunk in bin */

    if (in_smallbin_range(size)) {

      /* Take now instead of binning if small and exact fit */
      if (size == nb) {
        set_inuse_bit_at_offset(victim, size);
        check_malloced_chunk(victim, nb);
        return chunk2mem(victim);
      }

      victim_index = smallbin_index(size);
      bck = bin_at(av, victim_index);
      fwd = bck->fd;
    }
    else {
      victim_index = largebin_index(size);
      bck = bin_at(av, victim_index);
      fwd = bck->fd;

      /* maintain large bins in sorted order */
      if (fwd != bck) {
        while (fwd != bck && 
               (unsigned long)size < (unsigned long)(chunksize(fwd))) 
          fwd = fwd->fd;
        bck = fwd->bk;
      }

    }

    mark_bin(av, victim_index);
    victim->bk = bck;
    victim->fd = fwd;
    fwd->bk = victim;
    bck->fd = victim;
  }

  /*
    If a large request, scan through the chunks of current bin in
    sorted order to find smallest that fits.  This is the only step
    where an unbounded number of chunks might be scanned without doing
    anything useful with them. However the lists tend to be very
    short.
  */

  if (!in_smallbin_range(nb)) {
    idx = largebin_index(nb);
    bin = bin_at(av, idx);

    for (victim = last(bin); victim != bin; victim = victim->bk) {
      size = chunksize(victim);
      remainder_size = (long)size - (long)nb;

      if (remainder_size >= 0)  {
        unlink(victim, bck, fwd);

        /* Exhaust */
        if (remainder_size < (long)MINSIZE)  {
          set_inuse_bit_at_offset(victim, size);
          check_malloced_chunk(victim, nb);
          return chunk2mem(victim);
        }
        /* Split */
        else {
          remainder = chunk_at_offset(victim, nb);
          unsorted_chunks(av)->bk = unsorted_chunks(av)->fd = remainder;
          remainder->bk = remainder->fd = unsorted_chunks(av);
          set_head(victim, nb | PREV_INUSE);
          set_head(remainder, remainder_size | PREV_INUSE);
          set_foot(remainder, remainder_size);
          check_malloced_chunk(victim, nb);
          return chunk2mem(victim);
        }
      }
    }
  }

  /*
    Search for a chunk by scanning bins, starting with next largest
    bin. This search is strictly by best-fit; i.e., the smallest
    (with ties going to approximately the least recently used) chunk
    that fits is selected.
    
    The bitmap avoids needing to check that most blocks are nonempty.
    The particular case of skipping all bins during warm-up phases
    when no chunks have been returned yet is faster than it might look.

  */

  ++idx;
  bin = bin_at(av,idx);
  block = idx2block(idx);
  map = av->binmap[block];
  bit = idx2bit(idx);

  for (;;) {
    /*
      Skip rest of block if there are no more set bits in this block.
    */

    if (bit > map || bit == 0) {
      for (;;) {
        if (++block >= BINMAPSIZE) /* out of bins */
          break;

        else if ( (map = av->binmap[block]) != 0) {
          bin = bin_at(av, (block << BINMAPSHIFT));
          bit = 1;
          break;
        }
      }
      /* Optimizers seem to like this double-break better than goto */
      if (block >= BINMAPSIZE)
          break;
    }

    /* Advance to bin with set bit. There must be one. */
    while ((bit & map) == 0) {
      bin = next_bin(bin);
      bit <<= 1;
    }

    victim = last(bin);

    /*  False alarm -- the bin is empty. Clear the bit. */
    if (victim == bin) {
      av->binmap[block] = map &= ~bit; /* Write through */
      bin = next_bin(bin);
      bit <<= 1;
    }

    /*  We know the first chunk in this bin is big enough to use. */
    else {
      size = chunksize(victim);
      remainder_size = (long)size - (long)nb;

      assert(remainder_size >= 0);

      /* unlink */
      bck = victim->bk;
      bin->bk = bck;
      bck->fd = bin;


      /* Exhaust */
      if (remainder_size < (long)MINSIZE) {
        set_inuse_bit_at_offset(victim, size);
        check_malloced_chunk(victim, nb);
        return chunk2mem(victim);
      }

      /* Split */
      else {
        remainder = chunk_at_offset(victim, nb);

        unsorted_chunks(av)->bk = unsorted_chunks(av)->fd = remainder;
        remainder->bk = remainder->fd = unsorted_chunks(av);
        if (in_smallbin_range(nb)) {

          /* advertise remainder as last_remainder */
          av->last_remainder = remainder;

          /*
            If we are splitting a large free chunk to satisfy a small
            request but there are lots of fastbin chunks, arrange to
            consolidate on next malloc. It isn't worth consolidating
            and restarting current malloc since this is just a
            safeguard against buildup of fastbin chunks that never
            become requested.
          */

          if (remainder_size >= MIN_LARGE_SIZE  &&
              av->fastbytes  >= FASTBIN_CONSOLIDATION_THRESHOLD)
            set_consolidation_flag(av);
        }


        set_head(victim, nb | PREV_INUSE);
        set_head(remainder, remainder_size | PREV_INUSE);
        set_foot(remainder, remainder_size);
        check_malloced_chunk(victim, nb);
        return chunk2mem(victim);
      }
    }
  }

  /*
    If large enough, split off the chunk bordering the end of memory
    (`top'). Note that this use of top is in accord with the best-fit search
    rule.  In effect, top is treated as larger (and thus less well
    fitting) than any other available chunk since it can be extended
    to be as large as necessary (up to system limitations).
  */

  victim = av->top;
  size = chunksize(victim);
  remainder_size = (long)size - (long)nb;

  /* top must always have a remainder, so it always exists after split */
  if (remainder_size >= (long)MINSIZE) {
    remainder = chunk_at_offset(victim, nb);
    av->top = remainder;
    set_head(victim, nb | PREV_INUSE);
    set_head(remainder, remainder_size | PREV_INUSE);
    
    check_malloced_chunk(victim, nb);
    return chunk2mem(victim);
  }

  /*
    If fastbins exist, retry after consolidation to avoid expansion.
  */
  else if (av->fastbytes != 0) {
    set_consolidation_flag(av);
    return malloc(nb - MALLOC_ALIGN_MASK);
  }

  /* get more memory from system */
  else 
    return sysmalloc(nb);

}



void SharedData::free(Void_t* mem)
{
  mstate av = get_malloc_state();

  mchunkptr       p;           /* chunk corresponding to mem */
  INTERNAL_SIZE_T size;        /* its size */
  mfastbinptr     fb;          /* associated fastbin */
  mchunkptr       tail;        /* tail of bin */
  mchunkptr       nextchunk;   /* next contiguous chunk */
  INTERNAL_SIZE_T nextsize;    /* its size */
  int             nextinuse;   /* true if nextchunk is used */
  INTERNAL_SIZE_T prevsize;    /* size of previous contiguous chunk */
  mchunkptr       bck;         /* misc temp for linking */
  mchunkptr       fwd;         /* misc temp for linking */


  /* free(0) has no effect */
  if (mem != 0) {

    p = mem2chunk(mem);
    check_inuse_chunk(p);

    size = chunksize(p);

    /*
      If eligible, place chunk on a fastbin
    */

    if ((unsigned long)size <= (unsigned long)av->max_fast) {

      fb = &(av->fastbins[fastbin_index(size)]);
      tail = fb->tl;
      av->fastbytes += size;
      p->fd = 0;

      fb->tl = p;
      if (tail == 0)
        fb->hd = p;
      else
        tail->fd = p;
    }

    /*
       Consolidate chunks as they arrive.
    */

    else {

      /*
        Possibly schedule to consolidate fastbins on next malloc. The
        threshold compares against the sum of current size plus
        fastbin bytes, which means to arrange consolidation if
        freeing a large chunk, and/or if there are a lot of fastbin
        chunks but we are now freeing a non-fast one).
      */

      if (!need_consolidation(av) &&
          (unsigned long)(size + av->fastbytes) >=
          (unsigned long)FASTBIN_CONSOLIDATION_THRESHOLD)
          set_consolidation_flag(av);

      nextchunk = chunk_at_offset(p, size);

      /* consolidate backward */
      if (!prev_inuse(p)) {
        prevsize = p->prev_size;
        size += prevsize;
        p = chunk_at_offset(p, -((long) prevsize));
        unlink(p, bck, fwd);
      }

      nextsize = chunksize(nextchunk);

      if (nextchunk != av->top) {

        /* get and clear inuse bit */
        nextinuse = inuse_bit_at_offset(nextchunk, nextsize);
        set_head(nextchunk, nextsize);

        /* consolidate forward */
        if (!nextinuse) {
          unlink(nextchunk, bck, fwd);
          size += nextsize;
        }

        /*
          Place chunk in unsorted chunk list. Chunks are
          not placed into regular bins until after they have
          been given one chance to be used in malloc.
        */

        bck = unsorted_chunks(av);
        fwd = bck->fd;
        p->bk = bck;
        p->fd = fwd;
        bck->fd = p;
        fwd->bk = p;

        set_head(p, size | PREV_INUSE);
        set_foot(p, size);

      }

      /*
         If the chunk borders the current high end of memory,
         consolidate into top
      */

      else {

        size += nextsize;
        set_head(p, size | PREV_INUSE);
        av->top = p;

        /*
          If the total unused topmost memory exceeds trim
          threshold, ask malloc_trim to reduce top. Note that
          the comparison doesn't (can't) take into account
          fastbin chunks that may be bordering top, so may be
          an underestimate. malloc_trim will consolidate before
          trying to trim, so these chunks will be released. But
          it would be self-defeating to consolidate fastbins
          on each free just to see if threshold is reached.
        */

        if ((unsigned long)(size) > (unsigned long)(av->trim_threshold))
          mtrim(av->top_pad);

      }

    }
  }
}



/*
  malloc_consolidate is a specialized version of free() that tears
  down chunks held in fastbins.  Free itself cannot be used for this
  purpose since, among other things, it might place chunks back onto
  fastbins.  So, instead, we need to use a minor variant of the same
  code.
  
  Also, because this routine needs to be called the first time through
  malloc anyway, it turns out to be the perfect place to bury
  initialization code.
*/

void SharedData::malloc_consolidate()
{
  mstate av = get_malloc_state();
  mfastbinptr     fb;
  mchunkptr       p;
  mchunkptr       nextp;
  mchunkptr       uns;
  mchunkptr       ufd;

  /* These have same use as in free() */
  mchunkptr       nextchunk;
  INTERNAL_SIZE_T size;
  INTERNAL_SIZE_T nextsize;
  INTERNAL_SIZE_T prevsize;
  int             nextinuse;
  mchunkptr       bck;
  mchunkptr       fwd;

  /*
    If top bin isn't set up, we know that malloc hasn't
    yet been initialized, in which case do so.
  */

  if (av->top == 0) {
    malloc_init_state(av);
    check_malloc_state();
    return;
  }

  /* Clear flags */
  clear_consolidation_flag(av);

  if (av->fastbytes == 0) return;

  av->fastbytes = 0;

  uns = unsorted_chunks(av);

  /*
    Remove each chunk from fast bin and consolidate it, placing
    it then in unsorted bin. Placing in unsorted bin avoids
    needing to calculate actual bins until malloc is sure that chunks
    aren't immediately going to be reused anyway.
    
    Running through bins from largest to smallest seems to work a
    little better in practice than the other way around.
  */

  
  fb = &(av->fastbins[fastbin_index(av->max_fast)]);
  do {
    p = fb->hd;

    if (p != 0) {
      fb->hd = fb->tl = 0;

      do {
        check_inuse_chunk(p);
        nextp = p->fd;

        /* Slightly streamlined version of consolidation code in free() */
        size = p->size & ~PREV_INUSE;
        nextchunk = chunk_at_offset(p, size);

        if (!prev_inuse(p)) {
          prevsize = p->prev_size;
          size += prevsize;
          p = chunk_at_offset(p, -((long) prevsize));
          unlink(p, bck, fwd);
        }

        nextsize = chunksize(nextchunk);

        if (nextchunk != av->top) {

          nextinuse = inuse_bit_at_offset(nextchunk, nextsize);
          set_head(nextchunk, nextsize);

          if (!nextinuse) {
            size += nextsize;
            unlink(nextchunk, bck, fwd);
          }

          ufd = uns->fd;
          uns->fd = p;
          ufd->bk = p;

          set_head(p, size | PREV_INUSE);
          p->bk = uns;
          p->fd = ufd;
          set_foot(p, size);

        }

        else {
          size += nextsize;
          set_head(p, size | PREV_INUSE);
          av->top = p;
        }

      } while ( (p = nextp) != 0);

    }
  } while (fb-- != &(av->fastbins[0]));
}




/*
  Realloc algorithm cases:

  * Otherwise, if the reallocation is for additional space, and the
    chunk can be extended, it is, else a malloc-copy-free sequence is
    taken.  There are several different ways that a chunk could be
    extended. All are tried:

       * Extending forward into following adjacent free chunk.
       * Shifting backwards, joining preceding adjacent space
       * Both shifting backwards and extending forward.
       * Extending into newly sbrked space

  * If there is not enough memory available to realloc, realloc
    returns null, but does NOT free the existing space.

  * If the reallocation is for less space, the newly unused space is
    lopped off and freed. Unless the #define REALLOC_ZERO_BYTES_FREES
    is set, realloc with a size argument of zero (re)allocates a
    minimum-sized chunk.


  The old unix realloc convention of allowing the last-free'd chunk
  to be used as an argument to realloc is no longer supported.
  I don't know of any programs still relying on this feature,
  and allowing it would also allow too many other incorrect
  usages of realloc to be sensible.
*/


Void_t* SharedData::xrealloc(Void_t* oldmem, size_t bytes)
{
	errno=0;
	register Void_t *val=realloc(oldmem, bytes);

	if(val==NULL)
		fatal("SharedData::realloc cannot get"
		      " the requested %d bytes\n",
		      bytes);
	return val;
}

Void_t* SharedData::realloc(Void_t* oldmem, size_t bytes)
{
  mstate av = get_malloc_state();

  INTERNAL_SIZE_T  nb;              /* padded request size */

  mchunkptr        oldp;            /* chunk corresponding to oldmem */
  INTERNAL_SIZE_T  oldsize;         /* its size */

  mchunkptr        newp;            /* chunk to return */
  INTERNAL_SIZE_T  newsize;         /* its size */
  Void_t*          newmem;          /* corresponding user mem */

  mchunkptr        next;            /* next contiguous chunk after oldp */
//  mchunkptr        prev;            /* previous contiguous chunk before oldp */

  mchunkptr        remainder;       /* extra space at end of newp */
  INTERNAL_SIZE_T  remainder_size;  /* its size */

  mchunkptr        bck;             /* misc temp for linking */
  mchunkptr        fwd;             /* misc temp for linking */

  INTERNAL_SIZE_T  copysize;        /* bytes to copy */
  int              ncopies;         /* INTERNAL_SIZE_T words to copy */
  INTERNAL_SIZE_T* s;               /* copy source */ 
  INTERNAL_SIZE_T* d;               /* copy destination */

  int disposal_after_copy;          /* nonzero if shouldn't split after copy */

#ifdef REALLOC_ZERO_BYTES_FREES
  if (bytes == 0) {
    fREe(oldmem);
    return 0;
  }
#endif

  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return malloc(bytes);

  checked_request2size(bytes, nb);

  /*
    If a large request, clobber fastbins first
  */

  if (!in_smallbin_range(nb) && av->fastbytes != 0)
    malloc_consolidate();


  oldp    = mem2chunk(oldmem);
  oldsize = chunksize(oldp);

  check_inuse_chunk(oldp);

    if ((unsigned long)(oldsize) >= (unsigned long)(nb)) {
      /* already big enough; split below */
      newp = oldp;
      newsize = oldsize;
    }

    else {
      newp = 0;
      newsize = 0;
      disposal_after_copy = 0;

      next = chunk_at_offset(oldp, oldsize);

      if (next == av->top) {            /* Expand forward into top */
        newsize = oldsize + chunksize(next);

        if ((unsigned long)(newsize) >= (unsigned long)(nb + MINSIZE)) {
          set_head_size(oldp, nb);
          av->top = chunk_at_offset(oldp, nb);
          set_head(av->top, (newsize - nb) | PREV_INUSE);
          return chunk2mem(oldp);
        }

//          else if (!prev_inuse(oldp)) {   /* Shift backwards + top */
//            prev = prev_chunk(oldp);
//            newsize += chunksize(prev);

//            if ((unsigned long)(newsize) >= (unsigned long)(nb + MINSIZE)) {
//              newp = prev;
//              unlink(prev, bck, fwd);
//              set_head_size(newp, nb);
//              av->top = chunk_at_offset(newp, nb);
//              set_head(av->top, (newsize - nb) | PREV_INUSE);
//              disposal_after_copy = 2; /* signal not to trim remainder below */
//            }
//          }

      }

      else if (!inuse(next)) {          /* Forward into next chunk */
        newsize = oldsize + chunksize(next);
        
        if (((unsigned long)(newsize) >= (unsigned long)(nb))) {
          newp = oldp;
          unlink(next, bck, fwd);
        }
        
//          else if (!prev_inuse(oldp)) {   /* Forward + backward */
//            prev = prev_chunk(oldp);
//            newsize += chunksize(prev);
          
//            if (((unsigned long)(newsize) >= (unsigned long)(nb))) {
//              newp = prev;
//              unlink(prev, bck, fwd);
//              unlink(next, bck, fwd);
//            }
//          }
      }
      
//        else if (!prev_inuse(oldp)) {     /* Backward only */
//          prev = prev_chunk(oldp);
//          newsize = oldsize + chunksize(prev);
        
//          if ((unsigned long)(newsize) >= (unsigned long)(nb)) {
//            newp = prev;
//            unlink(prev, bck, fwd);
//          }
//        }

      if (newp == 0) {                  /* Must allocate */
        newmem = malloc(nb - MALLOC_ALIGN_MASK);
        if (newmem == 0)
          return 0; /* propagate failure */

        newp = mem2chunk(newmem);
        newsize = chunksize(newp);

        /*
          Avoid copy if newp is next chunk after oldp.
        */
        if (newp == next) {
          newsize += oldsize;
          newp = oldp;
        }
        else {
          disposal_after_copy = 1; /* signal to free below */
        }
      }

      /* copy if necessary */

      if (newp != oldp) {

        /*
          Unroll copy of <= 36 bytes (72 if 8byte sizes)
          We know that contents have an odd number of
          INTERNAL_SIZE_T-sized words; minimally 3.
        */

        copysize = oldsize - SIZE_SZ;
        s = (INTERNAL_SIZE_T*)oldmem;
        d = (INTERNAL_SIZE_T*)(chunk2mem(newp));
        ncopies = copysize / sizeof(INTERNAL_SIZE_T);
        assert(ncopies >= 3);

        if (ncopies > 9)
           MALLOC_COPY(d, s, copysize);

        else {
          *(d+0) = *(s+0);
          *(d+1) = *(s+1);
          *(d+2) = *(s+2);
          if (ncopies > 4) {
            *(d+3) = *(s+3);
            *(d+4) = *(s+4);
            if (ncopies > 6) {
              *(d+5) = *(s+5);
              *(d+6) = *(s+6);
              if (ncopies > 8) {
                *(d+7) = *(s+7);
                *(d+8) = *(s+8);
              }
            }
          }
        }

        /* was set to 1 above if need to free after copy */
        if (disposal_after_copy == 1) {
          free(oldmem);
        }

        /* was set nonzero above if should not split after copy */
        if (disposal_after_copy != 0) {
          check_inuse_chunk(newp);
          return chunk2mem(newp);
        }
      }

    }

    assert((long)newsize >= (long)nb);

    /* If possible, free extra space in old or extended chunk */

    if ((long)newsize - (long)nb >= (long)MINSIZE) { /* split remainder */
      remainder = chunk_at_offset(newp, nb);
      remainder_size = (long)newsize - (long)nb;
      set_head_size(newp, nb);
      set_head(remainder, remainder_size | PREV_INUSE);
      set_inuse_bit_at_offset(remainder, remainder_size);
      free(chunk2mem(remainder)); /* let free() deal with it */
    }

    else { /* not enough extra to split off */
      set_head_size(newp, newsize);
      set_inuse_bit_at_offset(newp, newsize);
    }

    check_inuse_chunk(newp);
    return chunk2mem(newp);

}




Void_t* SharedData::calloc(size_t n_elements, size_t elem_size)
{
  mchunkptr p;
  INTERNAL_SIZE_T clearsize;
  int nclears;
  INTERNAL_SIZE_T* d;

  Void_t* mem = malloc(n_elements * elem_size);

  if (mem != 0) {
    p = mem2chunk(mem);

      d = (INTERNAL_SIZE_T*)mem;
      clearsize = chunksize(p) - SIZE_SZ;
      nclears = clearsize / sizeof(INTERNAL_SIZE_T);
      assert(nclears >= 3);

      if (nclears > 9)
        MALLOC_ZERO(d, clearsize);

      else {
        *(d+0) = 0;
        *(d+1) = 0;
        *(d+2) = 0;
        if (nclears > 4) {
          *(d+3) = 0;
          *(d+4) = 0;
          if (nclears > 6) {
            *(d+5) = 0;
            *(d+6) = 0;
            if (nclears > 8) {
              *(d+7) = 0;
              *(d+8) = 0;
            }
          }
        }
      }
    }
  return mem;
}



size_t SharedData::musable(Void_t* mem)
{
  mchunkptr p;
  if (mem != 0) {
    p = mem2chunk(mem);
    if (inuse(p))
      return chunksize(p) - SIZE_SZ;
  }
  return 0;
}




struct mallinfo SharedData::mallinfo()
{
  mstate av = get_malloc_state();
  struct mallinfo mi;
  int i;
  mbinptr b;
  mchunkptr p;
  INTERNAL_SIZE_T avail;
  int navail;
  int nfastblocks;

  /* Ensure initialization */
  if (av->top == 0)  malloc_consolidate();

  check_malloc_state();

  /* Account for top */
  navail = 1;  /* top always exists */
  avail = chunksize(av->top);

  /* traverse fastbins */
  avail += av->fastbytes;
  nfastblocks = 0;

  for (i = fastbin_index(av->max_fast); i >= 0; --i) {
    for (p = (av->fastbins[i]).hd; p != 0; p = p->fd) {
      ++nfastblocks;
    }
  }

  /* traverse regular bins */
  for (i = 1; i < NBINS; ++i) {
    b = bin_at(av, i);
    for (p = last(b); p != b; p = p->bk) {
      avail += chunksize(p);
      navail++;
    }
  }

  mi.smblks = nfastblocks;
  mi.ordblks = navail;
  mi.fordblks = avail;
  mi.uordblks = av->sbrked_mem - avail;
  mi.arena = av->sbrked_mem;
  mi.hblks = 0;
  mi.hblkhd = 0;
  mi.fsmblks = av->fastbytes;
  mi.keepcost = chunksize(av->top);
  mi.usmblks = av->max_total_mem;
  return mi;
}



void SharedData::mstats()
{
  struct mallinfo mi = mallinfo();

  fprintf(stderr, "max system bytes = %10lu\n",
          (unsigned long)(mi.usmblks));
  fprintf(stderr, "system bytes     = %10lu\n",
          (unsigned long)(mi.arena + mi.hblkhd));
  fprintf(stderr, "in use bytes     = %10lu\n",
          (unsigned long)(mi.uordblks + mi.hblkhd));
}



int SharedData::mallopt(int param_number, int value)
{
  mstate av = get_malloc_state();
  /* Ensure initialization/consolidation */
  malloc_consolidate();

  switch(param_number) {
  case M_MXFAST:
    if (value >= 0 && value <= MAX_FAST_SIZE) {
      av->max_fast = (value == 0)? 0: request2size(value);
      clear_consolidation_flag(av);
      return 1;
    }
    else
      return 0;

  case M_TRIM_THRESHOLD:
    av->trim_threshold = value;
    return 1;

  case M_TOP_PAD:
    av->top_pad = value;
    return 1;

  default:
    return 0;
  }
}

