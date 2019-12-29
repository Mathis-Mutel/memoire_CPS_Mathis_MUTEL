#include <stdio.h>
#include <stdlib.h>

int main(){
	printf("Test 1 : \n");
	void* a =malloc(4);
	void* b =malloc(16);
	void* c =malloc(35);
	void* d =malloc(1);
	void* e =malloc(64);
	void* f =malloc(4);
	void* g =malloc(3);

	free(b); 
	free(c);
	free(e);
	free(d);
	free(a);
	free(g);
	free(f);

	a =malloc(2);
	b= malloc(35);
	c=malloc(4);

	free(b);

	b = malloc(8);

	free(a);
	free(b);
	free(c);
	return 0;
}