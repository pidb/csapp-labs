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

    printf("readcnt = %d\n", readcnt);
    return cachep;
}

void cache_reader(void)
{
    P(&mutex);
    readcnt++;
    if (readcnt == 1) /* First in */
        P(&w);
    V(&mutex);
    
    /* Critical section */
    /* Reading happens */
    
    P(&mutex);
    readcnt--;
    if (readcnt == 0) /* Last out */
        V(&w);
    V(&mutex);

}

void cache_writer(Cache *cachep, char *data, char *name, ssize_t size)
{
    P(&w);

    /* Critical section */
    cache_store(cachep, data, name, size);

    V(&w);
}

Item cache_get_item(Cache *cache, Key key)
{
    return STsearch(key);
}

void cache_store(Cache *cachep, char *data, char *name, ssize_t size)
{
    Item item;

    printf("----------> cached uri = %s, %d\n", name, strlen(name));

    strncpy(item.name, name, strlen(name));
    item.size = size;

    // item.data = (char *)Malloc(size);
    // memcpy(item.data, data, size);

    printf("coppied data \n");
    printf("struct size, %d \n", sizeof(Item));

    //STinsert(item);
    test_item(item);

    //printf("%s\n", head->item->name);

    cachep->tot_size += size;
}
