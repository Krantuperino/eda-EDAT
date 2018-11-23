#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

size_t
value_length(type_t type, void* value) {
    switch(type) {
        case INT:
            return sizeof(int);
        case STR:
            return (strlen((char*) value) + 1) * sizeof(char);
        default:
            return 0;
    }
}

void
print_value(FILE* f, type_t type, void* value) {
    switch(type) {
        case INT:
            fprintf(f, "%6d", *((int*) value));
            break;
        case STR:
            fprintf(f, "%s", (char*) value);
            break;
    }
}


