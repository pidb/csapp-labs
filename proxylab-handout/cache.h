#ifndef __CACHE_H__
#define __CACHE_H__

#include "csapp.h"
#include "set.h"

/* Global variables */
extern int readcnt; /* Initially = 0 */
extern sem_t mutex, w; /* Both initially = 1 */

typedef struct {
	// the root is global for now
	// STlink set_root;
	size_t tot_size;
} Cache;

Cache *init_cache(void);

void cache_reader(void);
void cache_writer(Cache *cachep, char *data, char *name, ssize_t size);

Item cache_get_item(Cache *cache, Key key);
void cache_store(Cache *cachep, char *data, char *name, ssize_t size);

#endif
