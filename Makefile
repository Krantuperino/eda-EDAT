CC = gcc
CFLAGS = -pedantic -Wall -ggdb
CLIBS = -lodbc
EXE = test_table score

all : $(EXE)
.PHONY : clean

clean :
	rm -rf *.o dummy_table.dat score.dat $(EXE)

$(EXE) : % : %.o table.o type.o odbc.o
	@echo "# Compiling $@"
	$(CC) $(CFLAGS) -o $@ $< table.o type.o odbc.o $(CLIBS)

odbc.o : odbc.c odbc.h
	@echo "# Generating $@"
	$(CC) $(CFLAGS) -c $< $(CLIBS)

table.o : table.c table.h
	@echo "# Generating $@"
	$(CC) $(CFLAGS) -c $<

type.o : type.c type.h
	@echo "# Generating $@"
	$(CC) $(CFLAGS) -c $<
