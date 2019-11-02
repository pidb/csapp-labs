#ifndef __SET_ITEM_H__
#define __SET_ITEM_H__

#include <sys/types.h>
#define MAX_LEN 200

typedef char* Key;

typedef struct set_item {
	ssize_t size;
	char *data;
	char name[MAX_LEN];
} Item;

Key key(Item item);
int eq(Key u, Key v);
int less(Key u, Key v);

#endif
