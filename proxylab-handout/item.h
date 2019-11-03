#ifndef __SET_ITEM_H__
#define __SET_ITEM_H__

#include <sys/types.h>
#define MAX_LEN 8192

typedef struct item_key {
	char kval[MAX_LEN];
} Key;

typedef struct set_item {
	ssize_t size;
	char *data;
	Key name;
} Item;

Key key(Item item);
int eq(Key u, Key v);
int less(Key u, Key v);
int is_null(Item item);
void item_set_key(Item *item, char *s);

#endif
