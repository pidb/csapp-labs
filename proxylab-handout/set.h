#ifndef __SET_BST_H__
#define __SET_BST_H__

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "item.h"

typedef struct STnode* STlink;

struct STnode {
	Item item;
	STlink l, r;
	int N; 
};

STlink NEW(Item item, STlink l, STlink r, int N);
void STinit();
int STcount();
Item searchR(STlink h, Key v);
Item STsearch(Key v);
STlink insertR(STlink h, Item item);
void STinsert(Item item);
STlink rotR(STlink h);
STlink rotL(STlink h);
STlink insertT(STlink h, Item item);
STlink partR(STlink h, int k);
STlink joinLR(STlink a, STlink b);
STlink deleteR(STlink h, Key v);
void STdeletye(Key v);
STlink STjoin(STlink a, STlink b);

void test_item(Item item);

#endif
