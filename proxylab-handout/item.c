#include "item.h"

Key key(Item item) {
	return item.name;
}

int eq(Key u, Key v) {
	return (!strcmp(u.kval, v.kval));
}

int less(Key u, Key v) {
	return (strcmp(u.kval, v.kval) < 0);
}

int is_null(Item item) {
	return (!strcmp(key(item).kval, ""));
}

void item_set_key(Item *item, char *s) {
	memset((item->name).kval, 0, MAX_LEN);
	strncpy((item->name).kval, s, strlen(s));
}
