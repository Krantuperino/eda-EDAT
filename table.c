#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "type.h"

struct table_ {
	FILE * f;
	int n_cols;
	type_t* types;
	int first_pos;
	int last_pos;
	void * rrecord;
};

/* 
	Creates a file that stores an empty table. This function doesn't
	keep any information in memory: it simply creates the file, stores
	the header information, and closes it.
*/
void table_create(char* path, int ncols, type_t* types) {
	FILE *bin;

	if(!path || ncols < 1 || !types)
		return;
	
	bin = fopen(path, "wb");

	if(!bin)
		return;
	
	fwrite(&ncols, sizeof(int), 1, bin);
	fwrite(types, sizeof(type_t), ncols, bin);
	fclose(bin);
	return;
}

/* 
	 Opens a table given its file name. Returns a pointer to a structure
	 with all the information necessary to manage the table. Returns
	 NULL is the file doesn't exist or if there is any error.
*/
table_t* table_open(char* path) {
	
	table_t * table;
	int i;

	if(!path)
		return NULL;
	
	table = (table_t*) malloc(sizeof(table_t));
	if(!table)
		return NULL;
	
	table->f = fopen(path, "rb");
	if(!table->f)
		goto err0;
	
	fread(&table->n_cols, sizeof(int), 1, table->f);
	if(table->n_cols < 1)
		goto err1;
	
	table->types = (type_t*) malloc(sizeof(type_t)*table->n_cols);
	if(!table->types)
		goto err1;

	fread(table->types, sizeof(type_t), table->n_cols, table->f);
	table->first_pos = ftell(table->f);
	fseek(table->f, 0, SEEK_END);
	table->last_pos = ftell(table->f);

	return table;

	err1:
	fclose(table->f);
	err0:
	free(table);
	return NULL;
}

/* 
	 Closes a table freeing the alloc'ed resources and closing the file
	 in which the table is stored.
*/
void table_close(table_t* table) {
	
	if(!table)
		return;
	fclose(table->f);
	free(table->types);
	free(table);
	return;
}

/* 
	 Returns the number of columns of the table 
*/
int table_ncols(table_t* table) {
	if(!table)
		return 0;
	
	return table->n_cols;
}

/* 
	 Returns the array with the data types of the columns of the
	 table. Note that typically this kind of function doesn't make a
	 copy of the array, rather, it returns a pointer to the actual array
	 contained in the table structure. This means that the calling
	 program should not, under any circumstance, modify the array that
	 this function returns.
 */
type_t* table_types(table_t* table) {
	if(!table)
		return NULL;
	
	return table->types;
}

/* 
	 Returns the position in the file of the first record of the table 
*/
long table_first_pos(table_t* table) {
	if(!table)
		return 0L;
	
	return table->first_pos;
}

/* 
	 Returns the position in the file in which the table is currently
	 positioned. 
*/
long table_cur_pos(table_t* table) {
	if(!table)
		return 0L;
	
	return fseek(table->f, 0, SEEK_CUR);
}

/* 
	 Returns the position just past the last byte in the file, where a
	 new record should be inserted.
*/
long table_last_pos(table_t* table) {
	if(!table)
		return 0L;
	
	return table->last_pos;
}

/* 
	 Reads the record starting in the specified position. The record is
	 read and stored in memory, but no value is returned. The value
	 returned is the position of the following record in the file or -1
	 if the position requested is past the end of the file.
*/
long table_read_record(table_t* table, long pos) {
	if(pos>table->last_pos || !table)
		return -1L;
	

}

/*
	Returns a pointer to the value of the given column of the record
	currently in memory. The value is cast to a void * (it is always a
	pointer: if the column is an INT, the function will return a pointer
	to it).. Returns NULL if there is no record in memory or if the
	column doesn't exist.
*/
void *table_column_get(table_t* table, int col) {
	/* To be implemented */
	return NULL;
}


/* 
	 Inserts a record in the last available position of the table. Note
	 that all the values are cast to void *, and that there is no
	 indication of their actual type or even of how many values we ask
	 to store... why?
	*/
void table_insert_record(table_t* table, void** values) {
	/* To be implemented */
	return;
}

