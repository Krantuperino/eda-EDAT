CC = gcc
CFLAGS = -pedantic -Wall -ggdb
EXE = table_test test_table

all : $(EXE) 
.PHONY : clean

clean : 
	rm -rf *.o $(EXE)

$(EXE) : % : %.o table.o type.o
	@echo "# Compiling $@"
	$(CC) $(CFLAGS) -o $@ $< table.o type.o

table.o : table.c table.h
	@echo "# Generating $@"
	$(CC) $(CFLAGS) -c $^

type.o : type.c type.h
	@echo "# Generating $@"
	$(CC) $(CFLAGS) -c $^