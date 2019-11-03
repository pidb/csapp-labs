#ifndef __PROXY_CACHE_H__
#define __PROXY_CACHE_H__

#include "csapp.h"
#include "set.h"

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* Global variables */
extern int readcnt; /* Initially = 0 */
extern sem_t mutex, w; /* Both initially = 1 */

typedef struct {
	// the root is global for now
	// STlink set_root;
	size_t tot_size;
} Cache;

Cache *init_cache(void);

int cache_reader(Cache *cache, char *key, char *buf, int *size);
void cache_writer(Cache *cachep, char *data, char *name, ssize_t size);

Item cache_get_item(Cache *cache, char *key);
void cache_store(Cache *cachep, char *data, char *name, ssize_t size);

#endif
