#include "mem.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

// constante définie dans gcc seulement
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define ALIGNMENT 16
#endif

#define allign(val,a) ( ( (val) + (a-1) ) & (a-1))
/* structure placée au début de la zone de l'allocateur

   Elle contient toutes les variables globales nécessaires au
   fonctionnement de l'allocateur
*/
struct allocator_header {
        size_t memory_size;
	mem_fit_function_t *fit;
  struct fb *first_zl;
};

/* La seule variable globale autorisée
 * On trouve à cette adresse le début de la zone à gérer
 * (et une structure 'struct allocator_header)
 */
static void* memory_addr;

static inline void *get_system_memory_addr() {
	return memory_addr;
}

static inline struct allocator_header *get_header() {
	struct allocator_header *h;
	h = (struct allocator_header*)get_system_memory_addr();
	return h;
}

static inline size_t get_system_memory_size() {
	return get_header()->memory_size;
}


struct fb {
	size_t size;
	struct fb* next;
	/* ... */
};


void mem_init(void* mem, size_t taille)
{
  memory_addr = mem;
        //*(size_t*)memory_addr = taille;
  struct allocator_header *header=(struct allocator_header *)mem;
  header->memory_size = taille;
  	mem_fit(&mem_fit_first);
  //header->fit = mem_fit(&mem_fit_first);
	assert(mem == get_system_memory_addr());
	assert(taille == get_system_memory_size());
  struct fb *zl = (struct fb*)(header+1);
  zl->size = taille - sizeof(struct allocator_header);
  zl->next = NULL;
  header->first_zl= zl;
}

void mem_show(void (*print)(void *, size_t, int)) {
  //struct fb* zl=(struct fb *) memory_addr + sizeof(struct allocator_header);//premiere zone libre
  struct fb* zl= get_header()->first_zl;
  void *current=get_memory_adr();
	void *fin_first=current + get_memory_size();//le + est ok car current et fin_first "void*"
    while (current < fin_first) {
    	size_t taille_zone= ((struct fb*)current)->size;
    	int libre=0;
    	if (current == zl) {
    		libre=1;
    		zl = zl->next;
    	}
    	print(current, taille_zone, libre);
    	current = current + taille_zone;
    }
}

void mem_fit(mem_fit_function_t *f) {
	get_header()->fit = f;
}

/*void *mem_alloc(size_t taille) {
	__attribute__((unused))
	struct fb *fb=get_header()->fit(get_header()->first_zl, get_header()->memory_size);
  int taille_zo;
  if( taille % 8!=0)
    taille_zo= taille /8 *8 + 8;
  else
    taille_zo= taille;
taille_zo += sizeof(int);

	return NULL;
}


void mem_free(void* mem) {
}*/

void *mem_alloc(size_t taille) {
	__attribute__((unused)) /* juste pour que gcc compile ce squelette avec -Werror */
  size_t taille_tot = taille+sizeof(size_t);
  struct fb *first_fb=get_header()->first_zl;
	struct fb *fb=get_header()->fit(first_fb, taille_tot);
  size_t *lg;
	if (fb->size >= taille_tot + sizeof(struct fb)){
    fb->size -= taille_tot;
    lg = (size_t*) (fb + fb->size);
    * lg = taille_tot;
  }else if (fb == first_fb){
    first_fb = fb->next;
    lg = (size_t*) fb;
    * lg = fb->size;
  }else{
    struct fb* fb_prec = first_fb;
    while (fb_prec->next != fb) {
      fb_prec = fb_prec->next;
    }
    fb_prec->next = (fb_prec->next)->next;
    lg = (size_t*) fb;
    * lg = fb->size;
  }
	return lg + sizeof(int);
}


void mem_free(void* mem) {
  size_t* taille = (size_t*)mem - sizeof(size_t);
  struct fb* fb;
  struct fb* first_fb= get_header()->first_zl;
  if (mem < (void *) first_fb){ //si première zone
    fb = (struct fb*) mem - sizeof(size_t);
    fb->size = *taille;
    fb->next = first_fb;
    first_fb = fb;
  }else{
    struct fb* fb_prec = first_fb;
    while ((void *)(fb_prec->next) < mem) {
      fb_prec = fb_prec->next;
    }
    if (fb_prec+(fb_prec->size)+sizeof(size_t) == mem){//si zone précédente libre
      fb_prec->size += *taille;
      fb = fb_prec;
    }else{ //si zone précédente non libre
      fb = (struct fb*) mem - sizeof(size_t);
      fb->size = *taille;
      fb->next = fb_prec->next;
      fb_prec->next = fb;
    }
  }
  if (fb->next == fb+(fb->size)){//si zone suivante libre
    fb->size += (fb->next)->size;
    fb->next = (fb->next)->next;
  }
}


struct fb* mem_fit_first(struct fb *list, size_t size) {
  struct fb *tmp = list;
 while (tmp != NULL) {
   if (tmp->size >= size)
     return tmp;
   tmp = tmp->next;
 }
 return NULL;
}

/* Fonction à faire dans un second temps
 * - utilisée par realloc() dans malloc_stub.c
 * - nécessaire pour remplacer l'allocateur de la libc
 * - donc nécessaire pour 'make test_ls'
 * Lire malloc_stub.c pour comprendre son utilisation
 * (ou en discuter avec l'enseignant)
 */
size_t mem_get_size(void *zone) {
	/* zone est une adresse qui a été retournée par mem_alloc() */

	/* la valeur retournée doit être la taille maximale que
	 * l'utilisateur peut utiliser dans cette zone */
	return 0;
}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb* mem_fit_best(struct fb *list, size_t size) {
  struct fb *min = list;
  struct fb *current = list;
  while (current != NULL) { // On parcourt la liste
    if (current->size - size > 0 && current->size < min->size)
      min = current; // si le min trouvé précedemment a une taille superieur à
                     // l'élément courant, on récupère cet élément courant
    current = current->next; // On avance dans la liste chainée
  }
  return min;
}

struct fb* mem_fit_worst(struct fb *list, size_t size) {
  struct fb *max = list;
  struct fb *current = list;
  while (current != NULL) {
    if (current->size - size > 0 &&
        current->size >
            max->size) /*pareil que pour le best sauf qu'on modifie la
                            comparaison pour trouver le max */
      max = current;
    current = current->next;
  }
  return max;
}
