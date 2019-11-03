#include <stdio.h>
#include "cache.h"
#include "set.h"

int readcnt;
sem_t mutex, w;

Cache *init_cache(void) {
    Cache *cachep = malloc(sizeof(Cache));

    cachep->tot_size = 0;

    /* Init the set. */
    STinit();

    /* Init semaphores. */
    readcnt = 0;
    Sem_init(&mutex, 0, 1);
    Sem_init(&w, 0, 1);

    //printf("readcnt = %d\n", readcnt);
    return cachep;
}

int cache_reader(Cache *cachep, char *key, char *buf, int *size)
{
    Item itm;
    int found_item = 0;

    P(&mutex);
    readcnt++;
    if (readcnt == 1) /* First in */
        P(&w);
    V(&mutex);
    
    /* Critical section */
    itm = cache_get_item(cachep, key);
    if (!is_null(itm)) {
        /* Read data. */
        memcpy(buf, itm.data, itm.size);
        *size = itm.size;
        found_item = 1;
    }
    /* End of critical section */
    
    P(&mutex);
    readcnt--;
    if (readcnt == 0) /* Last out */
        V(&w);
    V(&mutex);

    return found_item;
}

void cache_writer(Cache *cachep, char *data, char *name, ssize_t size)
{
    P(&w);

    /* Critical section */
    cache_store(cachep, data, name, size);
    /* End of critical section */
    
    V(&w);
}

Item cache_get_item(Cache *cache, char *key)
{
    Key u;
    memset(u.kval, 0, MAX_LEN);
    strncpy(u.kval ,key, strlen(key));

    //printf("Searching item %s, %s, %d\n", u.kval, key, strlen(key));

    return STsearch(u);
}

void cache_store(Cache *cachep, char *data, char *name, ssize_t size)
{
    if ((cachep->tot_size + size) > MAX_CACHE_SIZE) {
        // TODO: LRU eviction policy.
        //printf("Reached max size !\n");
        return;
    }
    //printf("----------> cached uri = %s, %d\n", name, strlen(name));

    Item item;
    
    item_set_key(&item, name);
    item.size = size;
    item.data = (char *)Malloc(size);
    memcpy(item.data, data, size);

    // printf("coppied data, %d bytes \n", size);
    // printf("struct size, %d \n", sizeof(Item));

    STinsert(item);

    cachep->tot_size += size;
}
