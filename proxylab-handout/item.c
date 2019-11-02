#include "item.h"

Key key(Item item) {
	return item.name;
}

int eq(Key u, Key v) {
	return (!strcmp(u, v));
}

int less(Key u, Key v) {
	return (strcmp(u, v) < 0);
}

int is_null(Item item) {
	return !key(item);
}