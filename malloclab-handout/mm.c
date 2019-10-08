/*
 * mm.c - Segragated fits allocator.
 * Perf index = 49 (util) + 40 (thru) = 89/100.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "-",
    /* First member's full name */
    "Maor Shutman",
    /* First member's email address */
    "-",
    /* Second member's full name (leave blank if none) */
    "-",
    /* Second member's email address (leave blank if none) */
    "-"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*
 * If NEXT_FIT defined use next fit search, else use first-fit search 
 */
#define NEXT_FITx

/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<9)  /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) //line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Given block ptr bp, compute address of the predecessor and succesor */
#define PRED(bp) ((char *)(bp))
#define SUCC(bp) ((char *)(bp) + WSIZE)

int mm_check(int lineno);

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
//static void checkheap(int verbose, int lineno);
static void checkblock(void *bp);

#define checkheap(lineno) //mm_check(lineno)

/* Explicit free list */
#define NUM_CLASSES 32
void splice_node(void *bp);
char **get_root(int size);
char **next_list(char **list_root);

void print_seg_lists();
static int find_in_list(void *bp);
static int check_ptrs(void *bp);
static int check_free_blocks(char *root);
static int find_cycles(char *root);

static void place_realloc(void *ptr, size_t asize, size_t size);

/* 
 * mm_init - Initialize the memory manager 
 */
int mm_init(void)
{
    heap_listp = 0;

    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk((4+NUM_CLASSES)*WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0); /* Alignment padding */

    /* NULL pointers for segreagted free lists */
    for (int i = 1; i <= NUM_CLASSES; ++i) {
        PUT(heap_listp + (i*WSIZE), NULL);
    }

    PUT(heap_listp + ((NUM_CLASSES+1)*WSIZE), PACK(DSIZE, 1)); /* Prologue header */ 
    PUT(heap_listp + ((NUM_CLASSES+2)*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */ 
    PUT(heap_listp + ((NUM_CLASSES+3)*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += ((NUM_CLASSES+2)*WSIZE);

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;

    checkheap(__LINE__);

    return 0;
}

/* 
 * mm_malloc - Allocate a block with at least size bytes of payload 
 */
void *mm_malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;      

    if (heap_listp == 0){
        mm_init();
    }
    /* Ignore spurious requests. */
    if (size == 0) {
        return NULL;
    }

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE) {
        asize = 2*DSIZE;
    } else {
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    }

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);                

        checkheap(__LINE__);

        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL) {
        return NULL;
    }
    place(bp, asize);

    checkheap(__LINE__);

    return bp;
} 

/* 
 * mm_free - Free a block 
 */
void mm_free(void *bp)
{
    if (bp == 0) 
        return;

    size_t size = GET_SIZE(HDRP(bp));
    if (heap_listp == 0){
        mm_init();
    }

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);

    checkheap(__LINE__);
}

/*
 * mm_realloc - Checks if the next or prev blocks can be
 * used to avoid a malloc. 
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(ptr == NULL) {
        return mm_malloc(size);
    }
    
    size_t asize;   /* Adjusted block size */
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE) {
        asize = 2*DSIZE;
    } else {
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    }

    size_t csize = GET_SIZE(HDRP(ptr));

    if (asize <= csize) {
        place_realloc(ptr, asize, csize);
        return ptr;
    } 

    else {
        /* Need to check if have place in the next block */
        char *bp;
        char *prev = PREV_BLKP(ptr);
        char *next = NEXT_BLKP(ptr);
        size_t prev_alloc = GET_ALLOC(HDRP(prev));
        size_t next_alloc = GET_ALLOC(HDRP(next));

        size_t prev_size = 0;
        size_t next_size = 0;

        if (!next_alloc) {
            next_size = GET_SIZE(HDRP(next));

            if ((next_size + csize) > asize) {
                splice_node(next);
                place_realloc(ptr, asize, next_size+csize);
                return ptr;
            }
        }

        if (!prev_alloc) {
            prev_size = GET_SIZE(HDRP(prev));
            if ((prev_size + csize) > asize) {                
                size_t oldsize = GET_SIZE(HDRP(ptr)) - DSIZE;
                splice_node(prev);
                memmove(prev, ptr, oldsize);
                place_realloc(prev, asize, prev_size+csize);
                return prev;
            }
        }
    }

    /* If nothing worked we reach here */
    newptr = mm_malloc(size);
    
    /* If realloc() fails the original block is left untouched. */
    if(!newptr) {
        return 0;
    }

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    if(size < oldsize) {
        oldsize = size; 
    }
    memcpy(newptr, ptr, oldsize - WSIZE);
 
    /* Free the old block. */
    mm_free(ptr);
    return newptr;
}

