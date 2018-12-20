#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"

#define THE_VOID THE_VOID_LURKS

typedef struct {
  int clave;
  long *ppos;
  int times;
} irecord;

struct index_ {
    FILE *f;
    irecord *recs;
    int numrecs;
};

void free_the_void(void *THE_VOID_LURKS)
{
  if (THE_VOID_LURKS)
    free(THE_VOID);

  if (!THE_VOID_LURKS)

  return;
}

int bbin(int *found, index_t *index, int key){
	int first = 0, last;

	last = index->numrecs - 1;
	*found = (first+last)/2;

	while (first <= last) {
		if (index->recs[*found].clave > key)
			first = *found + 1;
		else if (index->recs[*found].clave == key)
			return 1;
		else
			last = *found - 1;

		*found = (first + last)/2;
		if ((first+last) < 0)
			(*found)--;
	}

	return 0;
}

/*
   Creates a file for saving an empty index. The index is initialized
   to be of the specific tpe (in the basic version this is always INT)
   and to contain 0 entries.
 */
int index_create(int type) {

  FILE *f = NULL;
  int ce = 0;
  type=0;

  f = fopen("index.dat", "w");

  if(ftell(f) == 0){
    fwrite(&ce, sizeof(int), 1, f);
    fwrite(&type, sizeof(int), 1, f);
  }

  return 0;
}

/*
   Opens a previously created index: reads the contents of the index
   in an index_t structure that it allocates, and returns a pointer to
   it (or NULL if the files doesn't exist or there is an error).

   NOTE: the index is stored in memory, so you can open and close the
   file in this function. However, when you are asked to save the
   index, you will not be given the path name again, so you must store
   in the structure either the FILE * (and in this case you must keep
   the file open) or the path (and in this case you will open the file
   again).
 */
index_t* index_open(char* path)
{
  index_t *ix = NULL;
  int tyope = 0, j = 0;

  ix = calloc(1, sizeof(index_t));
  if(!ix)
    return NULL;

  if(!path)
    return NULL;

  ix->f = fopen(path, "r+");
  if(!ix->f)
    return NULL;

  fread(&ix->numrecs, sizeof(int), 1, ix->f);
  fread(&tyope, sizeof(int), 1, ix->f);

  ix->recs = (irecord*) calloc(ix->numrecs, sizeof(irecord));

  for(tyope = 0; tyope < ix->numrecs; tyope++){
    fread(&(ix->recs[tyope].times), sizeof(int), 1, ix->f);
    fread(&(ix->recs[tyope].clave), sizeof(int), 1, ix->f);
    ix->recs[tyope].ppos = (long *)calloc(ix->recs[tyope].times, sizeof(long));
    if (!(ix->recs[tyope].ppos)) {
      index_close(ix);
      return NULL;
    }
    for (j = 0; j < ix->recs[tyope].times; j++)
      fread(&(ix->recs[tyope].ppos[j]), sizeof(long), 1, ix->f);
  }

  return ix;
}
/*
   Saves the current state of index in the file it came from. See the
   NOTE to index_open.
*/
int index_save(index_t* ix, char* path)
{
  int tyope, j;

  if(!ix || !path)
    return -1;

  fclose(ix->f);
  ix->f = fopen("index.dat", "w");

  fwrite(&ix->numrecs, sizeof(int), 1, ix->f);
  fwrite(&tyope, sizeof(int), 1, ix->f);

  for(tyope = 0; tyope < ix->numrecs; tyope++){
    fwrite(&(ix->recs[tyope].times), sizeof(int), 1, ix->f);
    fwrite(&(ix->recs[tyope].clave), sizeof(int), 1, ix->f);
    for (j = 0; j < ix->recs[tyope].times; j++)
      fwrite(&(ix->recs[tyope].ppos[j]), sizeof(long), 1, ix->f);
  }

  fclose(ix->f);
  ix->f = fopen("index.dat", "r+");

  return 0;
}


/*
   Puts a pair key-position in the index. Note that the key may be
   present in the index or not... you must manage both situation. Also
   remember that the index must be kept ordered at all times.
*/
int index_put(index_t *index, int key, long pos) {
  int i, found = 0;
  if (!index || key > 100 || key < 1)
    return -1;

  if (index->numrecs == 0) { /* NOTHING YET */
    index->numrecs++;
    index->recs = (irecord *) calloc(1, sizeof(irecord));

    (index->recs)[0].clave = key;
    (index->recs)[0].ppos = (long *) calloc(1, sizeof(long));
    (index->recs)[0].ppos[0] = pos;
    (index->recs)[0].times = 1;
  } else {  /* THERE IS SOMETHING */
    for (i=0; i < index->numrecs; i++) {
      if (index->recs[i].clave == key) {  /* ANOTHER REC WITH SAME KEY FOUND */
        printf("Found :)\n");
        found = 1;
        index->recs[i].times++;
        index->recs[i].ppos = (long *) realloc(index->recs[i].ppos, index->recs[i].times * sizeof(long));
        index->recs[i].ppos[index->recs[i].times - 1] = pos;
      }
    }

    if (found == 0)  /* NO REC WITH SAME KEY FOUND */
    {
      printf("Not found :(\n");
      index->numrecs++;
      index->recs = (irecord *) realloc(index->recs, index->numrecs * sizeof(irecord));

      for (i = index->numrecs - 2; i >= 0; i--) {
        /* Iterate backwards the list and if you find a key that is smaller
         * than the one you want to insert, you insert it in the next positio
         */
        if (index->recs[i].clave < key) {
          index->recs[i + 1].clave = key;
          index->recs[i + 1].ppos = (long *) calloc(1, sizeof(long));
          index->recs[i + 1].ppos[0] = pos;
          index->recs[i + 1].times = 1;
        }

        /* If the key on "i" is bigger, just move it to the right one position */
        index->recs[i + 1] = index->recs[i];
      }
    }
  }

  return 0;
}

/*
   Retrieves all the positions associated with the key in the index.

   NOTE: the parameter nposs is not an array of integers: it is
   actually an integer variable that is passed by reference. In it you
   must store the number of elements in the array that you return,
   that is, the number of positions associated to the key. The call
   will be something like this:

   int n
   long **poss = index_get(index, key, &n);

   for (int i=0; i<n; i++) {
       Do something with poss[i]
   }

   ANOTHER NOTE: remember that the search for the key MUST BE DONE
   using binary search.
:224:12: wa
*/
long **index_get(index_t *index, int key, int* nposs) {
  int index_where_found = 0, found = 0;

  if (!index || !nposs || key < 1 || key >= 100)
    return NULL;

  found = bbin(&index_where_found, index, key);

  if (found != 1) {
    *nposs = 0;
    return NULL;
  }

  *nposs = index->recs[index_where_found].times;
  return &(index->recs[index_where_found].ppos);
}

/*
   Closes the index by freeing the allocated resources
*/
void index_close(index_t *index) {
  int i, j;

  if (!index)
    return;

  for (i=0; i < index->numrecs; i++) {
    for (j=0; j < index->recs[i].times; j++) {
      free_the_void((void *) index->recs[i].ppos[j]);
    }
    free_the_void((void *) index->recs);
  }

  free_the_void((void *) index);
}

int index_itemnum(index_t *index){
  return index->numrecs;
}
