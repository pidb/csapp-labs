#include "set.h"

// TODO: Exercise - Change s.t. there are no global variables.
static STlink head, z;
const Item NULLitem = {NULL, 0, NULL};

STlink NEW(Item item, STlink l, STlink r, int N)
{
	STlink x = malloc(sizeof *x);
	x->item = item;
	x->l = l; 
	x->r = r;
	x->N = N;
	return x;
}

void STinit()
{ 
	head = (z = NEW(NULLitem, 0, 0, 0)); 
}

int STcount() { return head->N; }

Item searchR(STlink h, Key v)
{ 
	Key t = key(h->item);
	if (h == z) { return NULLitem; }
	if (eq(v, t)) { return h->item; }
	if (less(v, t)) { return searchR(h->l, v); }
	else 		  { return searchR(h->r, v); }
}

Item STsearch(Key v)
{ 
	return searchR(head, v); 
} 

STlink insertR(STlink h, Item item)
{ 
	Key v = key(item), t = key(h->item);

	if (h == z) { return NEW(item, z, z, 1); }
	if (rand() < RAND_MAX / (h->N+1))
	return insertT(h, item);
	
	if (less(v, t)) { h->l = insertR(h->l, item); }
	else 		    { h->r = insertR(h->r, item); }

	(h->N)++; 
	return h; 
}

void STinsert(Item item)
{
	head = insertR(head, item);
}

STlink rotR(STlink h)
{
	STlink x = h->l; 
	h->l = x->r; 
	x->r = h;
	return x;
} 

STlink rotL(STlink h)
{
	STlink x = h->r;
	h->r = x->l;
	x->l = h;
	return x;
}

STlink insertT(STlink h, Item item) 
{
	Key v = key(item);
	if (h == z) { return NEW(item, z, z, 1); }
	
	if (less(v, key(h->item)))
	{
		h->l = insertT(h->l, item); 
		h = rotR(h); 
	} else {
		h->r = insertT(h->r, item);
		h = rotL(h);
	}
	return h;
}

STlink partR(STlink h, int k) 
{ 	
	int t = h->l->N;
	if(t > k) { 
		h->l = partR(h->l, k); 
		h = rotR(h);
	}	
	if(t < k) { 
		h->r = partR(h->r, k-t-1); 
		h = rotL(h); 
	}
	return h;
}

STlink joinLR(STlink a, STlink b)
{
	if (b == z) return a;
	b = partR(b, 0);
	b->l = a; 
	return b;
}

STlink deleteR(STlink h, Key v)
{
	STlink x; 
	Key t = key(h->item);

	if (h == z) return z;
	if (less(v, t)) { h->l = deleteR(h->l, v); }
	if (less(t, v)) { h->r = deleteR(h->r, v); };
	if (eq(v, t)) {
		x = h; 
		h = joinLR(h->l, h->r); 
		free(x);
	}
	return h;
}

void STdelete(Key v)
{
	head = deleteR(head, v);
}

STlink STjoin(STlink a, STlink b)
{
	if (b == z) return a;
	if (a == z) return b;
	b = insertT(b, a->item);
	b->l = STjoin(a->l, b->l);
	b->r = STjoin(a->r, b->r); 
	free(a);
	return b;
}

// DEBUG
void sortR(STlink h)
  { 
    if (h == z) return;
    sortR(h->l);
    printf("%s \n", (h->item).name); 
    sortR(h->r);
  }
void STsort(void)
  { sortR(head); } 

// int main() {
// 	STinit();

// 	Item i1 = {"adfgh", 0, "a"};
// 	Item i2 = {"b4asdf", 0, "a"};
// 	Item i3 = {"c", 0, "a"};
// 	Item i4 = {"aa", 0, "a"};
// 	Item i5 = {"as", 0, "a"};
// 	Item i6 = {"cvb", 0, "a"};
// 	Item i7 = {"asd", 0, "a"};
// 	Item i8 = {"zsd", 0, "a"};
// 	Item i9 = {"csd", 0, "a"};

// 	STinsert(i1);
// 	STinsert(i2);
// 	STinsert(i3);
// 	STinsert(i4);
// 	STinsert(i5);
// 	STinsert(i6);
// 	STinsert(i7);
// 	STinsert(i8);
// 	STinsert(i9);

// 	//printf("%d\n", less("abce", "abcd"));

// 	STsort();

// 	return 0;
// }