static void place_realloc(void *ptr, size_t asize, size_t size)
{   
    char *bp;
    if ((size - asize) >= (2*DSIZE)) {
        PUT(HDRP(ptr), PACK(asize, 1));
        PUT(FTRP(ptr), PACK(asize, 1));
        bp = NEXT_BLKP(ptr);
        PUT(HDRP(bp), PACK(size-asize, 0));
        PUT(FTRP(bp), PACK(size-asize, 0));

        insert_root(bp);
        coalesce(bp);
    } else {
        PUT(HDRP(ptr), PACK(size, 1));
        PUT(FTRP(ptr), PACK(size, 1));
    }
}

/* 
 * mm_check - Check the heap for correctness
 */
int mm_check(int lineno)
{
    printf("heapchecker on line %d \n", lineno);

    int verbose = 0;
    char *bp = heap_listp;

    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (verbose) 
            printblock(bp);
        checkblock(bp);
    }

    if (verbose)
        printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
        printf("Bad epilogue header\n");

    /* Check that all blocks in the free list are free */
    char **start = (char **)(heap_listp - ((NUM_CLASSES+1)*WSIZE));
    char **root;
    for (int i = 0; i < NUM_CLASSES; ++i) {
        root = (char **)((char *)start + i*WSIZE);

        if (check_free_blocks(*root)) {
            printf("Error: block in free list is not \n");
            exit(1);
        }

        if (find_cycles(*root)) {
            printf("Error: cycle \n");
            exit(1);
        }
    }
}

/* 
 * The remaining routines are internal helper routines 
 */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)  
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2*DSIZE)) {
        splice_node(bp);

        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));

        insert_root(bp);
        coalesce(bp);
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));

        /* Delete the free list node at bp */
        splice_node(bp);
    }
}

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
static void *find_fit(size_t asize)
{
    /* First-fit search */
    char **list_root;
    char *bp;
    
    /* Run over size classes */
    for (list_root = get_root(asize); list_root; list_root = next_list(list_root)) {
        bp = *list_root;

        if (!bp)
            continue; //return NULL;

        do {
            if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
                return (void *)bp;
            }        
        } while ((bp = GET(SUCC(bp))) != NULL);    
    }

    return NULL; /* No fit */
}

static void printblock(void *bp) 
{
    size_t hsize, halloc, fsize, falloc;

    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));  
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    if (hsize == 0) {
        printf("%p: EOL\n", bp);
        return;
    }

    printf("%p: header: [%ld:%c] footer: [%ld:%c]\n", bp,
           hsize, (halloc ? 'a' : 'f'),
           fsize, (falloc ? 'a' : 'f'));
}

static void checkblock(void *bp) 
{
    if ((size_t)bp % 8)
        printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Error: header does not match footer\n");

    /* Chech that all free blocks are in the free list */
    if (!GET_ALLOC(HDRP(bp))) {
        if (!find_in_list(bp)) {
            printf("Error: free block not in free list\n");
        }
    }

    /* Check list pointers concistancy */
    if (!GET_ALLOC(HDRP(bp))) {
        if ((bp != heap_listp) && !check_ptrs(bp)) {
            printf("Error: inconsistent block pointers\n");
            exit(1);
        }
    }

    /* Check that block is in the limmits of the heap */
    if ((bp > mem_heap_hi()) || (bp < mem_heap_lo())) {
        printf("Error: block not in heap bounds\n");
        exit(1);
    }
}

static int check_ptrs(void *bp) {
    char *pred = GET(PRED(bp));
    char *succ = GET(SUCC(bp));

    if (pred) {
        char *succ_pred = GET(SUCC(pred));
        if (succ_pred != bp)
            return 0;
    }

    if (succ) {
        char *pred_succ = GET(PRED(succ));
        if (pred_succ != bp)
            return 0;
    }

    return 1;
}

