#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NB_TESTS 10


	int test1(){
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
	int test2(){
		printf("Test 2 :\n");
	void* a = malloc(sizeof(int));
	void* b = malloc(sizeof(double));
	void* c = malloc(7);
	void* d = malloc(9);
	void* e = malloc(10);
	void* f = malloc(35);
	void* g = malloc(64);


	free(a);
	free(g);
	free(e);
	free(c);
	free(f);
	free(b);
	free(d);

	a = malloc(4*sizeof(int));

	free(a);
	return 0;
}

int test3(){
	int i;
	void* a[8000];
	//free(a);
	for(i=0;i<8111;i++)
	{
	 	a[i]=malloc(0);
		if(a[i]!= NULL)
		{
				printf("%i\n",i);
		}
	}
	for(;i>0;i--)
	{
		free(a[i]);
	}
	return 0;
}

	int main(int argc, char *argv[]) {
		fprintf(stderr, "Test réalisant de multiples fois une initialisation suivie d'une alloc max.\n"
				"Définir DEBUG à la compilation pour avoir une sortie un peu plus verbeuse."
	 		"\n");
		for (int i=0; i<NB_TESTS; i++) {
			debug("Initializing memory\n");
			mem_init(get_memory_adr(), get_memory_size());
			alloc_max(get_memory_size());
		}
		//test1();
		//test2();
		test3();
	// TEST OK
	return 0;
}