static int check_free_blocks(char *root) {
    for (char *p = root; p; p = GET(SUCC(p))) {
        if (p && GET_ALLOC(p)) { 
            return 1; 
        }
    }
    return 0;
}

static int find_cycles(char *root) {
    if (!root) { return 0; }
    char *slow = root;
    char *fast = root;

    while (1) {
        if (fast) { fast = GET(SUCC(fast)); }
        else { return 0; }
        if (fast) { fast = GET(SUCC(fast)); }
        else { return 0; }

        if (slow) { slow = GET(SUCC(slow)); }

        if (slow == fast) {
            return 1;
        }
    }
}

static int find_in_list(void *bp) {
    char **start = (char **)(heap_listp - ((NUM_CLASSES+1)*WSIZE));
    char **root;
    for (int i = 0; i < NUM_CLASSES; ++i) {
        root = (char **)((char *)start + i*WSIZE);
        for (char *p = *root; p; p = GET(SUCC(p))) {
            if (p == (char *)bp) {
                return 1;
            }
        }
    }
    
    exit(1);
    return 0;
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) 
{
    //printf("coalesce \n");

    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        insert_root(bp);
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        splice_node(NEXT_BLKP(bp));
        //insert_root(bp);

        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        insert_root(bp);
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        splice_node(PREV_BLKP(bp));
        //insert_root(PREV_BLKP(bp));

        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));

        insert_root(PREV_BLKP(bp));

        bp = PREV_BLKP(bp);
    }

    else {                                     /* Case 4 */
        splice_node(NEXT_BLKP(bp));
        splice_node(PREV_BLKP(bp));
        //insert_root(PREV_BLKP(bp));

        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        
        insert_root(PREV_BLKP(bp));

        bp = PREV_BLKP(bp);
    }

    return bp;
}

void splice_node(void *bp)
{
    char **list_root = get_root(GET_SIZE(HDRP(bp)));

    char *pred_bp = GET(PRED(bp));
    char *succ_bp = GET(SUCC(bp));

    if (pred_bp && succ_bp) {
        PUT(SUCC(pred_bp), succ_bp);
        PUT(PRED(succ_bp), pred_bp);
    }
    else if (!pred_bp && succ_bp) {
        /* Delete root */
        *list_root = succ_bp;
        PUT(PRED(*list_root), NULL);
    }
    else if (pred_bp && !succ_bp) {
        /* Delete last node */
        PUT(SUCC(pred_bp), NULL);
    }
    else /* (!pred_bp && !next_succ) */ {
        /* Delete last node */
        *list_root = NULL;
    }
}

void insert_root(char *new_root)
{
    char **list_root = get_root(GET_SIZE(HDRP(new_root)));

    if (new_root == (*list_root)) {
        return;
    }

    char *tmp_root = *list_root;
    *list_root = (char *)new_root;

    PUT(PRED(*list_root), NULL);
    PUT(SUCC(*list_root), tmp_root);

    if (tmp_root) {
        PUT(PRED(tmp_root), *list_root);
    }
}

char **get_root(int size) {
    int r = 0;
    while (size >>= 1) { ++r; }
    r = MIN(NUM_CLASSES-1, r);
    return (char **)(heap_listp - ((NUM_CLASSES+1)*WSIZE) + r*WSIZE);
}

char **next_list(char **list_root) {
    int diff = (int)(heap_listp - (char *)list_root);
    if (diff == DSIZE) {
        return NULL; /* list_root alredy points to the largest class */
    } else {
        return (char **)((char *)list_root + WSIZE);
    }
}

void print_seg_lists() {
    char **start = (char **)(heap_listp - ((NUM_CLASSES+1)*WSIZE));
    char **root;

    for (int i = 0; i < NUM_CLASSES; ++i) {
        root = (char **)((char *)start + i*WSIZE);
        printf("%d, %x", i, root);

        for (char *p = *root; p; p = GET(SUCC(p))) {
            if (p) {
                printf(" -> %d ", GET_SIZE(HDRP(p)));
            }
        }
        printf("\n");
    }
}
